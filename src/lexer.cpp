//
// Created by Schnond Chansuk on 29/8/2024 AD.
//

#include "../include/lexer.h"
#include <algorithm>
#include <bitset>
#include <cstring>
#include <iostream>
#include <unordered_set>

static constexpr std::bitset<256> isAlphaTable = []
{
    std::bitset<256> table;
    for (auto i = 0; i < 256; ++i)
    {
        if ((i >= 'a' && i <= 'z') || (i >= 'A' && i <= 'Z'))
            table.set(i);
    }
    return table;
}();
static constexpr std::bitset<256> isDigitTable = []
{
    std::bitset<256> table;
    for (int i = '0'; i <= '9'; ++i)
        table.set(i);
    return table;
}();
static constexpr std::bitset<256> isXdigitTable = []
{
    std::bitset<256> table = isDigitTable;
    for (int i = 'a'; i <= 'f'; ++i)
    {
        table.set(i);
        table.set(i - 32);
    }
    return table;
}();
static constexpr std::bitset<256> is_alnum_table = isAlphaTable | isDigitTable;

constexpr bool isAlpha(const char c)
{
    return isAlphaTable[static_cast<unsigned char>(c)];
}

constexpr bool isDigit(const char c)
{
    return isDigitTable[static_cast<unsigned char>(c)];
}

constexpr bool isXdigit(const char c)
{
    return isXdigitTable[static_cast<unsigned char>(c)];
}

constexpr bool isAlnum(const char c)
{
    return is_alnum_table[static_cast<unsigned char>(c)];
}

#define ADVANCE(lexer)                          \
    do {                                        \
        if ((lexer).position < (lexer).source.size()) {  \
            (lexer).current_char = (lexer).source[++(lexer).position]; \
            ++(lexer).column;                   \
            if ((lexer).current_char == '\n') { \
                ++(lexer).line;                 \
                (lexer).column = 1;             \
            }                                   \
        } else {                                \
            (lexer).current_char = '\0';        \
        }                                       \
    } while(0)

#define PEEK_NEXT(lexer) \
    ((lexer).position + 1 < (lexer).source.size() ? (lexer).source[(lexer).position + 1] : '\0')

static constexpr std::array<std::pair<std::string_view, lexer::token_type>, 25> keyword_t = {{
    { "true", lexer::token_type::KEYWORD }, { "false", lexer::token_type::KEYWORD }, { "null", lexer::token_type::KEYWORD },
    { "package", lexer::token_type::KEYWORD }, { "import", lexer::token_type::KEYWORD }, { "from", lexer::token_type::KEYWORD },
    { "var", lexer::token_type::KEYWORD }, { "const", lexer::token_type::KEYWORD }, { "function", lexer::token_type::KEYWORD },
    { "return", lexer::token_type::KEYWORD }, { "if", lexer::token_type::KEYWORD }, { "else", lexer::token_type::KEYWORD },
    { "for", lexer::token_type::KEYWORD }, { "while", lexer::token_type::KEYWORD }, { "class", lexer::token_type::KEYWORD },
    { "static", lexer::token_type::KEYWORD }, { "virtual", lexer::token_type::KEYWORD }, { "inherits", lexer::token_type::KEYWORD },
    { "final", lexer::token_type::KEYWORD }
}};

static constexpr std::array<std::pair<std::string_view, lexer::token_type>, 10> type_t = {{
    { "i32", lexer::token_type::TYPE }, { "i64", lexer::token_type::TYPE }, { "u32", lexer::token_type::TYPE }, { "u64", lexer::token_type::TYPE },
    { "f32", lexer::token_type::TYPE }, { "f64", lexer::token_type::TYPE }, { "string", lexer::token_type::TYPE }, { "boolean", lexer::token_type::TYPE },
    { "void", lexer::token_type::TYPE }, { "auto", lexer::token_type::TYPE }
}};

static const std::unordered_set<std::string_view> twoCharOp_t = {
    "->", "==", "!=", "<=", ">=", "&&", "||", "<<", ">>", "++", "--", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>="
};

void reportError(lexer::lexer_t& lexer, const std::string& msg)
{
    lexer.error_log.push_back(msg);
}

void lexer::lexerInit(lexer_t& lexer, const std::string_view source)
{
    lexer.source = source;
    lexer.position = 0;
    lexer.state = state_i::START;
    lexer.current_char = source.empty() ? '\0' : source[0];
    lexer.line = 1;
    lexer.column = 1;
    lexer.tokens.clear();
    lexer.error_log.clear();
}

