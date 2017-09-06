#include <cstring>
#include <iostream>
#include <fstream>

#include "../../src/litescript.hpp"

using namespace LiteScript;

/**
 * Read the content of a file
 * @param name The filename
 * @param i The line number reference
 * @return The content of the file
 */
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

/**
 * The print callback to include in the state of script
 * @param s The assembly state
 * @param args The arguments
 * @return Undefined value
 */
Variable print_var(State& s, std::vector<Variable>& args) {
    for (unsigned int i = 0, sz = args.size(); i < sz; i++) {
        std::cout << args[i];
        if (i < sz - 1)
            std::cout << ", ";
    }
    std::cout << std::endl;
    return s.memory.Create(Type::UNDEFINED);
}

/**
 * The commands
 */
std::array<const char *, 4> command_help({
                                                 "exit : stop execution",
                                                 "file <filepath> : execute script code in a file",
                                                 "help : redisplay this text",
                                                 "enter white line to execute previous code"
                                         });


/**
 * For display commands
 * @param commnds_c The number of strings
 * @param commnds_v The strings
 */
void display_commands(int commnds_c, const char ** commnds_v) {
    for (unsigned int i = 0; i < commnds_c; i++) {
        if (strlen(commnds_v[i]) != 0)
            std::cout << "- " << commnds_v[i];
        std::cout << std::endl;
    }
}


/**
 * The main fonction
 */
int main(int argc, char * argv[]) {
    // We create the main memory and the script state
    Memory memory;
    State state(memory);
    // We create an assembly object
    Script script(state);

    // We add the "print" callback in the global namespace
    DeclareVariable(state, "print", CreateVariable(memory, print_var));

    // If there is a parameter, it's a file assembly script to execute
    if (argc > 1) {
        unsigned int i;
        // We read the file content
        std::string code = readfile(argv[1], i);
        if (code == "error : can't open the file") {
            std::cout << code << " \"" << argv[1] << "\"" << std::endl;
            return -1;
        }

        // We execute the script
        script.Execute(code.c_str());
        // If there is an error, we print it
        if (script.error != Script::ErrorType::SCRPT_ERROR_NO) {
            std::cout << "ERROR(" << (script.line_error + 1) << "," << (script.col_error + 1) << "): " << script.GetError() << std::endl;
            return 1;
        }
        return 0;
    }

    // Else we read lines on the keyboard
    // (the script execution is relatively the same as above)
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

        script.Execute(code.c_str());
        if (script.error != Script::ErrorType::SCRPT_ERROR_NO)
            std::cout << "ERROR(" << (script.line_error + 1) << "," << (script.col_error + 1) << "): " << script.GetError() << std::endl;
    }
}