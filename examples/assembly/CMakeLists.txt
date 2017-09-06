# Add an executable for the assembly example
add_executable (assembly assembly.cpp)

# Target it to the LiteScript library
target_link_libraries (assembly LINK_PUBLIC LiteScript)