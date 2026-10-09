#ifndef LITESCRIPT_PLAYGROUND_TOOLS_TRANSPORT_HPP
#define LITESCRIPT_PLAYGROUND_TOOLS_TRANSPORT_HPP

#include <istream>
#include <mutex>
#include <optional>
#include <ostream>
#include <streambuf>

#include "jsonvalue.hpp"

namespace Tools {

    // The messages of the Language Server Protocol and of the Debug Adapter Protocol:
    // a header "Content-Length: <size>" followed by an empty line and the JSON content
    class Transport {

        std::istream in;
        std::ostream out;
        std::mutex write_mutex;

    public:

        /**
         * @param input The buffer of the input (the standard input)
         * @param output The buffer of the output (the standard output)
         */
        Transport(std::streambuf * input, std::streambuf * output) : in(input), out(output) {}

        /**
         * Read the next message (nullopt at the end of the input)
         * An invalid message gives an empty object.
         */
        std::optional<Json> Read();

        /**
         * Write a message (can be called by several threads)
         */
        void Write(const Json& message);

    };

    /**
     * Use the standard input and output in binary mode (no conversion of the line breaks on Windows)
     */
    void SetBinaryStandardStreams();

}

#endif //LITESCRIPT_PLAYGROUND_TOOLS_TRANSPORT_HPP
