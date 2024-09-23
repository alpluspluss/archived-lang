//
// Created by alpluspluss on 9/22/2024 AD.
//

#include <iostream>
#include "../include/lexer.h"

void test_lexer(const std::string& source, const size_t expected_token_count, const std::vector<std::pair<lexer::token_type, std::string>>& expected_tokens) {
    lexer::lexer_t lexer;
    lexerInit(lexer, source);

    const auto tokens = tokenize(lexer);

    if (tokens.size() != expected_token_count)
    {
        std::cerr << "Error: Expected " << expected_token_count << " tokens, but got " << tokens.size() << std::endl;
        exit(1);
    }

    for (size_t i = 0; i < expected_tokens.size(); ++i)
    {
        if (tokens[i].type != expected_tokens[i].first || tokens[i].value != expected_tokens[i].second)
        {
            std::cerr << "Error: Expected token " << i << " to be '"
                      << expected_tokens[i].second << "' of type " << static_cast<int>(expected_tokens[i].first)
                      << ", but got '" << tokens[i].value << "' of type " << static_cast<int>(tokens[i].type) << std::endl;
            exit(1);
        }
    }
    std::cout << "Test passed for source: \"" << source << "\"\n";
}

int main()
{
    // Test 1: Simple variable declaration
    test_lexer("var x = 10;", 6, {
        {lexer::token_type::KEYWORD, "var"},
        {lexer::token_type::IDENTIFIER, "x"},
        {lexer::token_type::OPERATOR, "="},
        {lexer::token_type::LITERAL, "10"},
        {lexer::token_type::PUNCTUAL, ";"},
        {lexer::token_type::END_OF_FILE, ""}
    });

    // Test 2: Function declaration
    test_lexer("function foo() { return 42; }", 10, {
        {lexer::token_type::KEYWORD, "function"},
        {lexer::token_type::IDENTIFIER, "foo"},
        {lexer::token_type::PUNCTUAL, "("},
        {lexer::token_type::PUNCTUAL, ")"},
        {lexer::token_type::PUNCTUAL, "{"},
        {lexer::token_type::KEYWORD, "return"},
        {lexer::token_type::LITERAL, "42"},
        {lexer::token_type::PUNCTUAL, ";"},
        {lexer::token_type::PUNCTUAL, "}"},
        {lexer::token_type::END_OF_FILE, ""}
    });

    // Test 3: String literal with escape characters
    test_lexer("var s = \"Hello, World!\";", 6, {
        {lexer::token_type::KEYWORD, "var"},
        {lexer::token_type::IDENTIFIER, "s"},
        {lexer::token_type::OPERATOR, "="},
        {lexer::token_type::STRING, "\"Hello, World!\""},
        {lexer::token_type::PUNCTUAL, ";"},
        {lexer::token_type::END_OF_FILE, ""}
    });

    // Test 4: Single-line comment skipping
    test_lexer("var x = 10; // This is a comment", 6, {
        {lexer::token_type::KEYWORD, "var"},
        {lexer::token_type::IDENTIFIER, "x"},
        {lexer::token_type::OPERATOR, "="},
        {lexer::token_type::LITERAL, "10"},
        {lexer::token_type::PUNCTUAL, ";"},
        {lexer::token_type::END_OF_FILE, ""}
    });

    // Test 5: Multi-line comment skipping
    test_lexer("var x /* comment */ = 10;", 6, {
        {lexer::token_type::KEYWORD, "var"},
        {lexer::token_type::IDENTIFIER, "x"},
        {lexer::token_type::OPERATOR, "="},
        {lexer::token_type::LITERAL, "10"},
        {lexer::token_type::PUNCTUAL, ";"},
        {lexer::token_type::END_OF_FILE, ""}
    });

    // Test 6: Operators
    test_lexer("x += 42;", 5, {
        {lexer::token_type::IDENTIFIER, "x"},
        {lexer::token_type::OPERATOR, "+="},
        {lexer::token_type::LITERAL, "42"},
        {lexer::token_type::PUNCTUAL, ";"},
        {lexer::token_type::END_OF_FILE, ""}
    });

    // Test 7: Complex statement
    test_lexer("if (x <= 100 && x != 0) { y = x / 2; }", 19, {
        {lexer::token_type::KEYWORD, "if"},
        {lexer::token_type::PUNCTUAL, "("},
        {lexer::token_type::IDENTIFIER, "x"},
        {lexer::token_type::OPERATOR, "<="},
        {lexer::token_type::LITERAL, "100"},
        {lexer::token_type::OPERATOR, "&&"},
        {lexer::token_type::IDENTIFIER, "x"},
        {lexer::token_type::OPERATOR, "!="},
        {lexer::token_type::LITERAL, "0"},
        {lexer::token_type::PUNCTUAL, ")"},
        {lexer::token_type::PUNCTUAL, "{"},
        {lexer::token_type::IDENTIFIER, "y"},
        {lexer::token_type::OPERATOR, "="},
        {lexer::token_type::IDENTIFIER, "x"},
        {lexer::token_type::OPERATOR, "/"},
        {lexer::token_type::LITERAL, "2"},
        {lexer::token_type::PUNCTUAL, ";"},
        {lexer::token_type::PUNCTUAL, "}"},
        {lexer::token_type::END_OF_FILE, ""}
    });

    // Test 8: Nullables and types
    test_lexer("var a: i32? = null;", 8, {
        {lexer::token_type::KEYWORD, "var"},
        {lexer::token_type::IDENTIFIER, "a"},
        {lexer::token_type::PUNCTUAL, ":"},
        {lexer::token_type::NULLABLE_TYPE, "i32?"},
        {lexer::token_type::OPERATOR, "="},
        {lexer::token_type::KEYWORD, "null"},
        {lexer::token_type::PUNCTUAL, ";"},
        {lexer::token_type::END_OF_FILE, ""}
    });

    // Test 9: Arrays and nullable types
    test_lexer("var arr: [i32?] = {};", 9, {
        {lexer::token_type::KEYWORD, "var"},
        {lexer::token_type::IDENTIFIER, "arr"},
        {lexer::token_type::PUNCTUAL, ":"},
        {lexer::token_type::TYPE, "[i32?]"},
        {lexer::token_type::OPERATOR, "="},
        {lexer::token_type::PUNCTUAL, "{"},
        {lexer::token_type::PUNCTUAL, "}"},
        {lexer::token_type::PUNCTUAL, ";"},
        {lexer::token_type::END_OF_FILE, ""}
    });

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
