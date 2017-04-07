#include <iostream>

#include "src/litescript.hpp"

using namespace LiteScript;

int main() {
    Object obj = Type::BOOLEAN.CreateObject();
    obj.GetData<bool>() = true;
    std::cout << obj.ToString() << std::endl;

    return 0;
}