#include <litescript/assembly.hpp>

#include "charconv.hpp"

#include <array>
#include <charconv>
#include <cmath>

namespace {

    using LiteScript::Assembly;
    using LiteScript::InstrCode;
    using LiteScript::Instruction;

    // Kinds of arguments of the commands
    enum class Argument {
        None,
        Boolean,
        Integer,
        Number,
        String,
        Line
    };

    struct Command {
        std::string_view name;
        InstrCode code;
        Argument argument;
    };

    // The commands, in the order of the instruction codes
    constexpr std::array<Command, LiteScript::INSTRUCTION_COUNT> COMMANDS{ {
        { "", InstrCode::Invalid, Argument::None },
        { "define.variable", InstrCode::DefineVariable, Argument::String },
        { "define.arg", InstrCode::DefineArg, Argument::Integer },
        { "define.return", InstrCode::DefineReturn, Argument::None },
        { "value.pop", InstrCode::ValuePop, Argument::None },
        { "value.copy", InstrCode::ValueCopy, Argument::None },
        { "value.assign", InstrCode::ValueAssign, Argument::None },
        { "value.undefined", InstrCode::ValueUndefined, Argument::None },
        { "value.null", InstrCode::ValueNull, Argument::None },
        { "value.boolean", InstrCode::ValueBoolean, Argument::Boolean },
        { "value.number", InstrCode::ValueNumber, Argument::Number },
        { "value.string", InstrCode::ValueString, Argument::String },
        { "value.callback", InstrCode::ValueCallback, Argument::Line },
        { "value.array", InstrCode::ValueArray, Argument::None },
        { "value.class", InstrCode::ValueClass, Argument::None },
        { "value.object", InstrCode::ValueObject, Argument::None },
        { "value.arg", InstrCode::ValueArg, Argument::Integer },
        { "value.args", InstrCode::ValueArgs, Argument::Integer },
        { "value.this", InstrCode::ValueThis, Argument::None },
        { "value.variable", InstrCode::ValueVariable, Argument::String },
        { "push.nsp", InstrCode::PushNamespace, Argument::None },
        { "push.args", InstrCode::PushArgs, Argument::None },
        { "pop.nsp", InstrCode::PopNamespace, Argument::None },
        { "pop.args", InstrCode::PopArgs, Argument::None },
        { "return", InstrCode::Return, Argument::None },
        { "operator.assign", InstrCode::OpAssign, Argument::None },
        { "operator.unary-plus", InstrCode::OpUnaryPlus, Argument::None },
        { "operator.unary-minus", InstrCode::OpUnaryMinus, Argument::None },
        { "operator.pre-incr", InstrCode::OpPreIncrement, Argument::None },
        { "operator.post-incr", InstrCode::OpPostIncrement, Argument::None },
        { "operator.pre-decr", InstrCode::OpPreDecrement, Argument::None },
        { "operator.post-decr", InstrCode::OpPostDecrement, Argument::None },
        { "operator.add", InstrCode::OpAdd, Argument::None },
        { "operator.sub", InstrCode::OpSubtract, Argument::None },
        { "operator.mul", InstrCode::OpMultiply, Argument::None },
        { "operator.div", InstrCode::OpDivide, Argument::None },
        { "operator.mod", InstrCode::OpModulo, Argument::None },
        { "operator.equ", InstrCode::OpEqual, Argument::None },
        { "operator.dif", InstrCode::OpNotEqual, Argument::None },
        { "operator.great", InstrCode::OpGreater, Argument::None },
        { "operator.less", InstrCode::OpLess, Argument::None },
        { "operator.great-equ", InstrCode::OpGreaterOrEqual, Argument::None },
        { "operator.less-equ", InstrCode::OpLessOrEqual, Argument::None },
        { "operator.log-not", InstrCode::OpLogicalNot, Argument::None },
        { "operator.log-and", InstrCode::OpLogicalAnd, Argument::None },
        { "operator.log-or", InstrCode::OpLogicalOr, Argument::None },
        { "operator.bit-not", InstrCode::OpBitwiseNot, Argument::None },
        { "operator.bit-and", InstrCode::OpBitwiseAnd, Argument::None },
        { "operator.bit-or", InstrCode::OpBitwiseOr, Argument::None },
        { "operator.bit-xor", InstrCode::OpBitwiseXor, Argument::None },
        { "operator.lshift", InstrCode::OpLeftShift, Argument::None },
        { "operator.rshift", InstrCode::OpRightShift, Argument::None },
        { "operator.add-assign", InstrCode::OpAddAssign, Argument::None },
        { "operator.sub-assign", InstrCode::OpSubtractAssign, Argument::None },
        { "operator.mul-assign", InstrCode::OpMultiplyAssign, Argument::None },
        { "operator.div-assign", InstrCode::OpDivideAssign, Argument::None },
        { "operator.array", InstrCode::OpArray, Argument::None },
        { "operator.member", InstrCode::OpMember, Argument::String },
        { "operator.call", InstrCode::OpCall, Argument::None },
        { "jump-to", InstrCode::JumpTo, Argument::Line },
        { "jump-if", InstrCode::JumpIf, Argument::Line },
        { "jump-else", InstrCode::JumpElse, Argument::Line },
        { "array.push-numeric", InstrCode::ArrayPushNumeric, Argument::Integer },
        { "array.push-literal", InstrCode::ArrayPushLiteral, Argument::String },
        { "class.push-static", InstrCode::ClassPushStatic, Argument::String },
        { "class.push-unstatic", InstrCode::ClassPushUnstatic, Argument::String },
        { "class.push-operator", InstrCode::ClassPushOperator, Argument::Integer },
        { "class.inherit", InstrCode::ClassInherit, Argument::None },
        { "class.constructor", InstrCode::ClassConstructor, Argument::String },
        { "namespace", InstrCode::NamespaceUse, Argument::String },
        { "try.begin", InstrCode::TryBegin, Argument::Line },
        { "try.end", InstrCode::TryEnd, Argument::None },
        { "throw", InstrCode::Throw, Argument::None }
    } };

