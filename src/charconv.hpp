#ifndef LITESCRIPT_CHARCONV_HPP
#define LITESCRIPT_CHARCONV_HPP

#include <charconv>
#include <cstdio>
#include <locale>
#include <sstream>
#include <string>
#include <string_view>

// Conversions between doubles and texts (std::from_chars and std::to_chars for floating numbers
// aren't available in every standard library, the fallbacks are independent of the locale)
namespace LiteScript::Internal {

    /**
     * Read a floating number that covers the whole text (format of std::from_chars)
     */
    inline bool ParseDouble(std::string_view text, double& value) {
        if (text.empty() || text.front() == '+' || text.front() == ' ')
            return false;
#if !defined(LITESCRIPT_NO_FLOAT_CHARCONV) && defined(__cpp_lib_to_chars) && __cpp_lib_to_chars >= 201611L
        auto result = std::from_chars(text.data(), text.data() + text.size(), value);
        return result.ec == std::errc() && result.ptr == text.data() + text.size();
#else
        std::istringstream stream{ std::string(text) };
        stream.imbue(std::locale::classic());
        stream >> value;
        return !stream.fail() && stream.peek() == std::char_traits<char>::eof();
#endif
    }

    /**
     * Return the shortest text that gives the same double when it's read
     */
    inline std::string FormatDouble(double value) {
#if !defined(LITESCRIPT_NO_FLOAT_CHARCONV) && defined(__cpp_lib_to_chars) && __cpp_lib_to_chars >= 201611L
        char buffer[64];
        auto result = std::to_chars(buffer, buffer + sizeof(buffer), value);
        return std::string(buffer, result.ptr);
#else
        for (int precision = 1; precision <= 17; precision++) {
            std::ostringstream stream;
            stream.imbue(std::locale::classic());
            stream.precision(precision);
            stream << value;
            double read;
            if (ParseDouble(stream.str(), read) && read == value)
                return stream.str();
        }
        std::ostringstream stream;
        stream.imbue(std::locale::classic());
        stream.precision(17);
        stream << value;
        return stream.str();
#endif
    }

}

#endif //LITESCRIPT_CHARCONV_HPP
