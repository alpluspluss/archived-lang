#include <fstream>
#include <iostream>
#include <string>
#include "include/lexer.h"

std::string readFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Error: Could not open file: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    return {(std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()};
}

int main(const int argc, char* argv[])
{
    std::string input;
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <source_code_string | filename>" << std::endl;
        return EXIT_FAILURE;
    }

    if (std::string(argv[1]) == "-f")
    {
        if (argc < 3) {
            std::cerr << "Error: No filename provided after '-f' flag." << std::endl;
            return EXIT_FAILURE;
        }
        input = readFile(argv[2]);
    } else {
        input = argv[1];
    }

    lexer::lexer_t lexer;
    lexerInit(lexer, input);

    for (const auto& [type, value] : lexer::tokenize(lexer))
    {
        std::cout << "Token Type: " << static_cast<int>(type) << ", Value: " << value << std::endl;
    }

    flushErrors(lexer);

    return 0;
}
