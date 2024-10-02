cmake_minimum_required(VERSION 3.29.6)
project(quanta-lang LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_DEPENDS_USE_COMPILER OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(quanta-lang
        main.cpp
        lang/lexer.cpp
        lang/lang.h
)
add_executable(test_lexer
        lang/lexer.cpp
        tests/test_lexer.cpp
)

enable_testing()
add_test(NAME LexerTest COMMAND test_lexer)
