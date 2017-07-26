cmake_minimum_required(VERSION 3.6)
project(LiteScript)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
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
        src/types/array.hpp src/types/array.cpp
        src/types/class.hpp src/types/class.cpp
        src/types/class_object.hpp src/types/class_object.cpp
        src/types/namespace.hpp src/types/namespace.cpp

        src/memory/memory.hpp src/memory/memory.cpp
        src/memory/variable.hpp src/memory/variable.cpp
        src/memory/basic_memory.hpp src/memory/basic_memory.cpp

        src/script/instruction.hpp src/script/instruction.cpp
        src/script/state.hpp src/script/state.cpp
        src/script/executor.hpp src/script/executor.cpp
        src/script/assembly.hpp src/script/assembly.cpp
        src/script/syntax.hpp src/script/syntax.cpp src/script/script.hpp src/script/script.cpp)

add_executable(LiteScript main.cpp ${SOURCE_FILES})