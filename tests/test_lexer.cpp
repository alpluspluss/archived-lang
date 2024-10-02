#include <cassert>
#include <iostream>
#include "../lang/lang.h"

void test_basic_tokenization()
{
    lexer::lexer_t lexer;
    lexer_init(lexer, "var x = 10;");

    auto tokens = tokenize(lexer);

    assert(tokens.size() == 6);
    assert(tokens[0].type == lexer::token_type::KEYWORD);
    assert(tokens[1].type == lexer::token_type::IDENTIFIER);
    assert(tokens[2].type == lexer::token_type::OPERATOR);
    assert(tokens[3].type == lexer::token_type::LITERAL);
    assert(tokens[4].type == lexer::token_type::PUNCTUAL);
    assert(tokens[5].type == lexer::token_type::END_OF_FILE);

    std::cout << "Basic Passed" << std::endl;
}

void test_identifier_tokenization()
{
    lexer::lexer_t lexer;
    lexer_init(lexer, "my_var myOtherVar");

    auto tokens = tokenize(lexer);

    assert(tokens.size() == 3);
    assert(tokens[0].type == lexer::token_type::IDENTIFIER);
    assert(tokens[0].lexeme == "my_var");
    assert(tokens[1].type == lexer::token_type::IDENTIFIER);
    assert(tokens[1].lexeme == "myOtherVar");
    assert(tokens[2].type == lexer::token_type::END_OF_FILE);

    std::cout << "Identifier Test Passed" << std::endl;
}

void test_numeric_literal()
{
    lexer::lexer_t lexer;
    lexer_init(lexer, "42 3.14 0xFF");

    auto tokens = tokenize(lexer);

    assert(tokens.size() == 4);
    assert(tokens[0].type == lexer::token_type::LITERAL);
    assert(tokens[0].lexeme == "42");
    assert(tokens[1].type == lexer::token_type::LITERAL);
    assert(tokens[1].lexeme == "3.14");
    assert(tokens[2].type == lexer::token_type::LITERAL);
    assert(tokens[2].lexeme == "0xFF");
    assert(tokens[3].type == lexer::token_type::END_OF_FILE);

    std::cout << "Numeric Literal Passed" << std::endl;
}

void test_string_literal()
{
    lexer::lexer_t lexer;
    lexer_init(lexer, "\"hello world\"");

    auto tokens = tokenize(lexer);

    assert(tokens.size() == 2);
    assert(tokens[0].type == lexer::token_type::STRING);
    assert(tokens[0].lexeme == "\"hello world\"");
    assert(tokens[1].type == lexer::token_type::END_OF_FILE);

    std::cout << "String Literal Passed" << std::endl;
}

void test_single_line_comment()
{
    lexer::lexer_t lexer;
    lexer_init(lexer, "var x = 10; // This is a comment");

    auto tokens = tokenize(lexer);

    assert(tokens.size() == 6);
    assert(tokens[0].type == lexer::token_type::KEYWORD);
    assert(tokens[4].type == lexer::token_type::PUNCTUAL);
    assert(tokens[5].type == lexer::token_type::END_OF_FILE);

    std::cout << "Sigle Line Comment Passed" << std::endl;
}

void test_block_comment()
{
    lexer::lexer_t lexer;
    lexer_init(lexer, "var y = 20; /* This is a block comment */ var z = 30;");

    auto tokens = tokenize(lexer);

    assert(tokens.size() == 10);
    assert(tokens[0].type == lexer::token_type::KEYWORD);
    assert(tokens[5].type == lexer::token_type::KEYWORD);
    assert(tokens[9].type == lexer::token_type::END_OF_FILE);

    std::cout << "Block comment Passed" << std::endl;
}

void test_basic_types()
{
    lexer::lexer_t lexer;
    lexer_init(lexer, "i32 i64 string boolean void");

    auto tokens = tokenize(lexer);

    assert(tokens.size() == 6);
    assert(tokens[0].type == lexer::token_type::TYPE);
    assert(tokens[0].lexeme == "i32");
    assert(tokens[1].type == lexer::token_type::TYPE);
    assert(tokens[1].lexeme == "i64");
    assert(tokens[2].type == lexer::token_type::TYPE);
    assert(tokens[2].lexeme == "string");
    assert(tokens[3].type == lexer::token_type::TYPE);
    assert(tokens[3].lexeme == "boolean");
    assert(tokens[4].type == lexer::token_type::TYPE);
    assert(tokens[4].lexeme == "void");
    assert(tokens[5].type == lexer::token_type::END_OF_FILE);

    std::cout << "Basic Types Passed" << std::endl;
}

void test_nullable_types()
{
    lexer::lexer_t lexer;
    lexer_init(lexer, "i32? string?");

    auto tokens = tokenize(lexer);

    assert(tokens.size() == 3);
    assert(tokens[0].type == lexer::token_type::NULLABLE_TYPE);
    assert(tokens[0].lexeme == "i32?");
    assert(tokens[1].type == lexer::token_type::NULLABLE_TYPE);
    assert(tokens[1].lexeme == "string?");
    assert(tokens[2].type == lexer::token_type::END_OF_FILE);

    std::cout << "Nullable Type Passed" << std::endl;
}

void test_array_types()
{
    lexer::lexer_t lexer;
    lexer_init(lexer, "[i32] [string]");

    auto tokens = tokenize(lexer);

    assert(tokens.size() == 3);
    assert(tokens[0].type == lexer::token_type::TYPE);
    assert(tokens[0].lexeme == "[i32]");
    assert(tokens[1].type == lexer::token_type::TYPE);
    assert(tokens[1].lexeme == "[string]");
    assert(tokens[2].type == lexer::token_type::END_OF_FILE);

    std::cout << "Array Types Passed" << std::endl;
}

void test_annotation_types()
{
    lexer::lexer_t lexer;
    lexer_init(lexer, "@deprecated @test @aligned(16)");
    auto tokens = tokenize(lexer);

    assert(tokens.size() == 3);
    assert(tokens[0].type == lexer::token_type::ANNOTATION);
    assert(tokens[0].value == "@deprecated");
    assert(tokens[1].type == lexer::token_type::ANNOTATION);
    assert(tokens[1].value == "@test");
    assert(tokens[2].type == lexer::token_type::ANNOTATION);
    assert(tokens[2].value == "@aligned(16)");
    assert(tokens[3].type == lexer::token_type::END_OF_FILE);

    std::cout << "Annotation Types Passed" << std::endl;
}

int main()
{
    test_basic_tokenization();
    test_identifier_tokenization();
    test_numeric_literal();
    test_string_literal();
    test_single_line_comment();
    test_block_comment();
    test_basic_types();
    test_nullable_types();
    test_array_types();
    test_annotation_types();

    std::cout << "All Tests Passed" << std::endl;
    return 0;
}
