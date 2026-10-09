#include <litescript/script.hpp>

#include <limits>

#include "charconv.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <optional>
#include <string>

namespace {

    using LiteScript::InstrCode;
    using LiteScript::Instruction;
    using Error = LiteScript::Script::Error;

    // Error raised during the compilation
    struct CompileError {
        Error error;
        std::size_t position;
    };

    // Keywords that can't be used as variable names
    constexpr std::array<std::string_view, 26> RESERVED_WORDS{ {
        "var", "function", "class", "if", "else", "for", "while", "do", "switch", "case", "default",
        "break", "continue", "return", "new", "this", "true", "false", "null", "undefined",
        "namespace", "static", "operator", "try", "catch", "throw"
    } };

    // Names of the overloadable operators (in the order of LiteScript::Operator)
    constexpr std::array<std::string_view, LiteScript::OPERATOR_COUNT> OPERATOR_NAMES{ {
        "assign", "unary_plus", "unary_minus", "pre_increment", "post_increment", "pre_decrement",
        "post_decrement", "add", "subtract", "multiply", "divide", "modulo", "equal", "not_equal",
        "greater", "lesser", "greater_equal", "lesser_equal", "not", "and", "or", "bitwise_not",
        "bitwise_and", "bitwise_or", "bitwise_xor", "left_shift", "right_shift", "add_assign",
        "subtract_assign", "multiply_assign", "divide_assign", "get", "call"
    } };

    // Operators of expressions waiting for their operands
    enum class Op {
        // Prefix operators
        PreIncrement, PreDecrement, UnaryPlus, UnaryMinus, Not, BitwiseNot, New,
        // Binary operators
        Multiply, Divide, Modulo,
        Add, Subtract,
        LeftShift, RightShift,
        Less, LessOrEqual, Greater, GreaterOrEqual,
        Equal, NotEqual,
        BitwiseAnd,
        BitwiseXor,
        BitwiseOr,
        And,
        Or,
        Assign, AddAssign, SubtractAssign, MultiplyAssign, DivideAssign
    };

    // Maximum nesting depth of instructions and expressions (the compiler is recursive,
    // the limit keeps the stack usage low even in sanitized builds and small thread stacks)
    constexpr std::size_t MAX_DEPTH = 100;

    // Priority of the assignment operators (the lowest one)
    constexpr int ASSIGN_PRIORITY = 13;

    // Return the priority of an operator (a lower value binds tighter)
    int Priority(Op op) {
        switch (op) {
            case Op::Multiply: case Op::Divide: case Op::Modulo:
                return 3;
            case Op::Add: case Op::Subtract:
                return 4;
            case Op::LeftShift: case Op::RightShift:
                return 5;
            case Op::Less: case Op::LessOrEqual: case Op::Greater: case Op::GreaterOrEqual:
                return 6;
            case Op::Equal: case Op::NotEqual:
                return 7;
            case Op::BitwiseAnd:
                return 8;
            case Op::BitwiseXor:
                return 9;
            case Op::BitwiseOr:
                return 10;
            case Op::And:
                return 11;
            case Op::Or:
                return 12;
            case Op::Assign: case Op::AddAssign: case Op::SubtractAssign: case Op::MultiplyAssign: case Op::DivideAssign:
                return ASSIGN_PRIORITY;
            default:
                return 2;
        }
    }

    bool IsDigit(char c) {
        return c >= '0' && c <= '9';
    }

    bool IsNameStart(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '$' || c == '_';
    }

    bool IsNameChar(char c) {
        return IsNameStart(c) || IsDigit(c);
    }

    bool IsReserved(std::string_view name) {
        for (std::string_view word : RESERVED_WORDS) {
            if (word == name)
                return true;
        }
        return false;
    }

    // Append a code point to an UTF-8 string
    void AppendCodePoint(std::string& out, char32_t c) {
        out += LiteScript::String::ConvertToUTF8(std::u32string_view(&c, 1));
    }

    // A loop or a switch that can be left with break (and continue for loops)
    // A block that break and continue can leave: a loop, a switch or a try block
    struct Breakable {
        enum class Kind { Loop, Switch, Try };
        Kind kind = Kind::Loop;
        std::vector<std::size_t> breaks;
        std::vector<std::size_t> continues;
    };

    // Recursive descent compiler of the script language
    class Compiler {

        // The source code
        std::string_view text;

        // The current position in the code
        std::size_t pos = 0;

        // The generated instructions
        std::vector<Instruction>& out;

        // The loops and switches containing the current instruction
        std::vector<Breakable> breakables;

        // The current nesting depth of instructions and expressions
        std::size_t depth = 0;

        // Limit the recursion of the compiler
        class DepthGuard {
            Compiler& compiler;
        public:
            explicit DepthGuard(Compiler& c) : compiler(c) {
                if (++this->compiler.depth > MAX_DEPTH)
                    this->compiler.Fail(Error::Nesting);
            }
            ~DepthGuard() { --this->compiler.depth; }
            DepthGuard(const DepthGuard&) = delete;
            DepthGuard& operator=(const DepthGuard&) = delete;
        };

    public:

        Compiler(std::string_view code, std::vector<Instruction>& instructions) : text(code), out(instructions) {}

        void CompileScript() {
            while (true) {
                this->SkipWhitespace();
                if (this->AtEnd())
                    break;
                this->ParseInstruction();
            }
        }

    private:

        ////// LEXICAL ANALYSIS //////

        [[noreturn]] void Fail(Error error) const {
            throw CompileError{ error, this->pos };
        }

