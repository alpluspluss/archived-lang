//
// Created by alpluspluss on 9/22/2024 AD.
//

#include <algorithm>
#include <array>
#include <bitset>
#include <cstring>
#include <iostream>
#include <unordered_set>
#include "lang.h"

#define WHITESPACE_BITMASK_LOW (1ULL << ' ' | 1ULL << '\t' | 1ULL << '\n' | 1ULL << '\r' | 1ULL << '\v' | 1ULL << '\f')
#define IS_WHITESPACE(c) (((unsigned char)(c) < 128) && (WHITESPACE_BITMASK_LOW & (1ULL << (c))))

static const std::bitset<256> isAlphaTable = []
{
    std::bitset<256> table;
    for (auto i = 0; i < 256; ++i)
    {
        if ((i >= 'a' && i <= 'z') || (i >= 'A' && i <= 'Z'))
            table.set(i);
    }
    return table;
}();
static const std::bitset<256> isDigitTable = []
{
    std::bitset<256> table;
    for (int i = '0'; i <= '9'; ++i)
        table.set(i);
    return table;
}();
static const std::bitset<256> isXdigitTable = []
{
    std::bitset<256> table = isDigitTable;
    for (int i = 'a'; i <= 'f'; ++i)
    {
        table.set(i);
        table.set(i - 32);
    }
    return table;
}();
static const std::bitset<256> is_alnum_table = isAlphaTable | isDigitTable;

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

static constexpr std::array<std::pair<std::string_view, lexer::token_type>, 34> keyword_t = {{
    // Type
    { "true", lexer::token_type::KEYWORD },
    { "false", lexer::token_type::KEYWORD },
    { "null", lexer::token_type::KEYWORD },
    // Imports
    { "package", lexer::token_type::KEYWORD },
    { "using", lexer::token_type::KEYWORD },
    { "import", lexer::token_type::KEYWORD },
    // Declaration
    { "var", lexer::token_type::KEYWORD },
    { "const", lexer::token_type::KEYWORD },
    { "function", lexer::token_type::KEYWORD },
    { "static", lexer::token_type::KEYWORD },
    { "inline", lexer::token_type::KEYWORD },
    { "return", lexer::token_type::KEYWORD },
    { "new", lexer::token_type::KEYWORD },
    // Control flow
    { "if", lexer::token_type::KEYWORD },
    { "else", lexer::token_type::KEYWORD },
    { "for", lexer::token_type::KEYWORD },
    { "while", lexer::token_type::KEYWORD },
    { "break", lexer::token_type::KEYWORD },
    { "continue", lexer::token_type::KEYWORD },
    { "switch", lexer::token_type::KEYWORD },
    { "case", lexer::token_type::KEYWORD },
    { "default", lexer::token_type::KEYWORD },
    // Object-Oriented
    { "class", lexer::token_type::KEYWORD },
    { "virtual", lexer::token_type::KEYWORD },
    { "extends", lexer::token_type::KEYWORD },
    { "final", lexer::token_type::KEYWORD },
    { "public", lexer::token_type::KEYWORD },
    { "private", lexer::token_type::KEYWORD },
    // Asynchhronous
    { "async", lexer::token_type::KEYWORD },
    { "await", lexer::token_type::KEYWORD },
    // Exception handling
    { "try", lexer::token_type::KEYWORD },
    { "catch", lexer::token_type::KEYWORD },
    { "finally", lexer::token_type::KEYWORD },
    { "throw", lexer::token_type::KEYWORD }
}};

static constexpr std::array<std::pair<std::string_view, lexer::token_type>, 17> type_t = {{
    { "u8", lexer::token_type::TYPE },
    { "i8", lexer::token_type::TYPE },
    { "u16", lexer::token_type::TYPE },
    { "i16", lexer::token_type::TYPE },
    { "u16", lexer::token_type::TYPE },
    { "i32", lexer::token_type::TYPE },
    { "i64", lexer::token_type::TYPE },
    { "u32", lexer::token_type::TYPE },
    { "u64", lexer::token_type::TYPE },
    { "f32", lexer::token_type::TYPE },
    { "f64", lexer::token_type::TYPE },
    { "string", lexer::token_type::TYPE },
    { "boolean", lexer::token_type::TYPE },
    { "void", lexer::token_type::TYPE },
    { "auto", lexer::token_type::TYPE },
    { "Unique", lexer::token_type::TYPE },
    { "Shared", lexer::token_type::TYPE }
}};

