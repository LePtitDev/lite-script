#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

#include <litescript/litescript.hpp>

using namespace LiteScript;

/**
 * Read the content of a file
 * @param name The filename
 * @return The content of the file or nullopt if the file can't be opened
 */
std::optional<std::string> ReadFile(const std::string& name) {
    std::ifstream file(name, std::ios::binary);
    if (!file.is_open())
        return std::nullopt;
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

/**
 * The print function to include in the state of script
 * @param state The script state
 * @param args The arguments
 * @return Undefined value
 */
Variable Print(State& state, std::vector<Variable>& args) {
    for (std::size_t i = 0; i < args.size(); i++) {
        // Strings are displayed without quotes
        std::cout << ToText(args[i]).ToUTF8();
        if (i + 1 < args.size())
            std::cout << ", ";
    }
    std::cout << std::endl;
    return state.GetMemory().CreateUndefined();
}

/**
 * Execute a code and display the syntax error if there is one
 * @return true if the code has been executed
 */
bool Run(State& state, const std::string& code) {
    Script::Result result = Script::Execute(state, code);
    if (!result)
        std::cout << "ERROR(" << (result.line + 1) << "," << (result.column + 1) << "): " << result.GetMessage() << std::endl;
    return static_cast<bool>(result);
}

/**
 * The commands
 */
constexpr const char * COMMANDS_HELP[] = {
    "exit : stop execution",
    "file <filepath> : execute script code in a file",
    "help : redisplay this text",
    "enter white line to execute previous code"
};

void DisplayCommands() {
    for (const char * command : COMMANDS_HELP)
        std::cout << "- " << command << std::endl;
}

/**
 * The main function
 */
int main(int argc, char * argv[]) {
    // We create the main memory and the script state
    Memory memory;
    State state(memory);

    // We add the "print" function in the global namespace
    state.DeclareVariable("print", memory.RegisterFunction("print", Print));

    // If there is a parameter, it's a script file to execute
    if (argc > 1) {
        auto code = ReadFile(argv[1]);
        if (!code) {
            std::cout << "error : can't open the file \"" << argv[1] << "\"" << std::endl;
            return -1;
        }
        return Run(state, *code) ? 0 : 1;
    }

    // Else we read lines on the keyboard
    std::cout << "Enter script code :" << std::endl;
    DisplayCommands();
    while (true) {
        std::cout << std::endl;
        std::string code, line;
        for (unsigned int line_num = 1; true; line_num++) {
            std::cout << line_num << " > ";
            if (!std::getline(std::cin, line) || line == "exit")
                return 0;
            if (line.rfind("file ", 0) == 0) {
                auto content = ReadFile(line.substr(5));
                if (content)
                    code += *content;
                else
                    std::cout << "error : can't open the file" << std::endl;
                break;
            }
            if (line == "help") {
                std::cout << "Commands :" << std::endl;
                DisplayCommands();
                line_num--;
                continue;
            }
            if (line.empty())
                break;
            code += line + '\n';
        }
        Run(state, code);
    }
}
