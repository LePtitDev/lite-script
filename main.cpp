#include <iostream>
#include <fstream>

#include "src/litescript.hpp"

using namespace LiteScript;

int main(int argc, char * argv[]) {
    Assembly assembly;
    char cmd[256];
    unsigned int line_num = 1;
    std::cout << "Enter assembly code : (\"exit\" for stop execution)" << std::endl;
    while (true) {
        std::cout << line_num << " > ";
        std::cin.getline(cmd, 256);
        if (strcmp(cmd, "exit") == 0)
            break;

        Variable result = assembly.ExecuteSingle(cmd);
        if (assembly.error != Assembly::ErrorType::ASSM_ERROR_NO) {
            std::cout << "ERROR: " << assembly.GetError() << std::endl;
        }
        else {
            std::cout << (std::string)(result) << std::endl;
            line_num++;
        }
    }

    return 0;
}