#ifndef LITESCRIPT_STRING_HPP
#define LITESCRIPT_STRING_HPP

#include <compare>
#include <cstddef>
#include <string>
#include <string_view>

namespace LiteScript {

    // Content of a string, stored as unicode code points
    class String {

        // The code points of the string
        std::u32string str;

    public:

        String() = default;

        /**
         * Constructor from an UTF-8 text
         */
        String(std::string_view utf8) : str(String::ConvertToUnicode(utf8)) {}

        /**
         * Constructor from an UTF-8 text
         */
        String(const char * utf8) : String(std::string_view(utf8)) {}

        /**
         * Constructor from code points
         */
        String(std::u32string data) : str(std::move(data)) {}

        /**
         * Constructor from a single code point
         */
        explicit String(char32_t c) : str(1, c) {}

        /**
         * Return the number of code points
         */
        std::size_t GetLength() const noexcept { return this->str.size(); }

        /**
         * Return the code points
         */
        std::u32string& GetData() noexcept { return this->str; }
        const std::u32string& GetData() const noexcept { return this->str; }

        /**
         * Return the content encoded in UTF-8
         */
        std::string ToUTF8() const { return String::ConvertToUTF8(this->str); }

        /**
         * Replace a part of the string
         *
         * @param index The index of the first code point to replace
         * @param length The number of code points to replace
         * @param s The replacement
         */
        void Replace(std::size_t index, std::size_t length, const String& s);

        /**
         * Return the string repeated several times (an empty string if count <= 0)
         */
        String Repeat(long long count) const;

        static std::string ConvertToUTF8(std::u32string_view str);
        static std::u32string ConvertToUnicode(std::string_view str);

        String operator+(const String& s) const { return String(this->str + s.str); }
        String& operator+=(const String& s) { this->str += s.str; return *this; }
        char32_t& operator[](std::size_t index) { return this->str[index]; }
        const char32_t& operator[](std::size_t index) const { return this->str[index]; }

        bool operator==(const String& s) const noexcept = default;
        std::strong_ordering operator<=>(const String& s) const noexcept { return this->str.compare(s.str) <=> 0; }

    };

}

#endif //LITESCRIPT_STRING_HPP
