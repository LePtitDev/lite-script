cmake_minimum_required(VERSION 3.6)
project(LiteScript)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        main.cpp

        src/litescript.hpp
        src/nullable.hpp

        src/memory/type.hpp src/memory/type.cpp
        src/memory/object.hpp src/memory/object.cpp

        src/types/internal.hpp src/types/internal.cpp
        src/types/undefined.hpp src/types/undefined.cpp
        src/types/null.hpp src/types/null.cpp
        src/types/boolean.hpp src/types/boolean.cpp
        src/types/number.hpp src/types/number.cpp
        src/types/string.hpp src/types/string.cpp
        src/types/character.hpp src/types/character.cpp
        src/types/callback.hpp src/types/callback.cpp

        src/memory/memory.hpp src/memory/memory.cpp
        src/memory/variable.hpp src/memory/variable.cpp
        src/memory/basic_memory.hpp src/memory/basic_memory.cpp)

add_executable(LiteScript ${SOURCE_FILES})