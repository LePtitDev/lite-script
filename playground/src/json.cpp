#include "helpers.hpp"
#include "modules.hpp"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <optional>
#include <unordered_set>

// Locale independent conversions of the engine
#include "charconv.hpp"

namespace {

    using namespace Playground;

    // Maximum nesting of the arrays and objects (protects the native stack)
    constexpr std::size_t MAX_DEPTH = 512;

    ////// PARSING //////

    // Convert a JSON text into script values: objects become arrays with named elements,
    // arrays become arrays with indexed elements
    class Parser {

        Memory& memory;
        std::string_view text;
        std::size_t pos = 0;
        std::size_t depth = 0;

    public:

        Parser(Memory& mem, std::string_view json) : memory(mem), text(json) {}

        Variable Parse() {
            // UTF-8 byte order mark
            if (this->text.starts_with("\xEF\xBB\xBF"))
                this->pos = 3;
            Variable value = this->Value();
            this->SkipSpaces();
            if (this->pos < this->text.size())
                this->Fail("unexpected character after the value");
            return value;
        }

    private:

        [[noreturn]] void Fail(const std::string& message) const {
            // Position as line and column (starting at 1)
            std::size_t line = 1, column = 1;
            for (std::size_t i = 0; i < this->pos && i < this->text.size(); i++) {
                if (this->text[i] == '\n') {
                    line++;
                    column = 1;
                }
                else {
                    column++;
                }
            }
            throw std::runtime_error("invalid JSON: " + message + " (line " + std::to_string(line) + ", column " + std::to_string(column) + ")");
        }

        void SkipSpaces() {
            while (this->pos < this->text.size()) {
                char c = this->text[this->pos];
                if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
                    break;
                this->pos++;
            }
        }

        bool Match(std::string_view word) {
            if (this->text.substr(this->pos).starts_with(word)) {
                this->pos += word.size();
                return true;
            }
            return false;
        }

        void Expect(char c, const char * message) {
            this->SkipSpaces();
            if (this->pos >= this->text.size() || this->text[this->pos] != c)
                this->Fail(message);
            this->pos++;
        }

        Variable Value() {
            this->SkipSpaces();
            if (this->pos >= this->text.size())
                this->Fail("value expected");
            char c = this->text[this->pos];
            if (c == '{')
                return this->Object();
            if (c == '[')
                return this->List();
            if (c == '"')
                return this->memory.Create(LiteScript::String(this->StringValue()));
            if (c == '-' || (c >= '0' && c <= '9'))
                return this->NumberValue();
            if (this->Match("true"))
                return this->memory.CreateBoolean(true);
            if (this->Match("false"))
                return this->memory.CreateBoolean(false);
            if (this->Match("null"))
                return this->memory.CreateNull();
            this->Fail("value expected");
        }

        void Enter() {
            if (++this->depth > MAX_DEPTH)
                this->Fail("too many nested values");
        }

        Variable Object() {
            this->Enter();
            this->pos++;
            Array array;
            this->SkipSpaces();
            if (this->pos < this->text.size() && this->text[this->pos] == '}') {
                this->pos++;
            }
            else {
                while (true) {
                    this->SkipSpaces();
                    if (this->pos >= this->text.size() || this->text[this->pos] != '"')
                        this->Fail("member name expected");
                    std::string name = LiteScript::String::ConvertToUTF8(this->StringValue());
                    this->Expect(':', "colon \":\" expected after the member name");
                    array.named.Set(name, this->Value());
                    this->SkipSpaces();
                    if (this->pos < this->text.size() && this->text[this->pos] == ',') {
                        this->pos++;
                        continue;
                    }
                    this->Expect('}', "comma \",\" or closing brace \"}\" expected");
                    break;
                }
            }
            this->depth--;
            return this->memory.Create(std::move(array));
        }

        Variable List() {
            this->Enter();
            this->pos++;
            Array array;
            this->SkipSpaces();
            if (this->pos < this->text.size() && this->text[this->pos] == ']') {
                this->pos++;
            }
            else {
                while (true) {
                    array.indexed.push_back(this->Value());
                    this->SkipSpaces();
                    if (this->pos < this->text.size() && this->text[this->pos] == ',') {
                        this->pos++;
                        continue;
                    }
                    this->Expect(']', "comma \",\" or closing bracket \"]\" expected");
                    break;
                }
            }
            this->depth--;
            return this->memory.Create(std::move(array));
        }

