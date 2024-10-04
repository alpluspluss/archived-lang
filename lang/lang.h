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
        PATH,
        OPERATOR,
        PUNCTUAL,
        STRING,
        KEYWORD,
        ANNOTATION,
        TYPE,
        NULLABLE_TYPE,
        ARRAY_TYPE,
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
        ANNOTATION_STATE,
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
    void skip_whitespace_comment(lexer_t& lexer);
    token_t handle_annotation(lexer_t& lexer);
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

namespace parser
{
    struct parser_t
    {
        size_t token_index;  // Current index in the token list
        std::unique_ptr<std::vector<lexer::token_t>> tokens;  // List of tokens from the lexer
        std::vector<std::string> error_log;  // Collects parsing errors
    };

    void parser_init(parser_t& parser, const std::vector<lexer::token_t>& tokens);

    inline lexer::token_t peek(const parser_t& parser);
    inline lexer::token_t next(parser_t& parser);
    inline void consume(parser_t& parser);

    inline bool expect_type(parser_t& parser, lexer::token_type type);
    inline bool expect_value(parser_t& parser, const std::string_view& value);

    bool parse_program(parser_t& parser);
    bool parse_function(parser_t& parser);
    bool parse_expression(parser_t& parser);
    bool parse_variable(parser_t& parser);

    void log_error(parser_t& parser, const std::string& message);
}

#endif
