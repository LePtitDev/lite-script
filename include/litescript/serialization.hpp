#ifndef LITESCRIPT_SERIALIZATION_HPP
#define LITESCRIPT_SERIALIZATION_HPP

#include <iosfwd>
#include <stdexcept>
#include <vector>

#include "instruction.hpp"

namespace LiteScript {

    // Error raised when a binary stream can't be written or read
    class SerializationError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    /**
     * Write instructions in a binary stream
     */
    void SaveInstructions(std::ostream& stream, const std::vector<Instruction>& instructions);

    /**
     * Read instructions from a binary stream
     *
     * @throw SerializationError if the stream is invalid
     */
    std::vector<Instruction> LoadInstructions(std::istream& stream);

}

#endif //LITESCRIPT_SERIALIZATION_HPP
