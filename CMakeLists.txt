cmake_minimum_required(VERSION 3.29.6)
project(README LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

add_executable(quanta-lang
        main.cpp
        src/lexer.cpp
)

enable_testing()
add_test(NAME LexerTest COMMAND test_lexer)
