cmake_minimum_required(VERSION 3.29.6)
project(lumen-lang LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS )
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(lumen-lang
        main.cpp
        lang/lexer.cpp
        lang/lang.h
        lang/parser.cpp
)
add_executable(test-lexer
        lang/lexer.cpp
        tests/test_lexer.cpp
)

enable_testing()
add_test(NAME LexerTest COMMAND test-lexer)
