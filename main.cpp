#include <iostream>
#include <fstream>

#include "src/litescript.hpp"

using namespace LiteScript;

int main(int argc, char * argv[]) {
    Object obj1 = Type::NUMBER.CreateObject(), obj2 = Type::NUMBER.CreateObject();
    obj1.GetData<Number>() = Number(12);
    obj2.GetData<Number>() = Number(3.5);
    std::cout << (std::string)(-obj1) << std::endl;

    return 0;
}