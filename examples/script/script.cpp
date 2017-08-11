#include <iostream>
#include <fstream>

#include "../../src/litescript.hpp"

using namespace LiteScript;

std::string readfile(const char * name, unsigned int& i) {
    std::string s;
    std::ifstream file(name);
    if (!file.is_open())
        return std::string("error : can't open the file");

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
    for (unsigned int i = 0, sz = args.size(); i < sz; i++) {
        std::cout << args[i];
        if (i < sz - 1)
            std::cout << ", ";
    }
    std::cout << std::endl;
    return s.memory.Create(Type::UNDEFINED);
}

std::array<const char *, 4> command_help({
                                                 "exit : stop execution",
                                                 "file <filepath> : execute script code in a file",
                                                 "help : redisplay this text",
                                                 "enter white line to execute previous code"
                                         });

void display_commands(int commnds_c, const char ** commnds_v) {
    for (unsigned int i = 0; i < commnds_c; i++) {
        if (strlen(commnds_v[i]) != 0)
            std::cout << "- " << commnds_v[i];
        std::cout << std::endl;
    }
}

int main(int argc, char * argv[]) {
    Script script;

    Variable v1 = script.memory.Create(Type::CALLBACK);
    v1->GetData<Callback>() = Callback(script.memory, print_var);
    script.state.GetCurrentNamer().Declare("print", v1);

    if (argc > 1) {
        unsigned int i;
        std::string code = readfile(argv[1], i);
        if (code == "error : can't open the file") {
            std::cout << code << " \"" << argv[1] << "\"" << std::endl;
            return -1;
        }

        Variable result = script.Execute(code.c_str());
        if (script.error != Script::ErrorType::SCRPT_ERROR_NO) {
            std::cout << "ERROR(" << (script.line_error + 1) << "," << (script.col_error + 1) << "): " << script.GetError() << std::endl;
            return 1;
        }
        else
            std::cout << (std::string)(result) << std::endl;
        return 0;
    }

    char cmd[256];
    std::string code;
    unsigned int line_num;
    std::cout << "Enter script code :" << std::endl;
    display_commands(command_help.size(), command_help.data());
    while (true) {
        line_num = 1;
        std::cout << std::endl;
        code.clear();
        while (true) {
            std::cout << line_num << " > ";
            std::cin.getline(cmd, 256);
            if (strncmp(cmd, "file ", 5) == 0) {
                code += readfile(cmd + 5, line_num);
                break;
            }
            else if (strncmp(cmd, "help", 4) == 0) {
                std::cout << "Commands :" << std::endl;
                display_commands(command_help.size(), command_help.data());
                std::cout << std::endl;
                continue;
            }
            else if (strcmp(cmd, "exit") == 0)
                return 0;
            else if (cmd[0] == '\0')
                break;
            code += cmd;
            code += '\n';
            line_num++;
        }
        code.pop_back();

        Variable result = script.Execute(code.c_str());
        if (script.error != Script::ErrorType::SCRPT_ERROR_NO)
            std::cout << "ERROR(" << (script.line_error + 1) << "," << (script.col_error + 1) << "): " << script.GetError() << std::endl;
    }
}