        bool AtEnd() const {
            return this->pos >= this->text.size();
        }

        char Peek(std::size_t offset = 0) const {
            return this->pos + offset < this->text.size() ? this->text[this->pos + offset] : '\0';
        }

        void SkipWhitespace() {
            while (!this->AtEnd()) {
                char c = this->Peek();
                if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v') {
                    this->pos++;
                }
                else if (c == '/' && this->Peek(1) == '/') {
                    while (!this->AtEnd() && this->Peek() != '\n')
                        this->pos++;
                }
                else if (c == '/' && this->Peek(1) == '*') {
                    std::size_t end = this->text.find("*/", this->pos + 2);
                    this->pos = end == std::string_view::npos ? this->text.size() : end + 2;
                }
                else {
                    break;
                }
            }
        }

        // Consume a character after the whitespaces
        bool Match(char c) {
            this->SkipWhitespace();
            if (this->Peek() != c)
                return false;
            this->pos++;
            return true;
        }

        void Expect(char c, Error error) {
            if (!this->Match(c))
                this->Fail(error);
        }

        // Return the name at the current position without consuming it
        std::string_view PeekName() const {
            if (!IsNameStart(this->Peek()))
                return std::string_view();
            std::size_t end = this->pos + 1;
            while (end < this->text.size() && IsNameChar(this->text[end]))
                end++;
            return this->text.substr(this->pos, end - this->pos);
        }

        // Consume a keyword after the whitespaces
        bool MatchKeyword(std::string_view keyword) {
            this->SkipWhitespace();
            if (this->PeekName() != keyword)
                return false;
            this->pos += keyword.size();
            return true;
        }

        // Indicate if the keyword at the current position is followed by a name
        bool KeywordFollowedByName(std::string_view keyword) {
            this->SkipWhitespace();
            if (this->PeekName() != keyword)
                return false;
            std::size_t saved = this->pos;
            this->pos += keyword.size();
            this->SkipWhitespace();
            bool result = IsNameStart(this->Peek());
            this->pos = saved;
            return result;
        }

        // Consume a name (any identifier)
        std::string ReadName(Error error) {
            this->SkipWhitespace();
            std::string_view name = this->PeekName();
            if (name.empty())
                this->Fail(error);
            this->pos += name.size();
            return std::string(name);
        }

        // Consume a variable name (not a reserved word)
        std::string ReadVariableName(Error error) {
            this->SkipWhitespace();
            std::string_view name = this->PeekName();
            if (name.empty() || IsReserved(name))
                this->Fail(error);
            this->pos += name.size();
            return std::string(name);
        }

        std::string ReadString() {
            std::size_t start = this->pos;
            char quote = this->text[this->pos++];
            std::string res;
            while (true) {
                if (this->AtEnd()) {
                    this->pos = start;
                    this->Fail(Error::UnterminatedString);
                }
                char c = this->text[this->pos++];
                if (c == quote)
                    return res;
                if (c != '\\') {
                    res += c;
                    continue;
                }
                if (this->AtEnd())
                    continue;
                char e = this->text[this->pos++];
                switch (e) {
                    case 'n': res += '\n'; break;
                    case 't': res += '\t'; break;
                    case 'r': res += '\r'; break;
                    case 'b': res += '\b'; break;
                    case 'f': res += '\f'; break;
                    case 'v': res += '\v'; break;
                    case '0': res += '\0'; break;
                    case 'u': {
                        // \uXXXX or \u{X...}
                        bool braces = this->Peek() == '{';
                        std::size_t first = this->pos + (braces ? 1 : 0);
                        std::size_t last = braces ? this->text.find('}', first) : first + 4;
                        std::uint32_t code = 0;
                        if (last != std::string_view::npos && last <= this->text.size() && last > first) {
                            auto r = std::from_chars(this->text.data() + first, this->text.data() + last, code, 16);
                            if (r.ec == std::errc() && r.ptr == this->text.data() + last) {
                                AppendCodePoint(res, static_cast<char32_t>(code));
                                this->pos = last + (braces ? 1 : 0);
                                break;
                            }
                        }
                        res += 'u';
                        break;
                    }
                    default: res += e; break;
                }
            }
        }

        ////// CODE GENERATION //////

        // Add an instruction generated by the code at an offset (the current position by default),
        // the offset is converted into a line and a column at the end of the compilation
        std::size_t Emit(Instruction instr, std::size_t at = std::string_view::npos) {
            instr.position.line = static_cast<std::uint32_t>(at == std::string_view::npos ? this->pos : at);
            this->out.push_back(std::move(instr));
            return this->out.size() - 1;
        }

        std::size_t Here() const {
            return this->out.size();
        }

        // Define the target of a jump
        void Patch(std::size_t index, std::size_t target) {
            this->out[index].operand = static_cast<std::int64_t>(target);
        }

