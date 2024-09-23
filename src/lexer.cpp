//
// Created by Schnond Chansuk on 29/8/2024 AD.
//

#include "../include/lexer.h"
#include <bitset>
#include <cstring>
#include <iostream>

static constexpr std::bitset<256> isAlphaTable = [](){
    std::bitset<256> table;
    for (int i = 0; i < 256; ++i) 
    {
        if ((i >= 'a' && i <= 'z') || (i >= 'A' && i <= 'Z')) {
            table.set(i);
        }
    }
    return table;
}();

static constexpr std::bitset<256> isDigitTable = [](){
    std::bitset<256> table;
    for (int i = '0'; i <= '9'; ++i) 
    {
        table.set(i);
    }
    return table;
}();

static constexpr std::bitset<256> isXdigitTable = [](){
    std::bitset<256> table = isDigitTable;
    for (int i = 'a'; i <= 'f'; ++i) 
    {
        table.set(i);
        table.set(i - 32);  // Set uppercase version
    }
    return table;
}();

static constexpr std::bitset<256> is_alnum_table = isAlphaTable | isDigitTable;

static constexpr std::array<std::pair<std::string_view, token_type>, 25> keyword_t = {{
    // Data type keywords
    { "true", KEYWORD },
    { "false", KEYWORD },
    { "null", KEYWORD },

    // Linkage keywords
    { "package", KEYWORD },
    { "import", KEYWORD },
    { "from", KEYWORD },

    // Declarative keywords
    { "var", KEYWORD },
    { "const", KEYWORD },
    { "function", KEYWORD },
    { "return", KEYWORD },

    // Control flow keywords
    { "if", KEYWORD },
    { "else", KEYWORD },
    { "for", KEYWORD },
    {"while", KEYWORD },

    // Object-oriented keywords
    { "class", KEYWORD },
    { "static", KEYWORD },
    { "virtual", KEYWORD },
    { "inherits", KEYWORD },
    { "final", KEYWORD }
    /* ... */
}};

static constexpr std::array<std::pair<std::string_view, token_type>, 10> type_t = {{
    { "i32", TYPE },
    { "i64", TYPE },
    { "u32", TYPE },
    { "u64", TYPE },
    { "f32", TYPE },
    { "f64", TYPE },
    { "string", TYPE },
    { "boolean", TYPE },
    { "void", TYPE },
    { "auto", TYPE }
}};

static const std::unordered_set<std::string_view> twoCharOp_t = {
    "->", "==", "!=", "<=", ">=", "&&", "||", "<<", ">>",
    "++", "--", "+=", "-=", "*=", "/=", "%=", "&=", "|=",
    "^=", "<<=", ">>="
};

std::vector<std::string> errorSet;

inline void reportError(const std::string_view msg)
{
    errorSet.emplace_back(msg);
    std::cerr << msg << std::endl;
}

constexpr bool isAlpha(char c)
{
    return isAlphaTable[static_cast<unsigned char>(c)];
}

constexpr bool isDigit(char c)
{
    return isDigitTable[static_cast<unsigned char>(c)];
}

constexpr bool isXdigit(char c)
{
    return isXdigitTable[static_cast<unsigned char>(c)];
}

constexpr bool isAlnum(char c)
{
    return is_alnum_table[static_cast<unsigned char>(c)];
}

void lexerInit(lexer_t& lexer, std::string_view source)
{
    lexer.source = source;
    lexer.position = 0;
    lexer.state = START;
    lexer.current_char = source.empty() ? '\0' : source[0];
    lexer.line = 1;
    lexer.column = 1;
    lexer.tokens.clear();
}

void advance(lexer_t& lexer)
{
    if (lexer.position < lexer.source.size())
    {
        lexer.current_char = lexer.position < lexer.source.size() ? lexer.source[++lexer.position] : '\0';
        ++lexer.column;
        if (lexer.current_char == '\n')
        {
            ++lexer.line;
            lexer.column = 1;
        }
    }
    else
    {
        lexer.current_char = '\0';
    }
}

char peekNext(const lexer_t& lexer)
{
    return lexer.position + 1 < lexer.source.size() ? lexer.source[lexer.position + 1] : '\0';
}

