//
// Created by alpluspluss on 9/22/2024 AD.
//

#ifndef LANG_H
#define LANG_H

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
        uint32_t line;
        uint32_t column;
        char current_char;
        state_i state;
        std::vector<token_t> tokens;
        std::vector<std::string> error_log;
    };

    void lexer_init(lexer_t& lexer, std::string_view source);
    char peek_next(const lexer_t& lexer);
    void skip_whitespace_comment(lexer_t& lexer);
    token_t handle_identifier(lexer_t& lexer);
    token_t handle_literal(lexer_t& lexer);
    token_t handle_operator(lexer_t& lexer);
    token_t handle_punctual(lexer_t& lexer);
    token_t handle_string(lexer_t& lexer);
    token_t handle_type(lexer_t& lexer);
    token_t handle_unknown(lexer_t& lexer);
    token_t next_token(lexer_t& lexer);

    [[nodiscard]] std::vector<token_t> tokenize(lexer_t& lexer);
    void flush_errors(const lexer_t& lexer);
}

#endif //LANG_H