        Variable NumberValue() {
            std::size_t start = this->pos;
            auto digits = [this]() {
                std::size_t first = this->pos;
                while (this->pos < this->text.size() && this->text[this->pos] >= '0' && this->text[this->pos] <= '9')
                    this->pos++;
                return this->pos - first;
            };
            if (this->text[this->pos] == '-')
                this->pos++;
            std::size_t first = this->pos;
            std::size_t count = digits();
            if (count == 0 || (count > 1 && this->text[first] == '0'))
                this->Fail("invalid number");
            bool integer = true;
            if (this->pos < this->text.size() && this->text[this->pos] == '.') {
                this->pos++;
                integer = false;
                if (digits() == 0)
                    this->Fail("invalid number");
            }
            if (this->pos < this->text.size() && (this->text[this->pos] == 'e' || this->text[this->pos] == 'E')) {
                this->pos++;
                integer = false;
                if (this->pos < this->text.size() && (this->text[this->pos] == '+' || this->text[this->pos] == '-'))
                    this->pos++;
                if (digits() == 0)
                    this->Fail("invalid number");
            }
            std::string_view literal = this->text.substr(start, this->pos - start);
            if (integer) {
                std::int64_t value;
                auto r = std::from_chars(literal.data(), literal.data() + literal.size(), value);
                if (r.ec == std::errc())
                    return this->memory.CreateNumber(Number(value));
            }
            double value = 0;
            if (!LiteScript::Internal::ParseDouble(literal, value))
                this->Fail("invalid number");
            return this->memory.CreateNumber(Number(value));
        }

        unsigned Hex4() {
            if (this->pos + 4 > this->text.size())
                this->Fail("invalid unicode escape");
            unsigned value = 0;
            auto r = std::from_chars(this->text.data() + this->pos, this->text.data() + this->pos + 4, value, 16);
            if (r.ec != std::errc() || r.ptr != this->text.data() + this->pos + 4)
                this->Fail("invalid unicode escape");
            this->pos += 4;
            return value;
        }

        // Read a string (the position is on the opening quote)
        std::u32string StringValue() {
            this->pos++;
            std::string utf8;
            std::u32string result;
            // The UTF-8 parts are converted at the end, the escapes are added as code points
            auto flush = [&]() {
                result += LiteScript::String::ConvertToUnicode(utf8);
                utf8.clear();
            };
            while (true) {
                if (this->pos >= this->text.size())
                    this->Fail("unterminated string");
                char c = this->text[this->pos++];
                if (c == '"')
                    break;
                if (static_cast<unsigned char>(c) < 0x20) {
                    this->pos--;
                    this->Fail("control character in a string");
                }
                if (c != '\\') {
                    utf8 += c;
                    continue;
                }
                if (this->pos >= this->text.size())
                    this->Fail("unterminated string");
                char e = this->text[this->pos++];
                switch (e) {
                    case '"': utf8 += '"'; break;
                    case '\\': utf8 += '\\'; break;
                    case '/': utf8 += '/'; break;
                    case 'b': utf8 += '\b'; break;
                    case 'f': utf8 += '\f'; break;
                    case 'n': utf8 += '\n'; break;
                    case 'r': utf8 += '\r'; break;
                    case 't': utf8 += '\t'; break;
                    case 'u': {
                        char32_t code = this->Hex4();
                        // Surrogate pair
                        if (code >= 0xD800 && code <= 0xDBFF && this->text.substr(this->pos).starts_with("\\u")) {
                            std::size_t save = this->pos;
                            this->pos += 2;
                            char32_t low = this->Hex4();
                            if (low >= 0xDC00 && low <= 0xDFFF)
                                code = 0x10000 + ((code - 0xD800) << 10) + (low - 0xDC00);
                            else
                                this->pos = save;
                        }
                        // Lone surrogates are replaced
                        if (code >= 0xD800 && code <= 0xDFFF)
                            code = 0xFFFD;
                        flush();
                        result += code;
                        break;
                    }
                    default:
                        this->pos--;
                        this->Fail("invalid escape sequence");
                }
            }
            flush();
            return result;
        }

    };

    ////// SERIALIZATION //////

    class Writer {

        // The text of an indentation level (empty for a compact output)
        std::string indent;

        // The arrays, instances and namespaces being written (detects the cycles)
        std::unordered_set<const LiteScript::Object *> visiting;

        std::string out;

    public:

        explicit Writer(std::string indentation) : indent(std::move(indentation)) {}

        // Return the JSON text (nullopt if the value can't be written)
        std::optional<std::string> Write(const Variable& value) {
            if (!this->Value(value, 0))
                return std::nullopt;
            return std::move(this->out);
        }

    private:

        void NewLine(std::size_t level) {
            if (this->indent.empty())
                return;
            this->out += '\n';
            for (std::size_t i = 0; i < level; i++)
                this->out += this->indent;
        }

