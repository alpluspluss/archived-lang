//
// Created by Schnond Chansuk on 29/8/2024 AD.
//

#ifndef LEXER_H
#define LEXER_H

#include <array>
#include <string_view>
#include <unordered_set>
#include <vector>

using token_type = enum
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

using token_t = struct
{
    token_type type;
    std::string_view value;
};

using state_i = enum
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

using lexer_t = struct
{
    std::string_view source;
    size_t position;
    state_i state;
    char current_char;
    unsigned int line;
    unsigned int column;
    std::vector<token_t> tokens;
};

void lexerInit(lexer_t& lexer, std::string_view source);
inline void advance(lexer_t& lexer);
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

#endif //LEXER_H