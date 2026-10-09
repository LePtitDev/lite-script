#ifndef LITESCRIPT_BINARY_STREAM_HPP
#define LITESCRIPT_BINARY_STREAM_HPP

#include <algorithm>
#include <bit>
#include <cstdint>
#include <istream>
#include <ostream>
#include <string>
#include <string_view>

#include <litescript/serialization.hpp>

namespace LiteScript::Internal {

    // Write values in little endian
    class BinaryWriter {

        std::ostream& stream;

    public:

        explicit BinaryWriter(std::ostream& s) : stream(s) {}

        void U8(std::uint8_t v) {
            this->stream.put(static_cast<char>(v));
        }

        void U32(std::uint32_t v) {
            for (int i = 0; i < 4; i++)
                this->U8(static_cast<std::uint8_t>(v >> (8 * i)));
        }

        void U64(std::uint64_t v) {
            for (int i = 0; i < 8; i++)
                this->U8(static_cast<std::uint8_t>(v >> (8 * i)));
        }

        void I64(std::int64_t v) {
            this->U64(static_cast<std::uint64_t>(v));
        }

        void F64(double v) {
            this->U64(std::bit_cast<std::uint64_t>(v));
        }

        void Size(std::size_t v) {
            if (v > UINT32_MAX)
                throw SerializationError("LiteScript: too many elements to save");
            this->U32(static_cast<std::uint32_t>(v));
        }

        void Text(std::string_view s) {
            this->Size(s.size());
            this->stream.write(s.data(), static_cast<std::streamsize>(s.size()));
        }

        void Check() {
            if (!this->stream)
                throw SerializationError("LiteScript: can't write in the stream");
        }

    };

    // Read values in little endian
    class BinaryReader {

        std::istream& stream;

    public:

        explicit BinaryReader(std::istream& s) : stream(s) {}

        [[noreturn]] static void Fail(const char * message) {
            throw SerializationError(std::string("LiteScript: invalid stream (") + message + ")");
        }

        std::uint8_t U8() {
            int c = this->stream.get();
            if (c == std::char_traits<char>::eof())
                Fail("unexpected end");
            return static_cast<std::uint8_t>(c);
        }

        std::uint32_t U32() {
            std::uint32_t v = 0;
            for (int i = 0; i < 4; i++)
                v |= static_cast<std::uint32_t>(this->U8()) << (8 * i);
            return v;
        }

        std::uint64_t U64() {
            std::uint64_t v = 0;
            for (int i = 0; i < 8; i++)
                v |= static_cast<std::uint64_t>(this->U8()) << (8 * i);
            return v;
        }

        std::int64_t I64() {
            return static_cast<std::int64_t>(this->U64());
        }

        double F64() {
            return std::bit_cast<double>(this->U64());
        }

        bool Bool() {
            std::uint8_t v = this->U8();
            if (v > 1)
                Fail("invalid boolean");
            return v == 1;
        }

        // Read a number of elements, each element needs at least one byte in the stream
        std::size_t Size() {
            return this->U32();
        }

        std::string Text() {
            std::size_t size = this->Size();
            std::string s;
            // Read by chunks to avoid allocating a huge string for an invalid size
            constexpr std::size_t CHUNK = 4096;
            while (s.size() < size) {
                std::size_t n = std::min(CHUNK, size - s.size());
                std::size_t offset = s.size();
                s.resize(offset + n);
                this->stream.read(s.data() + offset, static_cast<std::streamsize>(n));
                if (static_cast<std::size_t>(this->stream.gcount()) != n)
                    Fail("unexpected end");
            }
            return s;
        }

    };

}

#endif //LITESCRIPT_BINARY_STREAM_HPP