        void EmitOperator(Op op) {
            switch (op) {
                case Op::PreIncrement: this->Emit(InstrCode::OpPreIncrement); break;
                case Op::PreDecrement: this->Emit(InstrCode::OpPreDecrement); break;
                case Op::UnaryPlus: this->Emit(InstrCode::OpUnaryPlus); break;
                case Op::UnaryMinus: this->Emit(InstrCode::OpUnaryMinus); break;
                case Op::Not: this->Emit(InstrCode::OpLogicalNot); break;
                case Op::BitwiseNot: this->Emit(InstrCode::OpBitwiseNot); break;
                case Op::New:
                    // "new A" without arguments
                    this->Emit(InstrCode::PushArgs);
                    this->Emit(InstrCode::ValueObject);
                    break;
                case Op::Multiply: this->Emit(InstrCode::OpMultiply); break;
                case Op::Divide: this->Emit(InstrCode::OpDivide); break;
                case Op::Modulo: this->Emit(InstrCode::OpModulo); break;
                case Op::Add: this->Emit(InstrCode::OpAdd); break;
                case Op::Subtract: this->Emit(InstrCode::OpSubtract); break;
                case Op::LeftShift: this->Emit(InstrCode::OpLeftShift); break;
                case Op::RightShift: this->Emit(InstrCode::OpRightShift); break;
                case Op::Less: this->Emit(InstrCode::OpLess); break;
                case Op::LessOrEqual: this->Emit(InstrCode::OpLessOrEqual); break;
                case Op::Greater: this->Emit(InstrCode::OpGreater); break;
                case Op::GreaterOrEqual: this->Emit(InstrCode::OpGreaterOrEqual); break;
                case Op::Equal: this->Emit(InstrCode::OpEqual); break;
                case Op::NotEqual: this->Emit(InstrCode::OpNotEqual); break;
                case Op::BitwiseAnd: this->Emit(InstrCode::OpBitwiseAnd); break;
                case Op::BitwiseXor: this->Emit(InstrCode::OpBitwiseXor); break;
                case Op::BitwiseOr: this->Emit(InstrCode::OpBitwiseOr); break;
                case Op::And: this->Emit(InstrCode::OpLogicalAnd); break;
                case Op::Or: this->Emit(InstrCode::OpLogicalOr); break;
                case Op::Assign: this->Emit(InstrCode::OpAssign); break;
                case Op::AddAssign: this->Emit(InstrCode::OpAddAssign); break;
                case Op::SubtractAssign: this->Emit(InstrCode::OpSubtractAssign); break;
                case Op::MultiplyAssign: this->Emit(InstrCode::OpMultiplyAssign); break;
                case Op::DivideAssign: this->Emit(InstrCode::OpDivideAssign); break;
            }
        }

        ////// EXPRESSIONS //////

        // Push a binary operator, the operators with a higher priority are applied before
        void PushBinary(std::vector<Op>& ops, Op op) {
            bool right_associative = Priority(op) == ASSIGN_PRIORITY;
            while (!ops.empty()) {
                int top = Priority(ops.back()), current = Priority(op);
                if (top < current || (top == current && !right_associative)) {
                    this->EmitOperator(ops.back());
                    ops.pop_back();
                }
                else {
                    break;
                }
            }
            ops.push_back(op);
        }

        // Read a binary operator
        std::optional<Op> ReadBinaryOperator() {
            char c = this->Peek(), n = this->Peek(1);
            auto take = [this](std::size_t length, Op op) {
                this->pos += length;
                return std::optional<Op>(op);
            };
            switch (c) {
                case '+': return n == '=' ? take(2, Op::AddAssign) : take(1, Op::Add);
                case '-': return n == '=' ? take(2, Op::SubtractAssign) : take(1, Op::Subtract);
                case '*': return n == '=' ? take(2, Op::MultiplyAssign) : take(1, Op::Multiply);
                case '/': return n == '=' ? take(2, Op::DivideAssign) : take(1, Op::Divide);
                case '%': return take(1, Op::Modulo);
                case '=': return n == '=' ? take(2, Op::Equal) : take(1, Op::Assign);
                case '!':
                    if (n == '=')
                        return take(2, Op::NotEqual);
                    this->Fail(Error::Operator);
                case '<':
                    if (n == '<')
                        return take(2, Op::LeftShift);
                    return n == '=' ? take(2, Op::LessOrEqual) : take(1, Op::Less);
                case '>':
                    if (n == '>')
                        return take(2, Op::RightShift);
                    return n == '=' ? take(2, Op::GreaterOrEqual) : take(1, Op::Greater);
                case '&': return n == '&' ? take(2, Op::And) : take(1, Op::BitwiseAnd);
                case '|': return n == '|' ? take(2, Op::Or) : take(1, Op::BitwiseOr);
                case '^': return take(1, Op::BitwiseXor);
                default: return std::nullopt;
            }
        }

        // Parse an expression
        // @return false if there is no expression and it isn't required
        bool ParseExpression(bool required = true, Error missing = Error::Expression) {
            DepthGuard guard(*this);
            std::vector<Op> ops;
            if (!this->ParseOperand(ops)) {
                if (required)
                    this->Fail(missing);
                return false;
            }
            while (true) {
                this->SkipWhitespace();
                char c = this->Peek();
                std::size_t start = this->pos;
                if (c == '(') {
                    this->ParseCall(ops);
                }
                else if (c == '[') {
                    this->pos++;
                    this->ParseExpression();
                    this->Expect(']', Error::BracketClose);
                    this->Emit(InstrCode::OpArray, start);
                }
                else if (c == '.') {
                    this->pos++;
                    this->Emit(Instruction(InstrCode::OpMember, this->ReadName(Error::Name)), start);
                }
                else if (c == '+' && this->Peek(1) == '+') {
                    this->pos += 2;
                    this->Emit(InstrCode::OpPostIncrement);
                }
                else if (c == '-' && this->Peek(1) == '-') {
                    this->pos += 2;
                    this->Emit(InstrCode::OpPostDecrement);
                }
                else if (c == '?') {
                    this->ParseConditional(ops);
                    break;
                }
                else if (auto op = this->ReadBinaryOperator()) {
                    this->PushBinary(ops, *op);
                    if (!this->ParseOperand(ops))
                        this->Fail(Error::Value);
                }
                else {
                    break;
                }
            }
            while (!ops.empty()) {
                this->EmitOperator(ops.back());
                ops.pop_back();
            }
            return true;
        }

