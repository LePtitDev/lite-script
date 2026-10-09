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
    using Analysis = LiteScript::Script::Analysis;

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

        // The information collected for the development tools (nullptr if not requested),
        // the positions are offsets converted at the end of the compilation
        Analysis * analysis = nullptr;

        // The scopes containing the current position (indexes in analysis->scopes)
        std::vector<std::size_t> scopes;

        // The namespace selected by the last "namespace" instruction (empty for the global namespace)
        std::string nspace;

        // The last function parsed, attached to its declaration
        struct FunctionInfo {
            std::size_t scope = Analysis::NONE;
            std::size_t first = Analysis::NONE;
            std::size_t end = Analysis::NONE;
            std::size_t end_offset = 0;
            std::vector<std::string> parameters;
        } last_function;

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

        Compiler(std::string_view code, std::vector<Instruction>& instructions, Analysis * info) :
            text(code), out(instructions), analysis(info) {}

        void CompileScript() {
            this->OpenScope(Analysis::Scope::Kind::Script, 0);
            while (true) {
                this->SkipWhitespace();
                if (this->AtEnd())
                    break;
                this->ParseInstruction();
            }
            this->CloseScope(this->text.size());
        }

        // Close the scopes left open by an error
        void CloseScopes(std::size_t end) {
            while (!this->scopes.empty())
                this->CloseScope(end);
        }

    private:

        ////// ANALYSIS //////

        static Analysis::Range MakeRange(std::size_t start, std::size_t end) {
            Analysis::Range range;
            range.start.offset = start;
            range.end.offset = end;
            return range;
        }

        std::size_t CurrentScope() const {
            return this->scopes.empty() ? Analysis::NONE : this->scopes.back();
        }

        std::size_t OpenScope(Analysis::Scope::Kind kind, std::size_t start) {
            if (this->analysis == nullptr)
                return Analysis::NONE;
            Analysis::Scope scope;
            scope.kind = kind;
            scope.range = MakeRange(start, start);
            scope.parent = this->CurrentScope();
            this->analysis->scopes.push_back(scope);
            this->scopes.push_back(this->analysis->scopes.size() - 1);
            return this->scopes.back();
        }

        void CloseScope(std::size_t end) {
            if (this->analysis == nullptr || this->scopes.empty())
                return;
            this->analysis->scopes[this->scopes.back()].range.end.offset = end;
            this->scopes.pop_back();
        }

        // Record a declaration whose name ends at the current position
        // @return The index of the symbol (NONE if there is no analysis)
        std::size_t Declare(std::string_view name, Analysis::SymbolKind kind, bool is_static = false) {
            if (this->analysis == nullptr)
                return Analysis::NONE;
            Analysis::Symbol symbol;
            symbol.name = name;
            symbol.kind = kind;
            symbol.is_static = is_static;
            symbol.range = MakeRange(this->pos - name.size(), this->pos);
            symbol.full = symbol.range;
            symbol.scope = this->CurrentScope();
            if (symbol.scope != Analysis::NONE && this->analysis->scopes[symbol.scope].kind == Analysis::Scope::Kind::Script)
                symbol.nspace = this->nspace;
            this->analysis->symbols.push_back(std::move(symbol));
            return this->analysis->symbols.size() - 1;
        }

        // Attach the last function parsed to a declaration that ends at the current position
        void AttachFunction(std::size_t symbol, std::size_t start) {
            if (this->analysis == nullptr || symbol == Analysis::NONE)
                return;
            Analysis::Symbol& s = this->analysis->symbols[symbol];
            s.full = MakeRange(start, this->last_function.end_offset);
            s.body = this->last_function.scope;
            s.parameters = this->last_function.parameters;
            s.first_instruction = this->last_function.first;
            s.end_instruction = this->last_function.end;
        }

        // Record a use of a name that starts at an offset
        void Refer(std::string_view name, std::size_t start, bool member) {
            if (this->analysis == nullptr)
                return;
            Analysis::Reference reference;
            reference.name = name;
            reference.range = MakeRange(start, start + name.size());
            reference.scope = this->CurrentScope();
            reference.member = member;
            reference.nspace = this->nspace;
            this->analysis->references.push_back(std::move(reference));
        }

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
                    std::string name = this->ReadName(Error::Name);
                    this->Refer(name, this->pos - name.size(), true);
                    this->Emit(Instruction(InstrCode::OpMember, std::move(name)), start);
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
            else {
                this->Refer(name, this->pos, false);
                this->Emit(Instruction(InstrCode::ValueVariable, name));
            }
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
            this->SkipWhitespace();
            std::size_t scope = this->OpenScope(Analysis::Scope::Kind::Function, this->pos);
            std::vector<std::string> parameters;
            this->ParseArguments(parameters);
            // break and continue can't leave a function
            std::vector<Breakable> outer = std::move(this->breakables);
            this->breakables.clear();
            this->SkipWhitespace();
            if (this->Peek() != '{')
                this->Fail(Error::CallbackInstructions);
            this->ParseBlock();
            this->breakables = std::move(outer);
            this->Emit(InstrCode::Return);
            this->CloseScope(this->pos);
            this->last_function = FunctionInfo{ scope, start, this->Here(), this->pos, std::move(parameters) };
            this->Patch(jump, this->Here());
            this->Emit(Instruction(InstrCode::ValueCallback, start));
        }

        // Parse "(a, b, c...)", the arguments are copied in local variables by "value.arg"
        void ParseArguments(std::vector<std::string>& parameters) {
            this->Expect('(', Error::CallbackArguments);
            if (this->Match(')'))
                return;
            for (std::size_t index = 0; true; index++) {
                std::string name = this->ReadVariableName(Error::CallbackArgumentName);
                this->Declare(name, Analysis::SymbolKind::Parameter);
                parameters.push_back(name);
                this->SkipWhitespace();
                if (this->text.substr(this->pos, 3) == "...") {
                    parameters.back() += "...";
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
        // @param symbol The declaration of the class (NONE if anonymous)
        void ParseClass(std::string_view constructor, std::size_t symbol = Analysis::NONE, std::size_t start = 0) {
            this->Emit(InstrCode::ValueClass);
            if (this->Match(':')) {
                do {
                    this->ParseExpression(true, Error::ClassInherit);
                    this->Emit(InstrCode::ClassInherit);
                } while (this->Match(','));
            }
            this->Expect('{', Error::ClassBegin);
            std::size_t scope = this->OpenScope(Analysis::Scope::Kind::Class, this->pos - 1);
            if (this->analysis != nullptr && symbol != Analysis::NONE)
                this->analysis->symbols[symbol].body = scope;
            while (!this->Match('}')) {
                if (this->AtEnd())
                    this->Fail(Error::ClassEnd);
                this->ParseClassMember();
            }
            this->CloseScope(this->pos);
            if (this->analysis != nullptr && symbol != Analysis::NONE)
                this->analysis->symbols[symbol].full = MakeRange(start, this->pos);
            this->Emit(Instruction(InstrCode::ClassConstructor, constructor));
        }

        void ParseClassMember() {
            if (this->Match(';'))
                return;
            this->SkipWhitespace();
            std::size_t start = this->pos;
            if (this->MatchKeyword("operator")) {
                std::string name = this->ReadName(Error::ClassOperatorName);
                std::size_t op = 0;
                while (op < OPERATOR_NAMES.size() && OPERATOR_NAMES[op] != name)
                    op++;
                if (op == OPERATOR_NAMES.size())
                    this->Fail(Error::ClassOperatorInvalid);
                std::size_t symbol = this->Declare(name, Analysis::SymbolKind::Operator);
                this->ParseFunction();
                this->AttachFunction(symbol, start);
                this->Emit(Instruction(InstrCode::ClassPushOperator, op));
                return;
            }
            if (this->MatchKeyword("class")) {
                std::string name = this->ReadVariableName(Error::Name);
                std::size_t symbol = this->Declare(name, Analysis::SymbolKind::Class, true);
                this->ParseClass(name, symbol, start);
                this->Emit(Instruction(InstrCode::ClassPushStatic, std::move(name)));
                return;
            }
            bool is_static = this->MatchKeyword("static");
            InstrCode push = is_static ? InstrCode::ClassPushStatic : InstrCode::ClassPushUnstatic;
            if (this->MatchKeyword("var")) {
                this->ParseVariables(push, is_static);
                this->Expect(';', Error::Semicolon);
            }
            else if (this->MatchKeyword("function")) {
                std::string name = this->ReadVariableName(Error::Name);
                std::size_t symbol = this->Declare(name, Analysis::SymbolKind::Method, is_static);
                this->ParseFunction();
                this->AttachFunction(symbol, start);
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
            std::size_t start = this->pos;
            if (this->analysis != nullptr) {
                Analysis::Statement statement;
                statement.position.offset = start;
                statement.instruction = this->Here();
                this->analysis->statements.push_back(statement);
            }
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
                std::size_t symbol = this->Declare(name, Analysis::SymbolKind::Function);
                this->ParseFunction();
                this->AttachFunction(symbol, start);
                this->Emit(Instruction(InstrCode::DefineVariable, std::move(name)));
            }
            else if (word == "class" && this->KeywordFollowedByName("class")) {
                this->pos += word.size();
                std::string name = this->ReadVariableName(Error::Name);
                std::size_t symbol = this->Declare(name, Analysis::SymbolKind::Class);
                this->ParseClass(name, symbol, start);
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
        void ParseVariables(InstrCode define, bool is_static = false) {
            do {
                std::string name = this->ReadVariableName(Error::Name);
                std::size_t start = this->pos - name.size();
                bool member = define != InstrCode::DefineVariable;
                std::size_t symbol = this->Declare(name, member ? Analysis::SymbolKind::Attribute : Analysis::SymbolKind::Variable, is_static);
                if (this->Match('=')) {
                    // A variable initialized with a function is a function (or a method)
                    this->SkipWhitespace();
                    bool function = this->PeekName() == "function";
                    std::size_t init = this->pos;
                    this->ParseExpression();
                    // The expression is only the function (nothing after its body)
                    function = function && this->last_function.end_offset > init && this->last_function.end_offset <= this->pos &&
                        this->text.substr(this->last_function.end_offset, this->pos - this->last_function.end_offset).find_first_not_of(" \t\r\n") == std::string_view::npos;
                    if (function && symbol != Analysis::NONE) {
                        this->analysis->symbols[symbol].kind = member ? Analysis::SymbolKind::Method : Analysis::SymbolKind::Function;
                        this->AttachFunction(symbol, start);
                    }
                }
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
                std::string name = this->ReadVariableName(Error::Name);
                this->Declare(name, Analysis::SymbolKind::CatchVariable);
                this->Emit(Instruction(InstrCode::DefineVariable, std::move(name)));
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
            std::size_t start = this->pos - path.size();
            while (this->Match('.'))
                path += '.' + this->ReadName(Error::Namespace);
            if (this->analysis != nullptr && path != "global") {
                // The symbol covers the whole path ("A.B")
                std::size_t symbol = this->Declare(path, Analysis::SymbolKind::Namespace);
                this->analysis->symbols[symbol].range = MakeRange(start, this->pos);
                this->analysis->symbols[symbol].full = this->analysis->symbols[symbol].range;
                this->analysis->symbols[symbol].nspace.clear();
            }
            this->nspace = path == "global" ? std::string() : path;
            this->Expect(';', Error::Semicolon);
            this->Emit(Instruction(InstrCode::NamespaceUse, std::move(path)));
        }

    };

}

LiteScript::Script::Result LiteScript::Script::Compile(std::string_view code, Analysis * analysis) {
    Result result;
    if (analysis != nullptr)
        *analysis = Analysis();
    Compiler compiler(code, result.instructions, analysis);

    // Convert an offset into a line and a column (counted in characters: the continuation bytes of UTF-8 are ignored)
    std::vector<std::size_t> line_starts{ 0 };
    for (std::size_t i = 0; i < code.size(); i++) {
        if (code[i] == '\n')
            line_starts.push_back(i + 1);
    }
    auto locate = [&](std::size_t offset, std::size_t& line, std::size_t& column) {
        offset = std::min(offset, code.size());
        auto it = std::upper_bound(line_starts.begin(), line_starts.end(), offset) - 1;
        line = static_cast<std::size_t>(it - line_starts.begin());
        column = 0;
        for (std::size_t i = *it; i < offset; i++) {
            if ((static_cast<unsigned char>(code[i]) & 0xC0) != 0x80)
                column++;
        }
    };

    try {
        compiler.CompileScript();
        for (Instruction& instr : result.instructions) {
            std::size_t line, column;
            locate(instr.position.line, line, column);
            instr.position.line = static_cast<std::uint32_t>(line);
            instr.position.column = static_cast<std::uint32_t>(column);
        }
    }
    catch (const CompileError& e) {
        result.instructions.clear();
        result.error = e.error;
        locate(e.position, result.line, result.column);
        compiler.CloseScopes(std::min(e.position, code.size()));
    }

    if (analysis != nullptr) {
        auto convert = [&](Analysis::Position& position) {
            locate(position.offset, position.line, position.column);
        };
        auto convert_range = [&](Analysis::Range& range) {
            convert(range.start);
            convert(range.end);
        };
        for (auto& scope : analysis->scopes)
            convert_range(scope.range);
        for (auto& symbol : analysis->symbols) {
            convert_range(symbol.range);
            convert_range(symbol.full);
        }
        for (auto& reference : analysis->references)
            convert_range(reference.range);
        for (auto& statement : analysis->statements)
            convert(statement.position);
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
