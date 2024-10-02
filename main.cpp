//
// Created by alpluspluss 9/22/2024 AD.
//

#include <chrono>
#include <iostream>
#include <ostream>

#include "lang/lang.h"

int main()
{
    constexpr std::string_view source = R"(
        package stdlib.io;
        import stdlib.net;
        class Example
        {
            var x: i32 = 42;
            function foo() -> i32
            {
                return x + 1;
            }
        };

        class Generic<T>
        {
            var value: T;
            function getValue(): T
            {
                return value;
            }
        };
    )";

    lexer::lexer_t lexer;
    lexer_init(lexer, source);

    const auto start = std::chrono::high_resolution_clock::now();
    const auto tokens = tokenize(lexer);
    const auto end = std::chrono::high_resolution_clock::now();

    const std::chrono::duration<double> duration = end - start;
    const double tokens_per_second = tokens.size() / duration.count();

    std::cout << "Tokenized " << tokens.size() << " tokens in " << duration.count() << " seconds.\n";
    std::cout << "Tokens per second: " << tokens_per_second << "\n";

    return 0;
}