void skipWhitespaceAndComment(lexer_t& lexer)
{
    repeat_skip:
    while (lexer.current_char != '\0' && std::isspace(lexer.current_char))
    {
        advance(lexer);
    }
    if (lexer.current_char == '/')
    {
        if (peekNext(lexer) == '/')
        {
            advance(lexer);
            advance(lexer);
            while (lexer.current_char != '\0' && lexer.current_char != '\n')
                advance(lexer);
            if (lexer.current_char == '\n')
            {
                advance(lexer);
                goto repeat_skip;
            }
        }
        else if (peekNext(lexer) == '*')
        {
            advance(lexer);
            advance(lexer);
            while (lexer.current_char != '\0' && !(lexer.current_char == '*' && peekNext(lexer) == '/'))
            {
                advance(lexer);
                advance(lexer);
            }
            if (lexer.current_char == '*' && peekNext(lexer) == '/')
            {
                advance(lexer);
                advance(lexer);
                goto repeat_skip;
            }
        }
    }
}

token_t handleIdentifier(lexer_t& lexer)
{
    const std::string_view::size_type start = lexer.position;
    while (isAlnum(lexer.current_char) || lexer.current_char == '_')
    {
        advance(lexer);
    }
    std::string_view identifier = lexer.source.substr(start, lexer.position - start);
    const auto type_it = std::find_if(type_t.begin(), type_t.end(), [&](const auto &t)
    {
        return t.first == identifier;
    });

    if (type_it != type_t.end())
    {
        if (lexer.current_char == '?')
        {
            advance(lexer);
            return { NULLABLE_TYPE, lexer.source.substr(start, lexer.position - start) };
        }
        return { type_it->second, identifier };
    }

    const auto keyword_it = std::find_if(keyword_t.begin(), keyword_t.end(), [&](const auto &t)
    {
        return t.first == identifier;
    });

    if (keyword_it != keyword_t.end())
    {
        return { keyword_it->second, identifier };
    }
    return { IDENTIFIER, identifier };
}

token_t handleLiteral(lexer_t& lexer)
{
    const std::string_view::size_type start = lexer.position;
    if (lexer.current_char == '-' || lexer.current_char == '+')
        advance(lexer);

    if (lexer.current_char == '0' && (peekNext(lexer) == 'x' || peekNext(lexer) == 'X'))
    {
        advance(lexer);
        advance(lexer);
        while (isXdigit(lexer.current_char))
            advance(lexer);
    }
    else if (lexer.current_char == '0' && isDigit(peekNext(lexer)))
    {
        advance(lexer);
        while (lexer.current_char >= '0' && lexer.current_char <= '7')
            advance(lexer);
    }
    else
    {
        while (lexer.current_char)
            advance(lexer);
        if (lexer.current_char == '.')
        {
            advance(lexer);
            while (lexer.current_char)
                advance(lexer);
        }

        if (lexer.current_char == 'e' || lexer.current_char == 'E')
        {
            advance(lexer);
            if (lexer.current_char == '+' || lexer.current_char == '-')
                advance(lexer);
            while (isDigit(lexer.current_char))
                advance(lexer);
        }
    }
    return { LITERAL, lexer.source.substr(start, lexer.position - start) };
}

token_t handleOperator(lexer_t& lexer)
{
    if (lexer.current_char == '-' && isDigit(peekNext(lexer)))
        return handleLiteral(lexer);

    const std::string_view::size_type start = lexer.position;
    const char first_char = lexer.current_char;
    advance(lexer);

    if (lexer.current_char != '\0')
    {
        if (const std::string_view two_char_op(lexer.source.data() + start, 2); twoCharOp_t.contains(two_char_op))
        {
            advance(lexer);
            return { OPERATOR, two_char_op };
        }
    }

    if (strchr("~&|^<>+-*/%=!.", first_char))
    {
        return { OPERATOR, lexer.source.substr(start, 1) };
    }

    return { UNKNOWN, lexer.source.substr(start, 1) };
}

token_t handlePunctual(lexer_t& lexer)
{
    const std::string_view::size_type start = lexer.position;
    const char punct = lexer.current_char;
    advance(lexer);

    if (punct == ':' && lexer.current_char == ':')
    {
        advance(lexer);
        return { PUNCTUAL, lexer.source.substr(start, 2) };
    }

    return { PUNCTUAL, lexer.source.substr(start, 1) };
}