        void Quote(const std::u32string& text) {
            static constexpr char HEX[] = "0123456789abcdef";
            this->out += '"';
            std::u32string plain;
            auto flush = [&]() {
                this->out += LiteScript::String::ConvertToUTF8(plain);
                plain.clear();
            };
            for (char32_t c : text) {
                const char * escape = nullptr;
                switch (c) {
                    case U'"': escape = "\\\""; break;
                    case U'\\': escape = "\\\\"; break;
                    case U'\b': escape = "\\b"; break;
                    case U'\f': escape = "\\f"; break;
                    case U'\n': escape = "\\n"; break;
                    case U'\r': escape = "\\r"; break;
                    case U'\t': escape = "\\t"; break;
                    default: break;
                }
                if (escape != nullptr) {
                    flush();
                    this->out += escape;
                }
                else if (c < 0x20) {
                    flush();
                    this->out += "\\u00";
                    this->out += HEX[c >> 4];
                    this->out += HEX[c & 0xF];
                }
                else {
                    plain += c;
                }
            }
            flush();
            this->out += '"';
        }

        // Indicate if a value is written (functions, classes and undefined values are ignored)
        static bool Writable(const Variable& v) {
            switch (v.GetType()) {
                case Type::Undefined:
                case Type::Callback:
                case Type::Class:
                    return false;
                default:
                    return true;
            }
        }

        void Members(const std::vector<std::pair<std::string, Variable>>& members, std::size_t level) {
            this->out += '{';
            bool first = true;
            for (const auto& [name, value] : members) {
                if (!Writable(value))
                    continue;
                if (!first)
                    this->out += ',';
                first = false;
                this->NewLine(level + 1);
                this->Quote(LiteScript::String(name).GetData());
                this->out += this->indent.empty() ? ":" : ": ";
                this->Value(value, level + 1);
            }
            if (!first)
                this->NewLine(level);
            this->out += '}';
        }

        static std::vector<std::pair<std::string, Variable>> Collect(const MemberList& list) {
            std::vector<std::pair<std::string, Variable>> members;
            for (const auto& member : list)
                members.push_back(member);
            return members;
        }

        // Write a value (return false if the value isn't writable)
        bool Value(const Variable& v, std::size_t level) {
            if (!Writable(v))
                return false;
            switch (v.GetType()) {
                case Type::Null:
                    this->out += "null";
                    return true;
                case Type::Boolean:
                    this->out += v.As<bool>() ? "true" : "false";
                    return true;
                case Type::Number: {
                    const Number& n = v.As<Number>();
                    if (!n.IsInteger() && !std::isfinite(n.ToFloat()))
                        this->out += "null";
                    else
                        this->out += n.ToString();
                    return true;
                }
                case Type::String:
                case Type::Character:
                    this->Quote(ToText(v).GetData());
                    return true;
                default:
                    break;
            }
            const LiteScript::Object * target = &v->Target();
            if (!this->visiting.insert(target).second)
                throw std::runtime_error("JSON.stringify: the value contains a cycle");
            if (level >= MAX_DEPTH)
                throw std::runtime_error("JSON.stringify: too many nested values");
            if (v.Is<Array>()) {
                const Array& array = v.As<Array>();
                if (array.named.Empty()) {
                    // A list
                    this->out += '[';
                    for (std::size_t i = 0; i < array.indexed.size(); i++) {
                        if (i > 0)
                            this->out += ',';
                        this->NewLine(level + 1);
                        if (!this->Value(array.indexed[i], level + 1))
                            this->out += "null";
                    }
                    if (!array.indexed.empty())
                        this->NewLine(level);
                    this->out += ']';
                }
                else {
                    // An object, the indexed elements are written with their index as name
                    std::vector<std::pair<std::string, Variable>> members;
                    for (std::size_t i = 0; i < array.indexed.size(); i++)
                        members.emplace_back(std::to_string(i), array.indexed[i]);
                    for (const auto& member : array.named)
                        members.push_back(member);
                    this->Members(members, level);
                }
            }
            else if (v.Is<ClassObject>()) {
                this->Members(Collect(v.As<ClassObject>().members), level);
            }
            else {
                this->Members(Collect(v.As<Namespace>().variables), level);
            }
            this->visiting.erase(target);
            return true;
        }

    };

}

std::optional<std::string> Playground::Stringify(const Variable& value, std::string indent) {
    return Writer(std::move(indent)).Write(value);
}

void Playground::InstallJson(State& state) {
    Variable json = state.GetMemory().CreateNamespace();
    Define(state, json, "JSON.", "parse", [](State& s, Args& args) {
        return Parser(s.GetMemory(), TextArg(args, 0, "text")).Parse();
    });
    // stringify(value, indent) where indent is a number of spaces or a string (compact by default)
    Define(state, json, "JSON.", "stringify", [](State& s, Args& args) {
        std::string indent;
        if (args.size() > 1 && args[1].Is<Number>())
            indent.assign(static_cast<std::size_t>(std::clamp<std::int64_t>(args[1].As<Number>().ToInteger(), 0, 10)), ' ');
        else if (!IsMissing(args, 1))
            indent = ToText(args[1]).ToUTF8();
        auto text = Stringify(Arg(s, args, 0), std::move(indent));
        return text ? s.GetMemory().CreateString(*text) : s.GetMemory().CreateUndefined();
    });
    state.DeclareVariable("JSON", json);
}