void lexer::skipWhitespaceAndComment(lexer_t& lexer)
{
    while (lexer.current_char != '\0' && (std::isspace(lexer.current_char) || lexer.current_char == '\n'))
    {
        if (lexer.current_char == '\n')
        {
            ++lexer.line;
            lexer.column = 1;
        }
        else
        {
            ++lexer.column;
        }

        ADVANCE(lexer);
    }

    if (lexer.current_char == '/' && PEEK_NEXT(lexer) == '/')
    {
        while (lexer.current_char != '\0' && lexer.current_char != '\n')
            ADVANCE(lexer);

        if (lexer.current_char == '\n')
        {
            lexer.line++;
            lexer.column = 1;
        }

        ADVANCE(lexer);
        skipWhitespaceAndComment(lexer);
    }

    else if (lexer.current_char == '/' && PEEK_NEXT(lexer) == '*')
    {
        ADVANCE(lexer);
        ADVANCE(lexer);

        while (lexer.current_char != '\0' && !(lexer.current_char == '*' && PEEK_NEXT(lexer) == '/'))
        {
            if (lexer.current_char == '\n')
            {
                lexer.line++;
                lexer.column = 1;
            }
            else
            {
                lexer.column++;
            }
            ADVANCE(lexer);
        }

        if (lexer.current_char == '*')
        {
            ADVANCE(lexer);
            if (lexer.current_char == '/')
                ADVANCE(lexer);
        }

        skipWhitespaceAndComment(lexer);
    }
}

lexer::token_t lexer::handleIdentifier(lexer_t& lexer)
{
    const auto start = lexer.position;
    while (isAlnum(lexer.current_char) || lexer.current_char == '_')
    {
        ADVANCE(lexer);
    }
    std::string_view identifier = lexer.source.substr(start, lexer.position - start);

    if (const auto type_it = std::ranges::find_if(type_t, [&](const auto& t) { return t.first == identifier; }); type_it != type_t.end())
    {
        if (lexer.current_char == '?')
        {
            ADVANCE(lexer);
            return { token_type::NULLABLE_TYPE, lexer.source.substr(start, lexer.position - start) };
        }
        return { type_it->second, identifier };
    }

    if (const auto keyword_it = std::ranges::find_if(keyword_t, [&](const auto& t) { return t.first == identifier; }); keyword_it != keyword_t.end())
        return { keyword_it->second, identifier };
    return { token_type::IDENTIFIER, identifier };
}

lexer::token_t lexer::handleLiteral(lexer_t& lexer)
{
    const auto start = lexer.position;
    if (lexer.current_char == '-' || lexer.current_char == '+')
        ADVANCE(lexer);

    if (lexer.current_char == '0' && (PEEK_NEXT(lexer) == 'x' || PEEK_NEXT(lexer) == 'X'))
    {
        ADVANCE(lexer);
        ADVANCE(lexer);
        while (isXdigit(lexer.current_char))
            ADVANCE(lexer);
    }
    else if (lexer.current_char == '0' && isDigit(PEEK_NEXT(lexer)))
    {
        ADVANCE(lexer);
        while (lexer.current_char >= '0' && lexer.current_char <= '7')
            ADVANCE(lexer);
    }
    else
    {
        while (isDigit(lexer.current_char))
            ADVANCE(lexer);
        if (lexer.current_char == '.')
        {
            ADVANCE(lexer);
            while (isDigit(lexer.current_char))
                ADVANCE(lexer);
        }
        if (lexer.current_char == 'e' || lexer.current_char == 'E')
        {
            ADVANCE(lexer);
            if (lexer.current_char == '+' || lexer.current_char == '-')
                ADVANCE(lexer);
            while (isDigit(lexer.current_char))
                ADVANCE(lexer);
        }
    }
    return { token_type::LITERAL, lexer.source.substr(start, lexer.position - start) };
}

lexer::token_t lexer::handleOperator(lexer_t& lexer)
{
    if (lexer.current_char == '-' && isDigit(PEEK_NEXT(lexer)))
        return handleLiteral(lexer);

    const auto start = lexer.position;
    const char first_char = lexer.current_char;
    ADVANCE(lexer);

    if (lexer.current_char != '\0')
    {
        if (const std::string_view two_char_op(lexer.source.data() + start, 2); twoCharOp_t.contains(two_char_op))
        {
            ADVANCE(lexer);
            return { token_type::OPERATOR, two_char_op };
        }
    }

    if (strchr("~&|^<>+-*/%=!.", first_char))
        return { token_type::OPERATOR, lexer.source.substr(start, 1) };

    return { token_type::UNKNOWN, lexer.source.substr(start, 1) };
}

lexer::token_t lexer::handlePunctual(lexer_t& lexer)
{
    const auto start = lexer.position;
    const char punct = lexer.current_char;
    ADVANCE(lexer);

    if (punct == ':' && lexer.current_char == ':')
    {
        ADVANCE(lexer);
        return { token_type::PUNCTUAL, lexer.source.substr(start, 2) };
    }

    return { token_type::PUNCTUAL, lexer.source.substr(start, 1) };
}