token_t handleString(lexer_t& lexer)
{
    const std::string_view::size_type start = lexer.position;
    const char quoteType = lexer.current_char;

    advance(lexer);
    while (lexer.current_char != '\0' && lexer.current_char != quoteType)
    {
        if (lexer.current_char == '\\')
            advance(lexer);
        advance(lexer);
    }

    if (lexer.current_char == quoteType)
    {
        advance(lexer);
        return { STRING, lexer.source.substr(start, lexer.position - start) };
    }
    advance(lexer);
    reportError("Unclosed string literal found at line " + std::to_string(lexer.line) + ", column " + std::to_string(lexer.column));
    return { UNKNOWN, lexer.source.substr(start, lexer.position - start) };
}

token_t handleType(lexer_t& lexer)
{
    const std::string_view::size_type start = lexer.position;

    auto isArray = false;
    if (lexer.current_char == '[')
    {
        isArray = true;
        advance(lexer);
        while (lexer.current_char != '\0' && lexer.current_char != ']')
            advance(lexer);
        if (lexer.current_char == ']')
            advance(lexer);
    }

    while (isAlnum(lexer.current_char) || lexer.current_char == '_')
        advance(lexer);

    auto isNullable = false;
    if (lexer.current_char == '?')
    {
        isNullable = true;
        advance(lexer);
    }

    std::string_view typeName = lexer.source.substr(start, lexer.position - start);
    const auto type_it = std::find_if(type_t.begin(), type_t.end(), [&](const auto& pair)
    {
        return pair.first == typeName;
    });

    if (type_it != type_t.end())
    {
        return { TYPE, typeName };
    }

    if (isArray)
        return { TYPE, typeName };

    if (isNullable)
        return { NULLABLE_TYPE, typeName };

    lexer.position = start;
    lexer.current_char = lexer.source[start];
    return { UNKNOWN, {} };
}

token_t handleUnknown(lexer_t& lexer)
{
    std::cerr << "Unknown character '" << lexer.current_char << "' at line " << lexer.line << ", column " << lexer.column << "\n";
    const char unknown = lexer.current_char;
    advance(lexer);

    while (lexer.current_char != '\0' && lexer.current_char != '\n' && !std::strchr("(){};,:", lexer.current_char))
        advance(lexer);

    return { UNKNOWN, std::string_view(&unknown, 1) };
}

token_t nextToken(lexer_t& lexer)
{
    while (true)
    {
        skipWhitespaceAndComment(lexer);
        if (lexer.current_char == '\0')
            return { END_OF_FILE, {} };

        switch (lexer.state)
        {
            case START:
                if (isAlpha(lexer.current_char) || lexer.current_char == '_')
                {
                    lexer.state = IDENTIFIER_STATE;
                }
                else if (isDigit(lexer.current_char) || (lexer.current_char == '.' && isDigit(peekNext(lexer))))
                {
                    lexer.state = LITERAL_STATE;
                }
                else if (lexer.current_char == '[')
                {
                    lexer.state = TYPE_STATE;
                }
                else if (lexer.current_char == '"' || lexer.current_char == '\'')
                {
                    lexer.state = STRING_STATE;
                }
                else if (std::strchr("=!<>+-*/%&|", lexer.current_char))
                {
                    lexer.state = OPERATOR_STATE;
                }
                else if (std::strchr("(){};,:", lexer.current_char))
                {
                    lexer.state = PUNCTUAL_STATE;
                }
                else
                {
                    lexer.state = UNKNOWN_STATE;
                }
                break;
            case IDENTIFIER_STATE:
                return handleIdentifier(lexer);

            case TYPE_STATE:
                return handleType(lexer);

            case LITERAL_STATE:
                return handleLiteral(lexer);

            case STRING_STATE:
                return handleString(lexer);

            case OPERATOR_STATE:
                return handleOperator(lexer);

            case PUNCTUAL_STATE:
                return handlePunctual(lexer);

            case UNKNOWN_STATE:
                return handleUnknown(lexer);

            default:
                std::cerr << "Unexpected state at line " << lexer.line
                          << ", column " << lexer.column << std::endl;
            return handleUnknown(lexer);
        }
    }
}


std::vector<token_t> tokenize(lexer_t& lexer)
{
    std::vector<token_t> tokens;
    tokens.reserve(lexer.source.length() / 2);

    token_t token;
    do
    {
        lexer.state = START;
        token = nextToken(lexer);
        if (token.type != UNKNOWN)
            tokens.emplace_back(token);
    }
    while (token.type != END_OF_FILE);

    return tokens;
}
