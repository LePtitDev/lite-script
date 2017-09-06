# Define source files
set(SOURCE_FILES
        litescript.hpp
        nullable.hpp
        streamer.hpp

        memory/type.hpp memory/type.cpp
        memory/object.hpp memory/object.cpp

        types/internal.hpp types/internal.cpp
        types/undefined.hpp types/undefined.cpp
        types/null.hpp types/null.cpp
        types/boolean.hpp types/boolean.cpp
        types/number.hpp types/number.cpp
        types/string.hpp types/string.cpp
        types/character.hpp types/character.cpp
        types/callback.hpp types/callback.cpp
        types/array.hpp types/array.cpp
        types/class.hpp types/class.cpp
        types/class_object.hpp types/class_object.cpp
        types/namespace.hpp types/namespace.cpp

        memory/memory.hpp memory/memory.cpp
        memory/variable.hpp memory/variable.cpp
        memory/basic_memory.hpp memory/basic_memory.cpp

        script/instruction.hpp script/instruction.cpp
        script/namer.hpp script/namer.cpp
        script/state.hpp script/state.cpp
        script/executor.hpp script/executor.cpp
        script/assembly.hpp script/assembly.cpp
        script/syntax.hpp script/syntax.cpp
        script/script.hpp script/script.cpp

        api/types.hpp api/types.cpp
        api/script.hpp api/script.cpp)

# Add the library
add_library(LiteScript ${SOURCE_FILES})

# Define target
target_include_directories (LiteScript PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})