        // Parse "condition ? a : b" (the condition is on the operators stack)
        void ParseConditional(std::vector<Op>& ops) {
            while (!ops.empty() && Priority(ops.back()) < ASSIGN_PRIORITY) {
                this->EmitOperator(ops.back());
                ops.pop_back();
            }
            this->pos++;
            std::size_t jump_else = this->Emit(InstrCode::JumpElse);
            this->ParseExpression();
            this->Expect(':', Error::Colon);
            std::size_t jump_end = this->Emit(InstrCode::JumpTo);
            this->Patch(jump_else, this->Here());
            this->ParseExpression();
            this->Patch(jump_end, this->Here());
        }

        // Parse the prefix operators and a value
        bool ParseOperand(std::vector<Op>& ops) {
            bool prefix = false;
            while (true) {
                this->SkipWhitespace();
                char c = this->Peek(), n = this->Peek(1);
                std::optional<Op> op;
                if (c == '+' && n == '+') {
                    op = Op::PreIncrement;
                    this->pos += 2;
                }
                else if (c == '-' && n == '-') {
                    op = Op::PreDecrement;
                    this->pos += 2;
                }
                else if (c == '+' || c == '-' || c == '~' || (c == '!' && n != '=')) {
                    op = c == '+' ? Op::UnaryPlus : c == '-' ? Op::UnaryMinus : c == '~' ? Op::BitwiseNot : Op::Not;
                    this->pos++;
                }
                else if (this->MatchKeyword("new")) {
                    op = Op::New;
                }
                if (!op)
                    break;
                ops.push_back(*op);
                prefix = true;
            }
            if (this->ParseValue())
                return true;
            if (prefix)
                this->Fail(Error::Value);
            return false;
        }

        // Parse the arguments of a call or a construction
        void ParseCall(std::vector<Op>& ops) {
            std::size_t start = this->pos;
            this->pos++;
            this->Emit(InstrCode::PushArgs);
            if (!this->Match(')')) {
                std::size_t index = 0;
                while (true) {
                    this->ParseExpression();
                    this->Emit(Instruction(InstrCode::DefineArg, index++));
                    if (this->Match(')'))
                        break;
                    if (!this->Match(','))
                        this->Fail(Error::ParenthesisClose);
                }
            }
            if (!ops.empty() && ops.back() == Op::New) {
                ops.pop_back();
                this->Emit(InstrCode::ValueObject, start);
            }
            else {
                this->Emit(InstrCode::OpCall, start);
            }
        }

        // Parse a value (literal, variable, function, class, array or parenthesized expression)
        bool ParseValue() {
            this->SkipWhitespace();
            char c = this->Peek();
            if (IsDigit(c) || (c == '.' && IsDigit(this->Peek(1)))) {
                this->ParseNumber();
                return true;
            }
            if (c == '"' || c == '\'') {
                this->Emit(Instruction(InstrCode::ValueString, this->ReadString()));
                return true;
            }
            if (c == '[') {
                this->ParseArray();
                return true;
            }
            if (c == '{') {
                this->ParseNamedArray();
                return true;
            }
            if (c == '(') {
                this->pos++;
                this->ParseExpression();
                this->Expect(')', Error::ParenthesisClose);
                return true;
            }
            std::string_view name = this->PeekName();
            if (name.empty())
                return false;
            if (name == "undefined")
                this->Emit(InstrCode::ValueUndefined);
            else if (name == "null")
                this->Emit(InstrCode::ValueNull);
            else if (name == "this")
                this->Emit(InstrCode::ValueThis);
            else if (name == "true")
                this->Emit(Instruction(InstrCode::ValueBoolean, true));
            else if (name == "false")
                this->Emit(Instruction(InstrCode::ValueBoolean, false));
            else if (name == "function") {
                this->pos += name.size();
                // The name of a function expression is optional and ignored
                this->SkipWhitespace();
                if (IsNameStart(this->Peek()))
                    this->ReadVariableName(Error::Name);
                this->ParseFunction();
                return true;
            }
            else if (name == "class") {
                this->pos += name.size();
                this->ParseClass("constructor");
                return true;
            }
            else if (IsReserved(name))
                return false;
            else
                this->Emit(Instruction(InstrCode::ValueVariable, name));
            this->pos += name.size();
            return true;
        }

        void ParseNumber() {
            std::size_t start = this->pos;
            bool integer = true;
            while (IsDigit(this->Peek()))
                this->pos++;
            if (this->Peek() == '.' && IsDigit(this->Peek(1))) {
                integer = false;
                this->pos++;
                while (IsDigit(this->Peek()))
                    this->pos++;
            }
            if (this->Peek() == 'e' || this->Peek() == 'E') {
                std::size_t exponent = this->pos + 1;
                if (exponent < this->text.size() && (this->text[exponent] == '+' || this->text[exponent] == '-'))
                    exponent++;
                if (exponent < this->text.size() && IsDigit(this->text[exponent])) {
                    integer = false;
                    this->pos = exponent;
                    while (IsDigit(this->Peek()))
                        this->pos++;
                }
            }
            const char * first = this->text.data() + start;
            const char * last = this->text.data() + this->pos;
            if (integer) {
                std::int64_t value;
                auto r = std::from_chars(first, last, value);
                if (r.ec == std::errc() && r.ptr == last) {
                    this->Emit(Instruction(InstrCode::ValueNumber, value));
                    return;
                }
            }
            double value = 0;
            if (!LiteScript::Internal::ParseDouble(std::string_view(first, static_cast<std::size_t>(last - first)), value)) {
                // The number is too big or too small to be represented
                std::string_view digits(first, static_cast<std::size_t>(last - first));
                bool small = digits.find("e-") != std::string_view::npos || digits.find("E-") != std::string_view::npos;
                value = small ? 0.0 : std::numeric_limits<double>::infinity();
            }
            this->Emit(Instruction(InstrCode::ValueNumber, value));
        }

