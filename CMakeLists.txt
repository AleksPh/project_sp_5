cmake_minimum_required(VERSION 3.24)
project(SP5 C)

set(CMAKE_C_STANDARD 23)

add_executable(SP5 main.c dfa.c dfa.h)