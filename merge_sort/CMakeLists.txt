cmake_minimum_required(VERSION 3.8)
project(Structs)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -std=c++14 -O0 -Wall -Werror -Wsign-compare")
add_executable(Structs ${SOURCE_FILES})