        // Parse "[ a, b, ... ]"
        void ParseArray() {
            this->pos++;
            this->Emit(InstrCode::ValueArray);
            if (this->Match(']'))
                return;
            std::size_t index = 0;
            while (true) {
                this->ParseExpression();
                this->Emit(Instruction(InstrCode::ArrayPushNumeric, index++));
                if (this->Match(']'))
                    return;
                if (!this->Match(','))
                    this->Fail(Error::SimpleArrayEnd);
                if (this->Match(']'))
                    return;
            }
        }

        // Parse "{ name: a, "name": b, ... }"
        void ParseNamedArray() {
            this->pos++;
            this->Emit(InstrCode::ValueArray);
            if (this->Match('}'))
                return;
            while (true) {
                this->SkipWhitespace();
                std::string key;
                if (this->Peek() == '"' || this->Peek() == '\'')
                    key = this->ReadString();
                else
                    key = this->ReadName(Error::NamedArrayEnd);
                this->Expect(':', Error::NamedArrayColon);
                this->ParseExpression(true, Error::NamedArrayExpression);
                this->Emit(Instruction(InstrCode::ArrayPushLiteral, std::move(key)));
                if (this->Match('}'))
                    return;
                if (!this->Match(','))
                    this->Fail(Error::NamedArrayEnd);
                if (this->Match('}'))
                    return;
            }
        }

        // Parse the arguments and the body of a function, the callback is pushed on the stack
        void ParseFunction() {
            std::size_t jump = this->Emit(InstrCode::JumpTo);
            std::size_t start = this->Here();
            this->ParseArguments();
            // break and continue can't leave a function
            std::vector<Breakable> outer = std::move(this->breakables);
            this->breakables.clear();
            this->SkipWhitespace();
            if (this->Peek() != '{')
                this->Fail(Error::CallbackInstructions);
            this->ParseBlock();
            this->breakables = std::move(outer);
            this->Emit(InstrCode::Return);
            this->Patch(jump, this->Here());
            this->Emit(Instruction(InstrCode::ValueCallback, start));
        }

        // Parse "(a, b, c...)", the arguments are copied in local variables by "value.arg"
        void ParseArguments() {
            this->Expect('(', Error::CallbackArguments);
            if (this->Match(')'))
                return;
            for (std::size_t index = 0; true; index++) {
                std::string name = this->ReadVariableName(Error::CallbackArgumentName);
                this->SkipWhitespace();
                if (this->text.substr(this->pos, 3) == "...") {
                    this->pos += 3;
                    this->Emit(Instruction(InstrCode::ValueArgs, index));
                    this->Emit(Instruction(InstrCode::DefineVariable, std::move(name)));
                    this->Expect(')', Error::CallbackArgumentEnd);
                    return;
                }
                this->Emit(Instruction(InstrCode::ValueArg, index));
                this->Emit(Instruction(InstrCode::DefineVariable, std::move(name)));
                if (this->Match(')'))
                    return;
                if (!this->Match(','))
                    this->Fail(Error::CallbackArgumentEnd);
            }
        }

        // Parse the inherits and the members of a class, the class is pushed on the stack
        void ParseClass(std::string_view constructor) {
            this->Emit(InstrCode::ValueClass);
            if (this->Match(':')) {
                do {
                    this->ParseExpression(true, Error::ClassInherit);
                    this->Emit(InstrCode::ClassInherit);
                } while (this->Match(','));
            }
            this->Expect('{', Error::ClassBegin);
            while (!this->Match('}')) {
                if (this->AtEnd())
                    this->Fail(Error::ClassEnd);
                this->ParseClassMember();
            }
            this->Emit(Instruction(InstrCode::ClassConstructor, constructor));
        }

        void ParseClassMember() {
            if (this->Match(';'))
                return;
            if (this->MatchKeyword("operator")) {
                std::string name = this->ReadName(Error::ClassOperatorName);
                std::size_t op = 0;
                while (op < OPERATOR_NAMES.size() && OPERATOR_NAMES[op] != name)
                    op++;
                if (op == OPERATOR_NAMES.size())
                    this->Fail(Error::ClassOperatorInvalid);
                this->ParseFunction();
                this->Emit(Instruction(InstrCode::ClassPushOperator, op));
                return;
            }
            if (this->MatchKeyword("class")) {
                std::string name = this->ReadVariableName(Error::Name);
                this->ParseClass(name);
                this->Emit(Instruction(InstrCode::ClassPushStatic, std::move(name)));
                return;
            }
            bool is_static = this->MatchKeyword("static");
            InstrCode push = is_static ? InstrCode::ClassPushStatic : InstrCode::ClassPushUnstatic;
            if (this->MatchKeyword("var")) {
                this->ParseVariables(push);
                this->Expect(';', Error::Semicolon);
            }
            else if (this->MatchKeyword("function")) {
                std::string name = this->ReadVariableName(Error::Name);
                this->ParseFunction();
                this->Emit(Instruction(push, std::move(name)));
            }
            else {
                this->Fail(Error::ClassInvalid);
            }
        }

