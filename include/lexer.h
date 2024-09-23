//
// Created by alpluspluss on 9/22/2024 AD.
//

#ifndef LEXER_H
#define LEXER_H

#include <string_view>
#include <vector>

namespace lexer
{
    enum class token_type : uint8_t
    {
        IDENTIFIER,
        LITERAL,
        OPERATOR,
        PUNCTUAL,
        STRING,
        KEYWORD,
        TYPE,
        NULLABLE_TYPE,
        UNKNOWN,
        END_OF_FILE,
    };

    struct token_t
    {
        token_type type;
        std::string_view value;
    };

    enum class state_i : uint8_t
    {
        START,
        IDENTIFIER_STATE,
        LITERAL_STATE,
        PUNCTUAL_STATE,
        KEYWORD_STATE,
        TYPE_STATE,
        STRING_STATE,
        OPERATOR_STATE,
        UNKNOWN_STATE,
        END_STATE
    };

    struct lexer_t
    {
        std::string_view source;
        size_t position;
        unsigned int line;
        unsigned int column;
        char current_char;
        state_i state;
        std::vector<token_t> tokens;
        std::vector<std::string> error_log;
    };

    void lexerInit(lexer_t& lexer, std::string_view source);
    char peekNext(const lexer_t& lexer);
    void skipWhitespaceAndComment(lexer_t& lexer);
    token_t handleIdentifier(lexer_t& lexer);
    token_t handleLiteral(lexer_t& lexer);
    token_t handleOperator(lexer_t& lexer);
    token_t handlePunctual(lexer_t& lexer);
    token_t handleString(lexer_t& lexer);
    token_t handleType(lexer_t& lexer);
    token_t handleUnknown(lexer_t& lexer);
    token_t nextToken(lexer_t& lexer);

    [[nodiscard]] std::vector<token_t> tokenize(lexer_t& lexer);
    void flushErrors(const lexer_t& lexer);
}

#endif
