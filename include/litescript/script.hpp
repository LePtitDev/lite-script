#ifndef LITESCRIPT_SCRIPT_HPP
#define LITESCRIPT_SCRIPT_HPP

#include <cstddef>
#include <string_view>
#include <vector>

#include "instruction.hpp"
#include "state.hpp"

namespace LiteScript {

    // The object-oriented script language (similar to Javascript)
    class Script {

    public:

        // List of errors
        enum class Error : std::uint8_t {
            None,
            SimpleArrayEnd,
            NamedArrayColon,
            NamedArrayExpression,
            NamedArrayEnd,
            CallbackArguments,
            CallbackArgumentName,
            CallbackArgumentEnd,
            CallbackInstructions,
            ClassInherit,
            ClassBegin,
            ClassInvalid,
            ClassOperatorName,
            ClassOperatorInvalid,
            ClassEnd,
            ForInitialisation,
            ForCondition,
            SwitchKeywords,
            SwitchEnd,
            Name,
            Value,
            Operator,
            Expression,
            ParenthesisOpen,
            ParenthesisClose,
            BracketClose,
            BraceOpen,
            BraceClose,
            Colon,
            Semicolon,
            Instruction,
            Return,
            While,
            Namespace,
            Unknown,
            UnterminatedString,
            BreakOutsideLoop,
            ContinueOutsideLoop,
            Nesting,
            Catch
        };

        // The result of a compilation
        struct Result {

            // The instructions (empty if there is an error)
            std::vector<LiteScript::Instruction> instructions;

            // The error
            Error error = Error::None;

            // The line of the error (starts at 0)
            std::size_t line = 0;

            // The column of the error in characters (starts at 0)
            std::size_t column = 0;

            // The value on the top of the stack after the execution (for Execute)
            Variable value;

            explicit operator bool() const noexcept { return this->error == Error::None; }

            std::string_view GetMessage() const noexcept { return Script::GetErrorMessage(this->error); }

        };

        /**
         * Convert a script code into instructions
         */
        static Result Compile(std::string_view code);

        /**
         * Compile a code, add it in a new list of the state and execute it
         */
        static Result Execute(State& state, std::string_view code);

        /**
         * Return the message of an error
         */
        static std::string_view GetErrorMessage(Error error) noexcept;

    };

}

#endif //LITESCRIPT_SCRIPT_HPP
