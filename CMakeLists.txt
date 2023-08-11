cmake_minimum_required(VERSION 3.26)
project(ColourQuantization)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(ColourQuantization
        src/main.cpp
        src/main.h)