        ////// INSTRUCTIONS //////

        void ParseInstruction() {
            DepthGuard guard(*this);
            this->SkipWhitespace();
            if (this->AtEnd())
                this->Fail(Error::Instruction);
            char c = this->Peek();
            if (c == ';') {
                this->pos++;
                return;
            }
            if (c == '{') {
                this->ParseBlock();
                return;
            }
            std::string_view word = this->PeekName();
            if (word == "var") {
                this->pos += word.size();
                this->ParseVariables(InstrCode::DefineVariable);
                this->Expect(';', Error::Semicolon);
            }
            else if (word == "function" && this->KeywordFollowedByName("function")) {
                this->pos += word.size();
                std::string name = this->ReadVariableName(Error::Name);
                this->ParseFunction();
                this->Emit(Instruction(InstrCode::DefineVariable, std::move(name)));
            }
            else if (word == "class" && this->KeywordFollowedByName("class")) {
                this->pos += word.size();
                std::string name = this->ReadVariableName(Error::Name);
                this->ParseClass(name);
                this->Emit(Instruction(InstrCode::DefineVariable, std::move(name)));
            }
            else if (word == "if") {
                this->ParseIf();
            }
            else if (word == "return") {
                this->ParseReturn();
            }
            else if (word == "break" || word == "continue") {
                this->ParseJump(word == "break");
            }
            else if (word == "while") {
                this->ParseWhile();
            }
            else if (word == "do") {
                this->ParseDo();
            }
            else if (word == "for") {
                this->ParseFor();
            }
            else if (word == "switch") {
                this->ParseSwitch();
            }
            else if (word == "namespace") {
                this->ParseNamespace();
            }
            else if (word == "try") {
                this->ParseTry();
            }
            else if (word == "throw") {
                this->ParseThrow();
            }
            else {
                if (!this->ParseExpression(false))
                    this->Fail(Error::Unknown);
                this->Expect(';', Error::Semicolon);
                this->Emit(InstrCode::ValuePop);
            }
        }

        void ParseBlock() {
            this->Expect('{', Error::BraceOpen);
            while (!this->Match('}')) {
                if (this->AtEnd())
                    this->Fail(Error::BraceClose);
                this->ParseInstruction();
            }
        }

        // Parse "name [= value], ..." (after "var"), the values are defined by the instruction
        void ParseVariables(InstrCode define) {
            do {
                std::string name = this->ReadVariableName(Error::Name);
                if (this->Match('='))
                    this->ParseExpression();
                else
                    this->Emit(InstrCode::ValueNull);
                this->Emit(InstrCode::ValueAssign);
                this->Emit(Instruction(define, std::move(name)));
            } while (this->Match(','));
        }

        // Parse "(condition)"
        void ParseCondition() {
            this->Expect('(', Error::ParenthesisOpen);
            this->ParseExpression();
            this->Expect(')', Error::ParenthesisClose);
        }

        void ParseIf() {
            this->MatchKeyword("if");
            this->ParseCondition();
            std::size_t jump_else = this->Emit(InstrCode::JumpElse);
            this->ParseInstruction();
            if (this->MatchKeyword("else")) {
                std::size_t jump_end = this->Emit(InstrCode::JumpTo);
                this->Patch(jump_else, this->Here());
                this->ParseInstruction();
                this->Patch(jump_end, this->Here());
            }
            else {
                this->Patch(jump_else, this->Here());
            }
        }

        void ParseReturn() {
            this->MatchKeyword("return");
            if (this->Match(';')) {
                this->Emit(InstrCode::Return);
                return;
            }
            this->ParseExpression(true, Error::Return);
            this->Emit(InstrCode::DefineReturn);
            this->Emit(InstrCode::Return);
            this->Expect(';', Error::Semicolon);
        }

        void ParseJump(bool is_break) {
            this->MatchKeyword(is_break ? "break" : "continue");
            // break leaves the nearest loop or switch, continue the nearest loop
            auto target = this->breakables.rbegin();
            for (; target != this->breakables.rend(); ++target) {
                if (target->kind == Breakable::Kind::Loop || (is_break && target->kind == Breakable::Kind::Switch))
                    break;
            }
            if (target == this->breakables.rend())
                this->Fail(is_break ? Error::BreakOutsideLoop : Error::ContinueOutsideLoop);
            // The try blocks that are left are closed and the values of the switches are removed from the stack
            for (auto it = this->breakables.rbegin(); it != target; ++it)
                this->Emit(it->kind == Breakable::Kind::Try ? InstrCode::TryEnd : InstrCode::ValuePop);
            (is_break ? target->breaks : target->continues).push_back(this->Emit(InstrCode::JumpTo));
            this->Expect(';', Error::Semicolon);
        }

        void ParseThrow() {
            std::size_t start = this->pos;
            this->MatchKeyword("throw");
            this->ParseExpression();
            this->Emit(InstrCode::Throw, start);
            this->Expect(';', Error::Semicolon);
        }

