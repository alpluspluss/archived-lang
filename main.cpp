#include <iostream>
#include <vector>

#include "lang/lang.h"

int main()
{
    constexpr std::string_view source = R"(function() -> void { var x: i32 = 0; })";

    lexer::lexer_t lexer;
    lexer_init(lexer, source);

    const std::vector<lexer::token_t> tokens = tokenize(lexer);
    flush_errors(lexer);
    std::cout << "Token count: " << tokens.size() << std::endl;
    parser::parser_t parser;
    parser_init(parser, tokens);

    if (parse_program(parser))
    {
        std::cout << "Parsing completed successfully." << "\n";
    }
    else
    {
        std::cerr << "Parsing failed." << "\n";
    }

    lexer.tokens.clear();
    lexer.tokens.shrink_to_fit();

    return 0;
}
