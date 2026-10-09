#ifndef LITESCRIPT_TESTS_HELPERS_HPP
#define LITESCRIPT_TESTS_HELPERS_HPP

#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include <doctest/doctest.h>
#include <litescript/litescript.hpp>

namespace Tests {

    /**
     * Return the number of memory blocks currently allocated with operator new
     */
    std::int64_t LiveAllocations();

    /**
     * Execute a function and return the number of memory blocks it leaked
     * (the function must not use doctest assertions, they allocate memory)
     */
    template <typename F>
    std::int64_t LeakedAllocations(F&& f) {
        // The blocks kept to create the next objects aren't leaks
        LiteScript::Object::ReleaseCachedBlocks();
        std::int64_t before = LiveAllocations();
        f();
        LiteScript::Object::ReleaseCachedBlocks();
        return LiveAllocations() - before;
    }

    // A memory and a state with a "print" function that stores what it prints
    struct Engine {

        LiteScript::Memory memory;
        LiteScript::State state{ memory };
        std::vector<std::string> output;

        explicit Engine(std::size_t gc_threshold = 4096) : memory(gc_threshold) {
            this->state.DeclareVariable("print", this->memory.RegisterFunction("print",
                [this](LiteScript::State& s, std::vector<LiteScript::Variable>& args) {
                    std::string line;
                    for (std::size_t i = 0; i < args.size(); i++) {
                        if (i > 0)
                            line += ' ';
                        line += LiteScript::ToText(args[i]).ToUTF8();
                    }
                    this->output.push_back(line);
                    return s.GetMemory().CreateUndefined();
                }));
        }

        // Compile and execute a script, fails the test if there is a syntax error
        LiteScript::Variable Run(std::string_view code) {
            auto result = LiteScript::Script::Execute(this->state, code);
            INFO("line ", result.line + 1, ", column ", result.column + 1, ": ", result.GetMessage());
            REQUIRE(result);
            return result.value;
        }

        // Evaluate an expression and return its description
        std::string Eval(std::string_view expression) {
            return this->Run("return " + std::string(expression) + ";").ToString();
        }

        // Return the lines printed, separated by line breaks
        std::string Output() const {
            std::string res;
            for (const std::string& line : this->output) {
                if (!res.empty())
                    res += '\n';
                res += line;
            }
            return res;
        }

    };

}

#endif //LITESCRIPT_TESTS_HELPERS_HPP
