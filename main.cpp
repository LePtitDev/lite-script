#include <iostream>
#include <fstream>

#include "src/litescript.hpp"

using namespace LiteScript;

int main(int argc, char * argv[]) {
    Object  obj1 = Type::STRING.CreateObject(),
            obj2 = Type::NUMBER.CreateObject(),
            obj3 = Type::BOOLEAN.CreateObject();
    obj1.GetData<String>() = String("LOL ");
    obj2.GetData<Number>() = Number(3);
    obj3.GetData<bool>() = true;

    obj1 *= obj2;
    obj1 += obj2;
    obj1 += obj3;
    obj1 += Type::NIL.CreateObject();
    std::cout << obj1 + obj2 << std::endl;

    return 0;
}