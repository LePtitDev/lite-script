#include "jsonvalue.hpp"

#include <charconv>
#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace {

    using Tools::Json;

    void DumpString(std::string& out, std::string_view text) {
        out += '"';
        for (unsigned char c : text) {
            switch (c) {
                case '"': out += "\\\""; break;
                case '\\': out += "\\\\"; break;
                case '\n': out += "\\n"; break;
                case '\r': out += "\\r"; break;
                case '\t': out += "\\t"; break;
                case '\b': out += "\\b"; break;
                case '\f': out += "\\f"; break;
                default:
                    if (c < 0x20) {
                        char buffer[8];
                        std::snprintf(buffer, sizeof(buffer), "\\u%04x", c);
                        out += buffer;
                    }
                    else {
                        out += static_cast<char>(c);
                    }
            }
        }
        out += '"';
    }

    void DumpValue(std::string& out, const Json& value) {
        switch (value.GetType()) {
            case Json::Type::Null:
                out += "null";
                break;
            case Json::Type::Boolean:
                out += value.GetBool() ? "true" : "false";
                break;
            case Json::Type::Number: {
                double n = value.GetNumber();
                if (!std::isfinite(n)) {
                    out += "null";
                }
                else if (n == std::floor(n) && std::fabs(n) < 9e15) {
                    out += std::to_string(static_cast<std::int64_t>(n));
                }
                else {
                    char buffer[32];
                    std::snprintf(buffer, sizeof(buffer), "%.17g", n);
                    out += buffer;
                }
                break;
            }
            case Json::Type::String:
                DumpString(out, value.GetString());
                break;
            case Json::Type::Array:
                out += '[';
                for (std::size_t i = 0; i < value.Size(); i++) {
                    if (i > 0)
                        out += ',';
                    DumpValue(out, value.At(i));
                }
                out += ']';
                break;
            case Json::Type::Object:
                out += '{';
                for (std::size_t i = 0; i < value.Size(); i++) {
                    if (i > 0)
                        out += ',';
                    DumpString(out, value.Keys()[i]);
                    out += ':';
                    DumpValue(out, value.At(i));
                }
                out += '}';
                break;
        }
    }

    void AppendCodePoint(std::string& out, std::uint32_t c) {
        if (c < 0x80) {
            out += static_cast<char>(c);
        }
        else if (c < 0x800) {
            out += static_cast<char>(0xC0 | (c >> 6));
            out += static_cast<char>(0x80 | (c & 0x3F));
        }
        else if (c < 0x10000) {
            out += static_cast<char>(0xE0 | (c >> 12));
            out += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (c & 0x3F));
        }
        else {
            out += static_cast<char>(0xF0 | (c >> 18));
            out += static_cast<char>(0x80 | ((c >> 12) & 0x3F));
            out += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (c & 0x3F));
        }
    }

    class Parser {
        std::string_view text;
        std::size_t pos = 0;
        std::size_t depth = 0;

    public:
        explicit Parser(std::string_view t) : text(t) {}

        bool ParseDocument(Json& out) {
            if (!this->ParseValue(out))
                return false;
            this->SkipSpaces();
            return this->pos == this->text.size();
        }

    private:
        void SkipSpaces() {
            while (this->pos < this->text.size() && (this->text[this->pos] == ' ' || this->text[this->pos] == '\t' ||
                                                     this->text[this->pos] == '\n' || this->text[this->pos] == '\r'))
                this->pos++;
        }

        bool Consume(std::string_view word) {
            if (this->text.substr(this->pos, word.size()) != word)
                return false;
            this->pos += word.size();
            return true;
        }

        bool ParseHex(std::uint32_t& value) {
            if (this->pos + 4 > this->text.size())
                return false;
            auto r = std::from_chars(this->text.data() + this->pos, this->text.data() + this->pos + 4, value, 16);
            if (r.ptr != this->text.data() + this->pos + 4)
                return false;
            this->pos += 4;
            return true;
        }

        bool ParseString(std::string& out) {
            if (this->pos >= this->text.size() || this->text[this->pos] != '"')
                return false;
            this->pos++;
            while (this->pos < this->text.size()) {
                char c = this->text[this->pos++];
                if (c == '"')
                    return true;
                if (c != '\\') {
                    out += c;
                    continue;
                }
                if (this->pos >= this->text.size())
                    return false;
                char e = this->text[this->pos++];
                switch (e) {
                    case '"': out += '"'; break;
                    case '\\': out += '\\'; break;
                    case '/': out += '/'; break;
                    case 'b': out += '\b'; break;
                    case 'f': out += '\f'; break;
                    case 'n': out += '\n'; break;
                    case 'r': out += '\r'; break;
                    case 't': out += '\t'; break;
                    case 'u': {
                        std::uint32_t code;
                        if (!this->ParseHex(code))
                            return false;
                        // Surrogate pair
                        if (code >= 0xD800 && code < 0xDC00 && this->Consume("\\u")) {
                            std::uint32_t low;
                            if (!this->ParseHex(low))
                                return false;
                            code = 0x10000 + ((code - 0xD800) << 10) + (low - 0xDC00);
                        }
                        AppendCodePoint(out, code);
                        break;
                    }
                    default:
                        return false;
                }
            }
            return false;
        }

        bool ParseValue(Json& out) {
            if (++this->depth > 512)
                return false;
            struct Guard { std::size_t& d; ~Guard() { d--; } } guard{ this->depth };
            this->SkipSpaces();
            if (this->pos >= this->text.size())
                return false;
            char c = this->text[this->pos];
            if (c == 'n')
                return this->Consume("null") && (out = Json(), true);
            if (c == 't')
                return this->Consume("true") && (out = Json(true), true);
            if (c == 'f')
                return this->Consume("false") && (out = Json(false), true);
            if (c == '"') {
                std::string s;
                if (!this->ParseString(s))
                    return false;
                out = Json(std::move(s));
                return true;
            }
            if (c == '[') {
                this->pos++;
                out = Json::MakeArray();
                this->SkipSpaces();
                if (this->Consume("]"))
                    return true;
                while (true) {
                    Json item;
                    if (!this->ParseValue(item))
                        return false;
                    out.Push(std::move(item));
                    this->SkipSpaces();
                    if (this->Consume("]"))
                        return true;
                    if (!this->Consume(","))
                        return false;
                }
            }
            if (c == '{') {
                this->pos++;
                out = Json::MakeObject();
                this->SkipSpaces();
                if (this->Consume("}"))
                    return true;
                while (true) {
                    this->SkipSpaces();
                    std::string key;
                    if (!this->ParseString(key))
                        return false;
                    this->SkipSpaces();
                    if (!this->Consume(":"))
                        return false;
                    Json item;
                    if (!this->ParseValue(item))
                        return false;
                    out[key] = std::move(item);
                    this->SkipSpaces();
                    if (this->Consume("}"))
                        return true;
                    if (!this->Consume(","))
                        return false;
                }
            }
            // Number
            std::size_t start = this->pos;
            while (this->pos < this->text.size() && std::string_view("+-0123456789.eE").find(this->text[this->pos]) != std::string_view::npos)
                this->pos++;
            if (start == this->pos)
                return false;
            std::string digits(this->text.substr(start, this->pos - start));
            char * end = nullptr;
            double value = std::strtod(digits.c_str(), &end);
            if (end != digits.c_str() + digits.size())
                return false;
            out = Json(value);
            return true;
        }
    };

}

std::string Tools::Json::Dump() const {
    std::string out;
    DumpValue(out, *this);
    return out;
}

std::optional<Tools::Json> Tools::Json::Parse(std::string_view text) {
    Json res;
    Parser parser(text);
    if (!parser.ParseDocument(res))
        return std::nullopt;
    return res;
}
