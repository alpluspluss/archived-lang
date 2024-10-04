//
// Created by alpluspluss on 10/03/2024 AD.
//
// TODO: Implement type solver
// TODO: Implement ast builder
// TODO: Implement parse_package
// TODO: Implement parse_import
// TODO: Implement parse_annotation
// TODO: Implement parse_enum
// TODO: Implement parse_class

#include <iostream>
#include "lang.h"

void parser::parser_init(parser_t& parser, const std::vector<lexer::token_t>& tokens)
{
    parser.tokens = std::make_unique<std::vector<lexer::token_t>>(tokens);
    parser.token_index = 0;
}

lexer::token_t parser::peek(const parser_t& parser)
{
    if (parser.token_index < parser.tokens->size())
    {
        return (*parser.tokens)[parser.token_index];
    }
    return lexer::token_t{ lexer::token_type::END_OF_FILE, "" };
}

lexer::token_t parser::next(parser_t& parser)
{
    const lexer::token_t token = peek(parser);
    consume(parser);
    return token;
}

inline void parser::consume(parser_t& parser)
{
    parser.token_index += parser.token_index < parser.tokens->size();
}

inline bool parser::expect_type(parser_t& parser, const lexer::token_type type)
{
    const bool match = peek(parser).type == type;
    parser.token_index += match;
    return match;
}

inline bool parser::expect_value(parser_t& parser, const std::string_view& value)
{
    const bool match = peek(parser).value == value;
    parser.token_index += match;
    return match;
}

void parser::log_error(parser_t& parser, const std::string& message)
{
    parser.error_log.push_back(message);
    std::cerr << "Error: " << message << std::endl;
}

bool parser::parse_program(parser_t& parser)
{
    while (parser.token_index < parser.tokens->size())
    {
        if (const auto&[type, value] = peek(parser); value == "function")
        {
            if (!parse_function(parser))
            {
                log_error(parser, "Failed to parse function.");
                return false;
            }
        }
        else if (value == "var")
        {
            if (!parse_variable(parser))
            {
                log_error(parser, "Failed to parse variable.");
                return false;
            }
        }
        else if (type == lexer::token_type::END_OF_FILE)
        {
            break;
        }
        else
        {
            log_error(parser, "Unexpected token: " + std::string(value));
            return false;
        }
    }

    return true;
}

bool parser::parse_function(parser_t& parser)
{
    consume(parser);
    expect_type(parser, lexer::token_type::IDENTIFIER); // Optional function name check for anonymous functions

    if (!expect_value(parser, "(") || !expect_value(parser, ")"))
    {
        log_error(parser, "Expected '()' after function name.");
        return false;
    }

    if (!expect_value(parser, "->"))
    {
        log_error(parser, "Expected '->' after '()'.");
        return false;
    }

    auto type_found = false;
    for (const auto& type : {lexer::token_type::IDENTIFIER, lexer::token_type::TYPE, lexer::token_type::NULLABLE_TYPE})
    {
        if (expect_type(parser, type))
        {
            type_found = true;
            break;
        }
    }
    if (!type_found)
    {
        log_error(parser, "Expected return type after '->'.");
        return false;
    }

    if (!expect_value(parser, "{"))
    {
        log_error(parser, "Expected '{' to start function body.");
        return false;
    }

    while (!expect_value(parser, "}"))
    {
        consume(parser);
    }

    return true;
}

bool parser::parse_variable(parser_t& parser)
{
    consume(parser);
    if (!expect_type(parser, lexer::token_type::IDENTIFIER))
    {
        log_error(parser, "Expected variable name after 'var'.");
        return false;
    }

    if (!expect_value(parser, ":") || !expect_type(parser, lexer::token_type::TYPE))
    {
        log_error(parser, "Expected type after variable name.");
        return false;
    }

    if (expect_value(parser, "="))
    {
        if (!expect_type(parser, lexer::token_type::LITERAL) &&
            !expect_type(parser, lexer::token_type::IDENTIFIER))
        {
            log_error(parser, "Expected value after '='.");
            return false;
        }
    }

    if (!expect_value(parser, ";"))
    {
        log_error(parser, "Expected ';' at the end of variable declaration.");
        return false;
    }

    return true;
}
