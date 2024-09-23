//
// Created by alpluspluss 9/22/2024 AD.
//

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include "include/lexer.h"

std::string readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open the file: " + filename);
    }

    const std::streamsize size = file.tellg();
    if (size <= 0)
    {
        throw std::runtime_error("File is empty or cannot determine the size.");
    }

    std::string buffer(size, '\0');
    file.seekg(0, std::ios::beg);
    if (!file.read(&buffer[0], size))
    {
        throw std::runtime_error("Failed to read the file content.");
    }

    return buffer;
}

int main(const int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return EXIT_FAILURE;
    }

    const std::string filename = argv[1];
    try
    {
        auto start = std::chrono::high_resolution_clock::now();
        const std::string fileContent = readFile(filename);
        auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> fileReadTime = end - start;
        std::cout << "File read in " << fileReadTime.count() << " seconds." << std::endl;

        lexer::lexer_t lexer;
        lexerInit(lexer, fileContent);

        start = std::chrono::high_resolution_clock::now();
        const std::vector<lexer::token_t> tokens = tokenize(lexer);
        end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> tokenizeTime = end - start;
        std::cout << "Tokenized in " << tokenizeTime.count() << " seconds." << std::endl;

        std::cout << "Number of tokens: " << tokens.size() << std::endl;

        for (const auto&[type, value] : tokens)
        {
            std::cout << "Token: " << value << " Type: " << static_cast<int>(type) << std::endl;
        }

        flushErrors(lexer);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
