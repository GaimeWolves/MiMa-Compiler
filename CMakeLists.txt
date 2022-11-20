cmake_minimum_required(VERSION 3.24)
project(MIMA_Compiler)

set(CMAKE_CXX_STANDARD 23)

add_executable(MIMA_Compiler main.cpp lexer.cpp parser.cpp debug.cpp generator.cpp)
