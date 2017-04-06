cmake_minimum_required(VERSION 3.6)
project(LiteScript)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp src/memory/variable.hpp src/memory/type.hpp src/litescript.hpp src/memory/type.cpp src/memory/variable.cpp src/types/type_null.hpp src/types/type_null.cpp)
add_executable(LiteScript ${SOURCE_FILES})