        // Parse "try { ... } catch (name) { ... }" (the name is optional)
        void ParseTry() {
            this->MatchKeyword("try");
            std::size_t begin = this->Emit(InstrCode::TryBegin);
            Breakable context;
            context.kind = Breakable::Kind::Try;
            this->breakables.push_back(std::move(context));
            this->SkipWhitespace();
            if (this->Peek() != '{')
                this->Fail(Error::BraceOpen);
            this->ParseBlock();
            this->breakables.pop_back();
            this->Emit(InstrCode::TryEnd);
            std::size_t jump_end = this->Emit(InstrCode::JumpTo);
            // The error is on the stack when the catch block starts
            this->Patch(begin, this->Here());
            if (!this->MatchKeyword("catch"))
                this->Fail(Error::Catch);
            if (this->Match('(')) {
                this->Emit(Instruction(InstrCode::DefineVariable, this->ReadVariableName(Error::Name)));
                this->Expect(')', Error::ParenthesisClose);
            }
            else {
                this->Emit(InstrCode::ValuePop);
            }
            this->SkipWhitespace();
            if (this->Peek() != '{')
                this->Fail(Error::BraceOpen);
            this->ParseBlock();
            this->Patch(jump_end, this->Here());
        }

        // Parse the body of a loop and resolve its break and continue jumps
        template <typename F>
        void ParseLoopBody(std::size_t continue_target, F&& after_body) {
            this->breakables.emplace_back();
            this->ParseInstruction();
            after_body();
            Breakable loop = std::move(this->breakables.back());
            this->breakables.pop_back();
            for (std::size_t index : loop.breaks)
                this->Patch(index, this->Here());
            for (std::size_t index : loop.continues)
                this->Patch(index, continue_target);
        }

        void ParseWhile() {
            this->MatchKeyword("while");
            std::size_t condition = this->Here();
            this->ParseCondition();
            std::size_t jump_end = this->Emit(InstrCode::JumpElse);
            this->ParseLoopBody(condition, [&]() {
                this->Emit(Instruction(InstrCode::JumpTo, condition));
                this->Patch(jump_end, this->Here());
            });
        }

        void ParseDo() {
            this->MatchKeyword("do");
            std::size_t start = this->Here();
            this->breakables.emplace_back();
            this->ParseInstruction();
            if (!this->MatchKeyword("while"))
                this->Fail(Error::While);
            std::size_t condition = this->Here();
            this->ParseCondition();
            this->Emit(Instruction(InstrCode::JumpIf, start));
            this->Expect(';', Error::Semicolon);
            Breakable loop = std::move(this->breakables.back());
            this->breakables.pop_back();
            for (std::size_t index : loop.breaks)
                this->Patch(index, this->Here());
            for (std::size_t index : loop.continues)
                this->Patch(index, condition);
        }

        void ParseFor() {
            this->MatchKeyword("for");
            this->Expect('(', Error::ParenthesisOpen);
            // Initialisation
            if (!this->Match(';')) {
                if (this->MatchKeyword("var")) {
                    this->ParseVariables(InstrCode::DefineVariable);
                }
                else {
                    this->ParseExpression(true, Error::ForInitialisation);
                    this->Emit(InstrCode::ValuePop);
                }
                this->Expect(';', Error::Semicolon);
            }
            // Condition (always true if empty)
            std::size_t condition = this->Here();
            if (!this->ParseExpression(false))
                this->Emit(Instruction(InstrCode::ValueBoolean, true));
            this->Expect(';', Error::Semicolon);
            std::size_t jump_end = this->Emit(InstrCode::JumpElse);
            std::size_t jump_body = this->Emit(InstrCode::JumpTo);
            // Iteration
            std::size_t iteration = this->Here();
            if (this->ParseExpression(false))
                this->Emit(InstrCode::ValuePop);
            this->Expect(')', Error::ParenthesisClose);
            this->Emit(Instruction(InstrCode::JumpTo, condition));
            this->Patch(jump_body, this->Here());
            this->ParseLoopBody(iteration, [&]() {
                this->Emit(Instruction(InstrCode::JumpTo, iteration));
                this->Patch(jump_end, this->Here());
            });
        }

        void ParseSwitch() {
            this->MatchKeyword("switch");
            this->ParseCondition();
            this->Expect('{', Error::BraceOpen);
            Breakable context;
            context.kind = Breakable::Kind::Switch;
            this->breakables.push_back(std::move(context));
            // The jump at the end of the previous case body (to the next body)
            std::optional<std::size_t> jump_next_body;
            while (!this->Match('}')) {
                if (this->AtEnd())
                    this->Fail(Error::SwitchEnd);
                if (this->MatchKeyword("case")) {
                    // The switch value stays on the stack, it's copied for each comparison
                    this->Emit(InstrCode::ValueCopy);
                    this->ParseExpression();
                    this->Expect(':', Error::Colon);
                    this->Emit(InstrCode::OpEqual);
                    std::size_t jump_next_case = this->Emit(InstrCode::JumpElse);
                    if (jump_next_body)
                        this->Patch(*jump_next_body, this->Here());
                    this->ParseCaseBody();
                    jump_next_body = this->Emit(InstrCode::JumpTo);
                    this->Patch(jump_next_case, this->Here());
                }
                else if (this->MatchKeyword("default")) {
                    this->Expect(':', Error::Colon);
                    if (jump_next_body)
                        this->Patch(*jump_next_body, this->Here());
                    jump_next_body.reset();
                    this->ParseCaseBody();
                    // The default case must be the last one
                    this->Expect('}', Error::BraceClose);
                    break;
                }
                else {
                    this->Fail(Error::SwitchKeywords);
                }
            }
            if (jump_next_body)
                this->Patch(*jump_next_body, this->Here());
            Breakable context_end = std::move(this->breakables.back());
            this->breakables.pop_back();
            for (std::size_t index : context_end.breaks)
                this->Patch(index, this->Here());
            this->Emit(InstrCode::ValuePop);
        }

