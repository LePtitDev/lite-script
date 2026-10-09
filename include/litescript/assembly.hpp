#ifndef LITESCRIPT_ASSEMBLY_HPP
#define LITESCRIPT_ASSEMBLY_HPP

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include "instruction.hpp"
#include "state.hpp"

namespace LiteScript {

    // The assembly language: one instruction per line ("value.number 5", "jump-to 12"...)
    // Line numbers used by jumps and callbacks start at 1.
    class Assembly {

    public:

        // List of errors
        enum class Error : std::uint8_t {
            None,
            UnknownCommand,
            ExpectedBoolean,
            ExpectedInteger,
            ExpectedNumber,
            ExpectedString
        };

        // The result of a compilation
        struct Result {

            // The instructions (empty if there is an error)
            std::vector<Instruction> instructions;

            // The error
            Error error = Error::None;

            // The line of the error (starts at 0)
            std::size_t line = 0;

            // The value on the top of the stack after the execution (for Execute and ExecuteAfter)
            Variable value;

            explicit operator bool() const noexcept { return this->error == Error::None; }

            std::string_view GetMessage() const noexcept { return Assembly::GetErrorMessage(this->error); }

        };

        /**
         * Convert an assembly code into instructions
         * (empty lines are converted into invalid instructions that do nothing)
         */
        static Result Compile(std::string_view code);

        /**
         * Convert a line of assembly code into an instruction
         */
        static Instruction CompileInstruction(std::string_view line, Error& error);

        /**
         * Convert instructions into assembly code
         */
        static std::string Disassemble(const std::vector<Instruction>& instructions);

        /**
         * Convert an instruction into a line of assembly code (without line break)
         */
        static std::string DisassembleInstruction(const Instruction& instruction);

        /**
         * Compile a code, add it in a new list of the state and execute it
         */
        static Result Execute(State& state, std::string_view code);

        /**
         * Compile a code, add it at the end of the last list of the state and execute it
         * (line numbers continue after the previous instructions of the list)
         */
        static Result ExecuteAfter(State& state, std::string_view code);

        /**
         * Return the message of an error
         */
        static std::string_view GetErrorMessage(Error error) noexcept;

    };

}

#endif //LITESCRIPT_ASSEMBLY_HPP