    // Check at compile time that the table follows the order of the codes
    constexpr bool CheckCommandsOrder() {
        for (std::size_t i = 0; i < COMMANDS.size(); i++) {
            if (static_cast<std::size_t>(COMMANDS[i].code) != i)
                return false;
        }
        return true;
    }
    static_assert(CheckCommandsOrder(), "The commands table must follow the order of LiteScript::InstrCode");

    bool IsBlank(char c) {
        return c == ' ' || c == '\t' || c == '\r' || c == '\n';
    }

    std::string_view Trim(std::string_view s) {
        while (!s.empty() && IsBlank(s.front()))
            s.remove_prefix(1);
        while (!s.empty() && IsBlank(s.back()))
            s.remove_suffix(1);
        return s;
    }

    // Read an unsigned integer that covers the whole text
    bool ParseUnsigned(std::string_view text, std::int64_t& value) {
        if (text.empty() || text.front() < '0' || text.front() > '9')
            return false;
        auto result = std::from_chars(text.data(), text.data() + text.size(), value);
        return result.ec == std::errc() && result.ptr == text.data() + text.size();
    }

    // Read a number that covers the whole text
    bool ParseNumber(std::string_view text, LiteScript::Operand& operand) {
        if (text.empty())
            return false;
        std::string_view digits = text.front() == '+' ? text.substr(1) : text;
        if (digits.empty() || digits.front() == '+')
            return false;
        std::int64_t integer;
        auto ir = std::from_chars(digits.data(), digits.data() + digits.size(), integer);
        if (ir.ec == std::errc() && ir.ptr == digits.data() + digits.size()) {
            operand = integer;
            return true;
        }
        double value;
        if (!LiteScript::Internal::ParseDouble(digits, value) || !std::isfinite(value))
            return false;
        operand = value;
        return true;
    }

    // Decode the escape sequences of a string argument (\\, \n, \r, \t, \0)
    std::string Unescape(std::string_view text) {
        std::string res;
        res.reserve(text.size());
        for (std::size_t i = 0; i < text.size(); i++) {
            if (text[i] != '\\' || i + 1 == text.size()) {
                res += text[i];
                continue;
            }
            switch (text[++i]) {
                case 'n': res += '\n'; break;
                case 'r': res += '\r'; break;
                case 't': res += '\t'; break;
                case '0': res += '\0'; break;
                default: res += text[i]; break;
            }
        }
        return res;
    }

    // Encode the characters that can't be written on a single line
    std::string Escape(std::string_view text) {
        std::string res;
        res.reserve(text.size());
        for (char c : text) {
            switch (c) {
                case '\\': res += "\\\\"; break;
                case '\n': res += "\\n"; break;
                case '\r': res += "\\r"; break;
                case '\t': res += "\\t"; break;
                case '\0': res += "\\0"; break;
                default: res += c; break;
            }
        }
        return res;
    }

}

