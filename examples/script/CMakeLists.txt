# Add an executable for the script example
add_executable (script script.cpp)

# Target it to the LiteScript library
target_link_libraries (script LINK_PUBLIC LiteScript)