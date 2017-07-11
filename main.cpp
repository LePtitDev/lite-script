#include <iostream>
#include <fstream>

#include "src/litescript.hpp"

using namespace LiteScript;

std::string readfile(const char * name, unsigned int& i) {
    std::string s;
    std::ifstream file(name);
    if (!file.is_open())
        return s;

    char tmp[256];
    while (file.getline(tmp, 256)) {
        s += tmp;
        s += '\n';
        i++;
    }
    file.close();

    return s;
}

Variable print_var(State& s, std::vector<Variable>& args) {
    std::cout << "print: ";
    for (unsigned int i = 0, sz = args.size(); i < sz; i++) {
        std::cout << args[i];
        if (i < sz - 1)
            std::cout << ", ";
    }
    std::cout << std::endl;
    return s.memory.Create(Type::UNDEFINED);
}

int main(int argc, char * argv[]) {
    Assembly assembly;

    Variable v1 = assembly.memory.Create(Type::CALLBACK);
    v1->GetData<Callback>() = Callback(assembly.state, print_var);
    assembly.state.GetCurrentNamespace()->GetData<Namespace>().DefineVariable("print", v1);

    char cmd[256];
    std::string code;
    unsigned int line_num = 1, prev_num = 1;
    std::cout << "Enter assembly code :" << std::endl;
    std::cout << "- \"exit\" to stop execution" << std::endl;
    std::cout << "- enter white line to execute previous code" << std::endl;
    while (true) {
        std::cout << std::endl;
        code.clear();
        while (true) {
            std::cout << line_num << " > ";
            std::cin.getline(cmd, 256);
            if (strncmp(cmd, "file ", 5) == 0) {
                code += readfile(cmd + 5, line_num);
                break;
            }
            if (strcmp(cmd, "exit") == 0)
                return 0;
            else if (cmd[0] == '\0')
                break;
            code += cmd;
            code += '\n';
            line_num++;
        }
        code.pop_back();

        Variable result = assembly.ExecuteAfter(code.c_str());
        if (assembly.error != Assembly::ErrorType::ASSM_ERROR_NO) {
            std::cout << "ERROR(" << assembly.line_error + prev_num << "): " << assembly.GetError() << std::endl;
            line_num = prev_num;
        }
        else {
            std::cout << (std::string)(result) << std::endl;
            prev_num = line_num;
        }
    }
}