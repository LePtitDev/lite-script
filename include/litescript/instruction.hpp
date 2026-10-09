#ifndef LITESCRIPT_INSTRUCTION_HPP
#define LITESCRIPT_INSTRUCTION_HPP

#include <cstdint>
#include <string>
#include <string_view>
#include <variant>

namespace LiteScript {

    // Instruction codes (the order is used by the binary format of instructions)
    enum class InstrCode : std::uint8_t {
        // INVALID
        Invalid,
        // DEFINITIONS
        DefineVariable,
        DefineArg,
        DefineReturn,
        // VALUE CREATIONS
        ValuePop,
        ValueCopy,
        ValueAssign,
        ValueUndefined,
        ValueNull,
        ValueBoolean,
        ValueNumber,
        ValueString,
        ValueCallback,
        ValueArray,
        ValueClass,
        ValueObject,
        ValueArg,
        ValueArgs,
        ValueThis,
        ValueVariable,
        // STACKS MANAGEMENT
        PushNamespace,
        PushArgs,
        PopNamespace,
        PopArgs,
        Return,
        // OPERATIONS
        OpAssign,
        OpUnaryPlus,
        OpUnaryMinus,
        OpPreIncrement,
        OpPostIncrement,
        OpPreDecrement,
        OpPostDecrement,
        OpAdd,
        OpSubtract,
        OpMultiply,
        OpDivide,
        OpModulo,
        OpEqual,
        OpNotEqual,
        OpGreater,
        OpLess,
        OpGreaterOrEqual,
        OpLessOrEqual,
        OpLogicalNot,
        OpLogicalAnd,
        OpLogicalOr,
        OpBitwiseNot,
        OpBitwiseAnd,
        OpBitwiseOr,
        OpBitwiseXor,
        OpLeftShift,
        OpRightShift,
        OpAddAssign,
        OpSubtractAssign,
        OpMultiplyAssign,
        OpDivideAssign,
        OpArray,
        OpMember,
        OpCall,
        // CONTROL INSTRUCTIONS
        JumpTo,
        JumpIf,
        JumpElse,
        // COMPLEX VALUES COMPLETION
        ArrayPushNumeric,
        ArrayPushLiteral,
        ClassPushStatic,
        ClassPushUnstatic,
        ClassPushOperator,
        ClassInherit,
        ClassConstructor,
        // NAMESPACES
        NamespaceUse,
        // ERRORS
        TryBegin,
        TryEnd,
        Throw
    };

    // Number of instruction codes
    constexpr std::size_t INSTRUCTION_COUNT = static_cast<std::size_t>(InstrCode::Throw) + 1;

    // A position in a source code (the line and the column start at 0, the column is counted in characters)
    struct SourcePosition {
        std::uint32_t line = 0;
        std::uint32_t column = 0;

        bool operator==(const SourcePosition& p) const = default;
    };

    // The complement value of an instruction
    using Operand = std::variant<std::monostate, bool, std::int64_t, double, std::string>;

    // A reduced instruction
    struct Instruction {

        // The instruction code
        InstrCode code = InstrCode::Invalid;

        // The complement value
        Operand operand;

        // The position of the code that generated the instruction (used by the errors)
        SourcePosition position;

        Instruction() = default;
        Instruction(InstrCode c) : code(c) {}
        Instruction(InstrCode c, Operand value) : code(c), operand(std::move(value)) {}
        Instruction(InstrCode c, bool value) : code(c), operand(value) {}
        Instruction(InstrCode c, int value) : code(c), operand(std::int64_t{ value }) {}
        Instruction(InstrCode c, std::int64_t value) : code(c), operand(value) {}
        Instruction(InstrCode c, std::size_t value) : code(c), operand(static_cast<std::int64_t>(value)) {}
        Instruction(InstrCode c, double value) : code(c), operand(value) {}
        Instruction(InstrCode c, std::string value) : code(c), operand(std::move(value)) {}
        Instruction(InstrCode c, std::string_view value) : code(c), operand(std::string(value)) {}
        Instruction(InstrCode c, const char * value) : code(c), operand(std::string(value)) {}

        bool HasBoolean() const noexcept { return std::holds_alternative<bool>(this->operand); }
        bool HasInteger() const noexcept { return std::holds_alternative<std::int64_t>(this->operand); }
        bool HasFloat() const noexcept { return std::holds_alternative<double>(this->operand); }
        bool HasString() const noexcept { return std::holds_alternative<std::string>(this->operand); }

        bool GetBoolean() const { return std::get<bool>(this->operand); }
        std::int64_t GetInteger() const { return std::get<std::int64_t>(this->operand); }
        double GetFloat() const { return std::get<double>(this->operand); }
        const std::string& GetString() const { return std::get<std::string>(this->operand); }

        /**
         * Compare the code and the operand (the position is ignored)
         */
        bool operator==(const Instruction& i) const { return this->code == i.code && this->operand == i.operand; }

    };

}

#endif //LITESCRIPT_INSTRUCTION_HPP
