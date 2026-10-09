#ifndef LITESCRIPT_ERROR_HPP
#define LITESCRIPT_ERROR_HPP

#include <stdexcept>
#include <string>

#include "instruction.hpp"
#include "variable.hpp"

namespace LiteScript {

    // An error of a script that hasn't been caught by a "try ... catch" block
    // (raised by "throw", by an invalid operation or by an exception of a native function)
    class ScriptError : public std::runtime_error {

        // The thrown value
        Variable value;

        // The position of the instruction that raised the error
        SourcePosition position;

    public:

        ScriptError(Variable thrown, const std::string& message, SourcePosition pos) :
            std::runtime_error(message), value(std::move(thrown)), position(pos) {}

        /**
         * Return the thrown value (for the errors of the engine: an array with the members
         * "message", "line" and "column", the line and the column start at 1)
         */
        const Variable& GetValue() const noexcept { return this->value; }

        /**
         * Return the position of the instruction that raised the error (starts at 0)
         */
        SourcePosition GetPosition() const noexcept { return this->position; }

    };

}

#endif //LITESCRIPT_ERROR_HPP
