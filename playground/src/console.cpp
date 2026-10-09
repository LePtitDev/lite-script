#include "helpers.hpp"
#include "modules.hpp"

#include <iostream>
#include <iterator>

namespace {

    using namespace Playground;

    // Write the texts of the values separated by spaces
    void WriteValues(std::ostream& stream, const Args& args) {
        for (std::size_t i = 0; i < args.size(); i++) {
            if (i > 0)
                stream << ' ';
            stream << ToText(args[i]).ToUTF8();
        }
    }

}

void Playground::InstallConsole(State& state) {
    Memory& memory = state.GetMemory();
    const Variable& global = state.GetGlobalNamespace();

    Define(state, global, "", "print", [](State& s, Args& args) {
        WriteValues(std::cout, args);
        std::cout << '\n';
        return s.GetMemory().CreateUndefined();
    });
    Define(state, global, "", "assert", [](State& s, Args& args) {
        if (args.empty() || !IsTruthy(args[0])) {
            std::string message = "assertion failed";
            if (!IsMissing(args, 1))
                message += ": " + ToText(args[1]).ToUTF8();
            throw std::runtime_error(message);
        }
        return s.GetMemory().CreateUndefined();
    });

    Variable console = memory.CreateNamespace();
    Define(state, console, "Console.", "write", [](State& s, Args& args) {
        WriteValues(std::cout, args);
        std::cout.flush();
        return s.GetMemory().CreateUndefined();
    });
    Define(state, console, "Console.", "writeLine", [](State& s, Args& args) {
        WriteValues(std::cout, args);
        std::cout << '\n';
        return s.GetMemory().CreateUndefined();
    });
    Define(state, console, "Console.", "error", [](State& s, Args& args) {
        std::cout.flush();
        WriteValues(std::cerr, args);
        std::cerr << std::endl;
        return s.GetMemory().CreateUndefined();
    });
    // readLine(prompt) gives the next line of the standard input, null at the end of the input
    Define(state, console, "Console.", "readLine", [](State& s, Args& args) {
        if (!IsMissing(args, 0))
            std::cout << ToText(args[0]).ToUTF8();
        std::cout.flush();
        std::string line;
        if (!std::getline(std::cin, line))
            return s.GetMemory().CreateNull();
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        return s.GetMemory().CreateString(line);
    });
    Define(state, console, "Console.", "readAll", [](State& s, Args&) {
        std::string content(std::istreambuf_iterator<char>(std::cin), {});
        return s.GetMemory().CreateString(content);
    });
    state.DeclareVariable("Console", console);
}
