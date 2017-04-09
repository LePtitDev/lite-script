#include <iostream>
#include <fstream>

#include "src/litescript.hpp"

using namespace LiteScript;

int main(int argc, char * argv[]) {
    Object  obj1 = Type::STRING.CreateObject(),
            obj2 = Type::NUMBER.CreateObject(),
            obj3 = Type::BOOLEAN.CreateObject();
    obj1.GetData<String>() = String("LOL ");
    obj2.GetData<Number>() = Number(2);
    obj3.GetData<bool>() = true;

    obj1[obj2] = Object(obj1);
    std::cout << (obj1) << std::endl;

    return 0;
}