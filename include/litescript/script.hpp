#ifndef LITESCRIPT_SCRIPT_HPP
#define LITESCRIPT_SCRIPT_HPP

#include <cstddef>
#include <cstdint>
#include <string>
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

        // Information about the declarations and the names of a script, for the development tools
        // (editors, debuggers). Filled during the compilation, partially if there is an error.
        struct Analysis {

            static constexpr std::size_t NONE = static_cast<std::size_t>(-1);

            // A position in the code (line and column start at 0, the column is counted in characters)
            struct Position {
                std::size_t line = 0;
                std::size_t column = 0;
                // The offset in bytes
                std::size_t offset = 0;
            };

            struct Range {
                Position start;
                Position end;
            };

            // A part of the code where variables are declared
            struct Scope {
                enum class Kind : std::uint8_t {
                    // The whole script
                    Script,
                    // The arguments and the body of a function
                    Function,
                    // The members of a class (between the braces)
                    Class
                };
                Kind kind = Kind::Script;
                Range range;
                // The scope that contains this one (NONE for the script)
                std::size_t parent = NONE;
            };

            enum class SymbolKind : std::uint8_t {
                Variable,
                Function,
                Class,
                Parameter,
                // Attribute of a class
                Attribute,
                // Method of a class
                Method,
                // Overloaded operator of a class
                Operator,
                // Variable of a "catch" block
                CatchVariable,
                // Namespace selected by the "namespace" instruction
                Namespace
            };

            // A declaration
            struct Symbol {
                std::string name;
                SymbolKind kind = SymbolKind::Variable;
                // Static member of a class
                bool is_static = false;
                // The range of the name
                Range range;
                // The range of the whole declaration (with the body of a function or a class)
                Range full;
                // The scope that contains the declaration
                std::size_t scope = NONE;
                // The scope created by a function or a class (NONE for the others)
                std::size_t body = NONE;
                // The namespace selected when the symbol is declared (empty for the global namespace)
                std::string nspace;
                // The arguments of a function (the last one ends with "..." if it gathers the remaining arguments)
                std::vector<std::string> parameters;
                // The instructions of a function: [first, end[ (NONE for the others)
                std::size_t first_instruction = NONE;
                std::size_t end_instruction = NONE;
            };

            // A use of a name
            struct Reference {
                std::string name;
                Range range;
                // The scope that contains the use
                std::size_t scope = NONE;
                // Member access ("object.name")
                bool member = false;
                // The namespace selected at the position of the use (empty for the global namespace)
                std::string nspace;
            };

            // The beginning of an instruction of the language (used for the breakpoints and the steps)
            struct Statement {
                Position position;
                // The index of the first instruction generated by the statement
                std::size_t instruction = 0;
            };

            std::vector<Scope> scopes;
            std::vector<Symbol> symbols;
            std::vector<Reference> references;
            std::vector<Statement> statements;

        };

        /**
         * Convert a script code into instructions
         *
         * @param analysis If given, receives the declarations and the names of the script
         */
        static Result Compile(std::string_view code, Analysis * analysis = nullptr);

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
