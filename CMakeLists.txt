cmake_minimum_required(VERSION 2.8.4)
project(LiteScriptProj)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Add the library
add_subdirectory (src)

# Add examples
add_subdirectory(examples)