lexer::token_t lexer::handleString(lexer_t& lexer)
{
    const auto start = lexer.position;
    const char quoteType = lexer.current_char;
    ADVANCE(lexer);

    while (lexer.current_char != '\0' && lexer.current_char != quoteType)
    {
        if (lexer.current_char == '\\') ADVANCE(lexer);
        ADVANCE(lexer);
    }

    if (lexer.current_char == quoteType)
    {
        ADVANCE(lexer);
        return { token_type::STRING, lexer.source.substr(start, lexer.position - start) };
    }
    ADVANCE(lexer);
    reportError(lexer, "Unclosed string literal at line " + std::to_string(lexer.line) + ", column " + std::to_string(lexer.column));
    return { token_type::UNKNOWN, lexer.source.substr(start, lexer.position - start) };
}

lexer::token_t lexer::handleType(lexer_t& lexer)
{
    const auto start = lexer.position;
    if (lexer.current_char == '[')
    {
        ADVANCE(lexer);
        while (lexer.current_char != '\0' && lexer.current_char != ']')
            ADVANCE(lexer);
        if (lexer.current_char == ']')
        {
            ADVANCE(lexer);
        }
    }

    while (isAlnum(lexer.current_char) || lexer.current_char == '_')
        ADVANCE(lexer);

    auto isNullable = false;
    if (lexer.current_char == '?')
    {
        isNullable = true;
        ADVANCE(lexer);
    }

    std::string_view typeName = lexer.source.substr(start, lexer.position - start);

    const auto type_it = std::ranges::find_if(type_t, [&](const auto& pair)
    {
        return pair.first == typeName;
    });

    if (type_it != type_t.end())
        return { isNullable ? token_type::NULLABLE_TYPE : token_type::TYPE, typeName };

    return { token_type::UNKNOWN, typeName };
}


lexer::token_t lexer::handleUnknown(lexer_t& lexer)
{
    const std::string error_msg = "Unknown character '" + std::string(1, lexer.current_char) + "' at line " + std::to_string(lexer.line) + ", column " + std::to_string(lexer.column);
    reportError(lexer, error_msg);
    ADVANCE(lexer);
    return { token_type::UNKNOWN, {} };
}

lexer::token_t lexer::nextToken(lexer_t& lexer)
{
    while (true)
    {
        skipWhitespaceAndComment(lexer);
        if (lexer.current_char == '\0')
            return { token_type::END_OF_FILE, {} };

        switch (lexer.state)
        {
            case state_i::START:
                if (isAlpha(lexer.current_char) || lexer.current_char == '_')
                {
                    lexer.state = state_i::IDENTIFIER_STATE;
                }
                else if (isDigit(lexer.current_char) || (lexer.current_char == '.' && isDigit(PEEK_NEXT(lexer))))
                {
                    lexer.state = state_i::LITERAL_STATE;
                }
                else if (lexer.current_char == '[')
                {
                    lexer.state = state_i::TYPE_STATE;
                }
                else if (lexer.current_char == '"' || lexer.current_char == '\'')
                {
                    lexer.state = state_i::STRING_STATE;
                }
                else if (strchr("=!<>+-*/%&|", lexer.current_char))
                {
                    lexer.state = state_i::OPERATOR_STATE;
                }
                else if (strchr("(){};,:", lexer.current_char))
                {
                    lexer.state = state_i::PUNCTUAL_STATE;
                }
                else
                {
                    lexer.state = state_i::UNKNOWN_STATE;
                }
            break;

            case state_i::IDENTIFIER_STATE:
                return handleIdentifier(lexer);
            case state_i::TYPE_STATE:
                return handleType(lexer);
            case state_i::LITERAL_STATE:
                return handleLiteral(lexer);
            case state_i::STRING_STATE:
                return handleString(lexer);
            case state_i::OPERATOR_STATE:
                return handleOperator(lexer);
            case state_i::PUNCTUAL_STATE:
                return handlePunctual(lexer);
            default:
                reportError(lexer, "Unexpected state at line " + std::to_string(lexer.line) + ", column " + std::to_string(lexer.column));
        }
    }
}

std::vector<lexer::token_t> lexer::tokenize(lexer_t& lexer)
{
    std::vector<token_t> tokens;
    tokens.reserve(lexer.source.length() / 2);

    token_t token;
    do
    {
        lexer.state = state_i::START;
        token = nextToken(lexer);
        if (token.type != token_type::UNKNOWN)
            tokens.emplace_back(token);
    }
    while (token.type != token_type::END_OF_FILE);

    return tokens;
}

void lexer::flushErrors(const lexer_t& lexer)
{
    for (const auto& error : lexer.error_log)
        std::cerr << error << std::endl;
}
