#include <litescript/string.hpp>

#include <stdexcept>

namespace {

    // Code point used for invalid UTF-8 sequences
    constexpr char32_t REPLACEMENT_CHARACTER = 0xFFFD;

    // Maximum length of a string created by repetition
    constexpr std::size_t MAX_REPEAT_LENGTH = std::size_t(1) << 26;

}

void LiteScript::String::Replace(std::size_t index, std::size_t length, const String& s) {
    if (index > this->str.size())
        index = this->str.size();
    this->str.replace(index, length, s.str);
}

LiteScript::String LiteScript::String::Repeat(long long count) const {
    if (count <= 0 || this->str.empty())
        return String();
    if (static_cast<unsigned long long>(count) > MAX_REPEAT_LENGTH / this->str.size())
        throw std::length_error("LiteScript::String::Repeat: string too long");
    std::u32string result;
    result.reserve(this->str.size() * static_cast<std::size_t>(count));
    for (long long i = 0; i < count; i++)
        result += this->str;
    return String(std::move(result));
}

std::string LiteScript::String::ConvertToUTF8(std::u32string_view str) {
    std::string res;
    res.reserve(str.size());
    for (char32_t c : str) {
        if (c > 0x10FFFF || (c >= 0xD800 && c <= 0xDFFF))
            c = REPLACEMENT_CHARACTER;
        if (c < 0x80) {
            res.push_back(static_cast<char>(c));
        }
        else if (c < 0x800) {
            res.push_back(static_cast<char>(0xC0 | (c >> 6)));
            res.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        }
        else if (c < 0x10000) {
            res.push_back(static_cast<char>(0xE0 | (c >> 12)));
            res.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
            res.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        }
        else {
            res.push_back(static_cast<char>(0xF0 | (c >> 18)));
            res.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
            res.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
            res.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        }
    }
    return res;
}

std::u32string LiteScript::String::ConvertToUnicode(std::string_view str) {
    std::u32string res;
    res.reserve(str.size());
    std::size_t i = 0, sz = str.size();
    while (i < sz) {
        auto byte = static_cast<unsigned char>(str[i]);
        std::size_t length;
        char32_t c;
        if (byte < 0x80) {
            length = 1;
            c = byte;
        }
        else if ((byte & 0xE0) == 0xC0) {
            length = 2;
            c = byte & 0x1F;
        }
        else if ((byte & 0xF0) == 0xE0) {
            length = 3;
            c = byte & 0x0F;
        }
        else if ((byte & 0xF8) == 0xF0) {
            length = 4;
            c = byte & 0x07;
        }
        else {
            res.push_back(REPLACEMENT_CHARACTER);
            i++;
            continue;
        }
        if (i + length > sz) {
            res.push_back(REPLACEMENT_CHARACTER);
            break;
        }
        bool valid = true;
        for (std::size_t j = 1; j < length; j++) {
            auto next = static_cast<unsigned char>(str[i + j]);
            if ((next & 0xC0) != 0x80) {
                valid = false;
                break;
            }
            c = (c << 6) | (next & 0x3F);
        }
        if (!valid) {
            res.push_back(REPLACEMENT_CHARACTER);
            i++;
            continue;
        }
        res.push_back(c);
        i += length;
    }
    return res;
}