LiteScript::Instruction LiteScript::Assembly::CompileInstruction(std::string_view line, Error& error) {
    error = Error::None;
    // Remove the line break and the leading spaces (string arguments keep their trailing spaces)
    while (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
        line.remove_suffix(1);
    while (!line.empty() && IsBlank(line.front()))
        line.remove_prefix(1);
    if (Trim(line).empty())
        return Instruction(InstrCode::Invalid);

    std::size_t space = line.find_first_of(" \t");
    std::string_view name = line.substr(0, space);
    bool has_argument = space != std::string_view::npos;
    std::string_view raw_argument = has_argument ? line.substr(space + 1) : std::string_view();
    std::string_view argument = Trim(raw_argument);

    for (const Command& command : COMMANDS) {
        if (command.code == InstrCode::Invalid || command.name != name)
            continue;
        switch (command.argument) {
            case Argument::None:
                return Instruction(command.code);
            case Argument::Boolean:
                if (argument == "true")
                    return Instruction(command.code, true);
                if (argument == "false")
                    return Instruction(command.code, false);
                error = Error::ExpectedBoolean;
                return Instruction(InstrCode::Invalid);
            case Argument::Integer: {
                std::int64_t value;
                if (ParseUnsigned(argument, value))
                    return Instruction(command.code, value);
                error = Error::ExpectedInteger;
                return Instruction(InstrCode::Invalid);
            }
            case Argument::Line: {
                std::int64_t value;
                if (ParseUnsigned(argument, value) && value >= 1)
                    return Instruction(command.code, value - 1);
                error = Error::ExpectedInteger;
                return Instruction(InstrCode::Invalid);
            }
            case Argument::Number: {
                Operand operand;
                if (ParseNumber(argument, operand))
                    return Instruction{ command.code, std::move(operand) };
                error = Error::ExpectedNumber;
                return Instruction(InstrCode::Invalid);
            }
            case Argument::String:
                // The string is the rest of the line after the space
                if (has_argument && (command.code == InstrCode::ValueString || !argument.empty()))
                    return Instruction(command.code, command.code == InstrCode::ValueString ? Unescape(raw_argument) : Unescape(argument));
                error = Error::ExpectedString;
                return Instruction(InstrCode::Invalid);
        }
    }
    error = Error::UnknownCommand;
    return Instruction(InstrCode::Invalid);
}

LiteScript::Assembly::Result LiteScript::Assembly::Compile(std::string_view code) {
    Result result;
    std::size_t line = 0;
    while (true) {
        std::size_t end = code.find('\n');
        result.instructions.push_back(CompileInstruction(code.substr(0, end), result.error));
        result.instructions.back().position.line = static_cast<std::uint32_t>(line);
        if (result.error != Error::None) {
            result.instructions.clear();
            result.line = line;
            return result;
        }
        if (end == std::string_view::npos)
            break;
        code.remove_prefix(end + 1);
        line++;
    }
    // The end of the code (empty code or line break at the end) doesn't create an instruction
    if (code.empty())
        result.instructions.pop_back();
    return result;
}

std::string LiteScript::Assembly::DisassembleInstruction(const Instruction& instruction) {
    auto index = static_cast<std::size_t>(instruction.code);
    if (index >= COMMANDS.size() || instruction.code == InstrCode::Invalid)
        return std::string();
    const Command& command = COMMANDS[index];
    std::string res(command.name);
    switch (command.argument) {
        case Argument::None:
            break;
        case Argument::Boolean:
            res += instruction.HasBoolean() && instruction.GetBoolean() ? " true" : " false";
            break;
        case Argument::Integer:
            res += ' ' + std::to_string(instruction.HasInteger() ? instruction.GetInteger() : 0);
            break;
        case Argument::Line:
            res += ' ' + std::to_string(instruction.HasInteger() ? instruction.GetInteger() + 1 : 0);
            break;
        case Argument::Number:
            if (instruction.HasFloat()) {
                std::string text = LiteScript::Internal::FormatDouble(instruction.GetFloat());
                res += ' ';
                res += text;
                // An integral float keeps its type when it's read again
                if (text.find_first_of(".en") == std::string_view::npos)
                    res += ".0";
            }
            else {
                res += ' ' + std::to_string(instruction.HasInteger() ? instruction.GetInteger() : 0);
            }
            break;
        case Argument::String:
            res += ' ';
            if (instruction.HasString())
                res += Escape(instruction.GetString());
            break;
    }
    return res;
}

std::string LiteScript::Assembly::Disassemble(const std::vector<Instruction>& instructions) {
    std::string res;
    for (const Instruction& instruction : instructions) {
        res += DisassembleInstruction(instruction);
        res += '\n';
    }
    return res;
}

LiteScript::Assembly::Result LiteScript::Assembly::Execute(State& state, std::string_view code) {
    Result result = Compile(code);
    if (result) {
        state.AddInstructions(result.instructions);
        result.value = state.Execute();
    }
    return result;
}

LiteScript::Assembly::Result LiteScript::Assembly::ExecuteAfter(State& state, std::string_view code) {
    Result result = Compile(code);
    if (result) {
        for (const Instruction& instruction : result.instructions)
            state.AddInstruction(instruction);
        result.value = state.Execute();
    }
    return result;
}

std::string_view LiteScript::Assembly::GetErrorMessage(Error error) noexcept {
    switch (error) {
        case Error::None: return "No error";
        case Error::UnknownCommand: return "Unknown assembly command";
        case Error::ExpectedBoolean: return "Boolean argument expected";
        case Error::ExpectedInteger: return "Unsigned integer argument expected";
        case Error::ExpectedNumber: return "Number argument expected";
        case Error::ExpectedString: return "String argument expected";
    }
    return "Unknown error";
}