static constexpr std::array<std::pair<std::string_view, lexer::token_type>, 3> annotation_t = {{
    { "@packed", lexer::token_type::ANNOTATION },
    { "@aligned", lexer::token_type::ANNOTATION },
    { "@deprecated", lexer::token_type::ANNOTATION },
}};

static const std::unordered_set<std::string_view> twoCharOp_t = {
    "->", "==", "!=", "<=", ">=", "&&", "||", "<<", ">>", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>="
};

void reportError(lexer::lexer_t& lexer, const std::string& msg)
{
    lexer.error_log.push_back(msg);
}

inline void ADVANCE(lexer::lexer_t& lexer)
{
    if (lexer.current_char != '\0')
    {
        ++lexer.position;
        lexer.current_char = lexer.position < lexer.source.size() ? lexer.source[lexer.position] : '\0';

        lexer.line += lexer.current_char == '\n';
        lexer.column = lexer.current_char == '\n' ? 1 : lexer.column + 1;
    }
}

inline char PEEK_NEXT(const lexer::lexer_t& lexer)
{
    return lexer.position + 1 < lexer.source.size() ? lexer.source[lexer.position + 1] : '\0';
}

void lexer::lexer_init(lexer_t& lexer, const std::string_view source)
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

void lexer::skip_whitespace_comment(lexer_t& lexer)
{
    while (IS_WHITESPACE(lexer.current_char) || lexer.current_char == '/')
    {
        if (lexer.current_char == '/')
        {
            const char next_char = PEEK_NEXT(lexer);
            const bool is_single_line_comment = next_char == '/';
            const bool is_multi_line_comment = next_char == '*';
            if (is_single_line_comment)
            {
                while (lexer.current_char != '\n' && lexer.current_char != '\0')
                {
                    ADVANCE(lexer);
                }
            }
            else if (is_multi_line_comment)
            {
                ADVANCE(lexer);
                ADVANCE(lexer);
                while (lexer.current_char != '\0' && !(lexer.current_char == '*' && PEEK_NEXT(lexer) == '/'))
                {
                    ADVANCE(lexer);
                }
                if (lexer.current_char != '\0')
                {
                    ADVANCE(lexer);
                    ADVANCE(lexer);
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            ADVANCE(lexer);
        }
    }
}

lexer::token_t lexer::handle_annotation(lexer_t& lexer)
{
    const auto start = lexer.position;
    ADVANCE(lexer);

    while (isAlnum(lexer.current_char) || lexer.current_char == '_')
        ADVANCE(lexer);

    std::string_view annotation_name = lexer.source.substr(start, lexer.position - start);
    if (const auto annotation_it = std::ranges::find_if(annotation_t, [&](const auto& a) { return a.first == annotation_name; }); annotation_it != annotation_t.end())
    {
        return { annotation_it->second, annotation_name };
    }
    reportError(lexer, "Unknown annotation '" + std::string(annotation_name) + "' at line " + std::to_string(lexer.line));
    return { token_type::UNKNOWN, annotation_name };
}

lexer::token_t lexer::handle_identifier(lexer_t& lexer)
{
    const auto start = lexer.position;
    while (isAlnum(lexer.current_char) || lexer.current_char == '_')
    {
        ADVANCE(lexer);
    }

    if (lexer.current_char == '.' && PEEK_NEXT(lexer) == '.' && lexer.source[lexer.position + 2] == '.')
    {
        ADVANCE(lexer);
        ADVANCE(lexer);
        ADVANCE(lexer);
        return { token_type::IDENTIFIER, lexer.source.substr(start, lexer.position - start) };
    }

    while (lexer.current_char == '.' && isAlpha(PEEK_NEXT(lexer)))
    {
        ADVANCE(lexer);
        while (isAlnum(lexer.current_char) || lexer.current_char == '_')
        {
            ADVANCE(lexer);
        }
    }

    std::string_view identifier = lexer.source.substr(start, lexer.position - start);

    if (const auto keyword_it = std::ranges::find_if(keyword_t, [&](const auto& t) { return t.first == identifier; }); keyword_it != keyword_t.end())
    {
        return { keyword_it->second, identifier };
    }

    if (const auto type_it = std::ranges::find_if(type_t, [&](const auto& t) { return t.first == identifier; }); type_it != type_t.end())
    {
        if (lexer.current_char == '?')
        {
            ADVANCE(lexer);
            return { token_type::NULLABLE_TYPE, lexer.source.substr(start, lexer.position - start) };
        }
        return { type_it->second, identifier };
    }

    return { token_type::IDENTIFIER, identifier };
}

lexer::token_t lexer::handle_literal(lexer_t& lexer)
{
    const auto start = lexer.position;

    const bool isHex = lexer.current_char == '0' && (PEEK_NEXT(lexer) == 'x' || PEEK_NEXT(lexer) == 'X');
    if (isHex)
    {
        ADVANCE(lexer);
        ADVANCE(lexer);
    }

    while ((isHex && isXdigit(lexer.current_char)) || (!isHex && isDigit(lexer.current_char)))
        ADVANCE(lexer);

    if (!isHex && lexer.current_char == '.')
    {
        ADVANCE(lexer);
        if (!isDigit(lexer.current_char))
        {
            reportError(lexer, "Invalid floating point number at line " + std::to_string(lexer.line) + ", column " + std::to_string(lexer.column));
        }

        while (isDigit(lexer.current_char))
            ADVANCE(lexer);
    }

    if (!isHex && (lexer.current_char == 'e' || lexer.current_char == 'E'))
    {
        ADVANCE(lexer);
        if (lexer.current_char == '+' || lexer.current_char == '-')
            ADVANCE(lexer);
        if (!isDigit(lexer.current_char))
        {
            reportError(lexer, "Invalid exponent in floating point number at line " + std::to_string(lexer.line) + ", column " + std::to_string(lexer.column));
        }
        while (isDigit(lexer.current_char))
            ADVANCE(lexer);
    }

    return { token_type::LITERAL, lexer.source.substr(start, lexer.position - start) };
}

lexer::token_t lexer::handle_operator(lexer_t& lexer)
{
    const auto start = lexer.position;
    if (lexer.current_char != '\0' && PEEK_NEXT(lexer) != '\0')
    {
        if (const std::string_view two_char_op(lexer.source.data() + start, 2); twoCharOp_t.contains(two_char_op))
        {
            ADVANCE(lexer);
            ADVANCE(lexer);
            return { token_type::OPERATOR, two_char_op };
        }
    }

    const char first_char = lexer.current_char;
    ADVANCE(lexer);

    if (strchr("~&|^<>+-*/%=!.", first_char))
    {
        return { token_type::OPERATOR, lexer.source.substr(start, 1) };
    }

    return { token_type::UNKNOWN, lexer.source.substr(start, 1) };
}

lexer::token_t lexer::handle_punctual(lexer_t& lexer)
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

lexer::token_t lexer::handle_string(lexer_t& lexer)
{
    const auto start = lexer.position;
    const char quoteType = lexer.current_char;
    ADVANCE(lexer);

    auto isEscaped = false;

    while (lexer.current_char != '\0' && (lexer.current_char != quoteType || isEscaped))
    {
        if (lexer.current_char == '\\')
            isEscaped = !isEscaped;
        else
            isEscaped = false;
        ADVANCE(lexer);
    }

    if (lexer.current_char == quoteType)
    {
        ADVANCE(lexer);
        return { token_type::STRING, lexer.source.substr(start, lexer.position - start) };
    }

    reportError(lexer, "Unclosed string literal at line " + std::to_string(lexer.line) + ", column " + std::to_string(lexer.column));
    return { token_type::UNKNOWN, lexer.source.substr(start, lexer.position - start) };
}

lexer::token_t lexer::handle_type(lexer_t& lexer)
{
    const auto start = lexer.position;
    if (lexer.current_char == '[')
    {
        ADVANCE(lexer);
        skip_whitespace_comment(lexer);

        const auto type_start = lexer.position;
        while (isAlnum(lexer.current_char) || lexer.current_char == '_')
            ADVANCE(lexer);

        std::string_view type_name = lexer.source.substr(type_start, lexer.position - type_start);
        if (std::ranges::find_if(type_t, [&](const auto& t) { return t.first == type_name; }) == type_t.end())
        {
            reportError(lexer, "Invalid type name inside array type.");
            return { token_type::UNKNOWN, lexer.source.substr(start, lexer.position - start) };
        }

        skip_whitespace_comment(lexer);
        if (lexer.current_char == ']')
        {
            ADVANCE(lexer);

            if (const char next_char = PEEK_NEXT(lexer); lexer.current_char == '?' && next_char != ']')
            {
                ADVANCE(lexer);
                return { token_type::NULLABLE_TYPE, lexer.source.substr(start, lexer.position - start) };
            }

            return { token_type::TYPE, lexer.source.substr(start, lexer.position - start) };
        }

        reportError(lexer, "Expected ']' for array type.");
        return { token_type::UNKNOWN, lexer.source.substr(start, lexer.position - start) };
    }

    reportError(lexer, "Expected '[' to begin array type.");
    return { token_type::UNKNOWN, lexer.source.substr(start, lexer.position - start) };
}

lexer::token_t lexer::handle_unknown(lexer_t& lexer)
{
    const std::string error_msg = "Unknown character '" + std::string(1, lexer.current_char) + "' at line " + std::to_string(lexer.line) + ", column " + std::to_string(lexer.column);
    reportError(lexer, error_msg);
    ADVANCE(lexer);
    return { token_type::UNKNOWN, {} };
}

lexer::token_t lexer::next_token(lexer_t& lexer)
{
    while (true)
    {
        skip_whitespace_comment(lexer);
        if (lexer.current_char == '\0')
            return { token_type::END_OF_FILE, {} };

        switch (lexer.state)
        {
            case state_i::START:
                if (lexer.current_char == '@')
                {
                    lexer.state = state_i::ANNOTATION_STATE;
                }
                else if (lexer.current_char == '[')
                {
                    lexer.state = state_i::TYPE_STATE;
                }
                else if (isAlpha(lexer.current_char) || lexer.current_char == '_')
                {
                    lexer.state = state_i::IDENTIFIER_STATE;
                }
                else if (isDigit(lexer.current_char) || (lexer.current_char == '.' && isDigit(PEEK_NEXT(lexer))))
                {
                    lexer.state = state_i::LITERAL_STATE;
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

            case state_i::ANNOTATION_STATE:
                return handle_annotation(lexer);
            case state_i::IDENTIFIER_STATE:
                return handle_identifier(lexer);
            case state_i::TYPE_STATE:
                return handle_type(lexer);
            case state_i::LITERAL_STATE:
                return handle_literal(lexer);
            case state_i::STRING_STATE:
                return handle_string(lexer);
            case state_i::OPERATOR_STATE:
                return handle_operator(lexer);
            case state_i::PUNCTUAL_STATE:
                return handle_punctual(lexer);
            case state_i::UNKNOWN_STATE:
                return handle_unknown(lexer);
            default:
                lexer.state = state_i::START;
                ADVANCE(lexer);
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
        token = next_token(lexer);
        if (token.type != token_type::UNKNOWN)
            tokens.emplace_back(token);
    }
    while (token.type != token_type::END_OF_FILE);

    return tokens;
}

void lexer::flush_errors(const lexer_t& lexer)
{
    for (const auto& error : lexer.error_log)
        std::cerr << error << std::endl;
}