        void ParseCaseBody() {
            while (true) {
                this->SkipWhitespace();
                std::string_view word = this->PeekName();
                if (this->AtEnd() || this->Peek() == '}' || word == "case" || word == "default")
                    return;
                this->ParseInstruction();
            }
        }

        void ParseNamespace() {
            this->MatchKeyword("namespace");
            std::string path = this->ReadName(Error::Namespace);
            while (this->Match('.'))
                path += '.' + this->ReadName(Error::Namespace);
            this->Expect(';', Error::Semicolon);
            this->Emit(Instruction(InstrCode::NamespaceUse, std::move(path)));
        }

    };

}

LiteScript::Script::Result LiteScript::Script::Compile(std::string_view code) {
    Result result;
    Compiler compiler(code, result.instructions);
    try {
        compiler.CompileScript();
        // Convert the offsets of the instructions into lines and columns
        std::vector<std::size_t> line_starts{ 0 };
        for (std::size_t i = 0; i < code.size(); i++) {
            if (code[i] == '\n')
                line_starts.push_back(i + 1);
        }
        for (Instruction& instr : result.instructions) {
            std::size_t offset = std::min<std::size_t>(instr.position.line, code.size());
            auto it = std::upper_bound(line_starts.begin(), line_starts.end(), offset) - 1;
            std::size_t column = 0;
            for (std::size_t i = *it; i < offset; i++) {
                if ((static_cast<unsigned char>(code[i]) & 0xC0) != 0x80)
                    column++;
            }
            instr.position.line = static_cast<std::uint32_t>(it - line_starts.begin());
            instr.position.column = static_cast<std::uint32_t>(column);
        }
    }
    catch (const CompileError& e) {
        result.instructions.clear();
        result.error = e.error;
        std::size_t line_start = 0;
        for (std::size_t i = 0; i < e.position && i < code.size(); i++) {
            if (code[i] == '\n') {
                result.line++;
                line_start = i + 1;
            }
        }
        // The column is counted in characters (the continuation bytes of UTF-8 are ignored)
        for (std::size_t i = line_start; i < e.position && i < code.size(); i++) {
            if ((static_cast<unsigned char>(code[i]) & 0xC0) != 0x80)
                result.column++;
        }
    }
    return result;
}

LiteScript::Script::Result LiteScript::Script::Execute(State& state, std::string_view code) {
    Result result = Compile(code);
    if (result) {
        state.AddInstructions(result.instructions);
        result.value = state.Execute();
    }
    return result;
}

std::string_view LiteScript::Script::GetErrorMessage(Error error) noexcept {
    switch (error) {
        case Error::None: return "No error";
        case Error::SimpleArrayEnd: return "Closing bracket \"]\" expected for array";
        case Error::NamedArrayColon: return "Colon \":\" character expected after array member name";
        case Error::NamedArrayExpression: return "Expression expected after array member name";
        case Error::NamedArrayEnd: return "Closing brace \"}\" expected for array closing";
        case Error::CallbackArguments: return "Arguments expected for callback definition";
        case Error::CallbackArgumentName: return "Variable name expected for arguments";
        case Error::CallbackArgumentEnd: return "Closing parenthesis \")\" expected for arguments";
        case Error::CallbackInstructions: return "Instructions expected for callback definition";
        case Error::ClassInherit: return "Class to inherit expected for class definition";
        case Error::ClassBegin: return "Opening brace \"{\" expected for class definition";
        case Error::ClassInvalid: return "Invalid member inside class definition";
        case Error::ClassOperatorName: return "Operator name expected for class definition";
        case Error::ClassOperatorInvalid: return "Invalid operator name for class definition";
        case Error::ClassEnd: return "Closing brace \"}\" expected for class definition";
        case Error::ForInitialisation: return "Initialisation expected in for control statement";
        case Error::ForCondition: return "Condition expected in for control statement";
        case Error::SwitchKeywords: return "\"case\" or \"default\" keywords expected in switch control statement";
        case Error::SwitchEnd: return "Closing brace \"}\" expected in switch control statement";
        case Error::Name: return "Variable name expected";
        case Error::Value: return "Value expected";
        case Error::Operator: return "Invalid operator";
        case Error::Expression: return "Expression expected";
        case Error::ParenthesisOpen: return "Opening parenthesis \"(\" expected";
        case Error::ParenthesisClose: return "Closing parenthesis \")\" expected";
        case Error::BracketClose: return "Closing bracket \"]\" expected";
        case Error::BraceOpen: return "Opening brace \"{\" expected";
        case Error::BraceClose: return "Closing brace \"}\" expected";
        case Error::Colon: return "Colon \":\" expected";
        case Error::Semicolon: return "Semi-colon \";\" expected";
        case Error::Instruction: return "Instruction expected";
        case Error::Return: return "Expression or semi-colon expected for return";
        case Error::While: return "\"while\" keyword expected in do..while control statement";
        case Error::Namespace: return "Invalid namespace";
        case Error::Unknown: return "Unknown element";
        case Error::UnterminatedString: return "Closing quote expected for string";
        case Error::BreakOutsideLoop: return "\"break\" outside of a loop or a switch";
        case Error::ContinueOutsideLoop: return "\"continue\" outside of a loop";
        case Error::Nesting: return "Too many nested elements";
        case Error::Catch: return "\"catch\" expected after a try block";
    }
    return "Unknown error";
}
