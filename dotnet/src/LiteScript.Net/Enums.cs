namespace LiteScript
{
    /// <summary>
    /// Types of the values (LiteScript::Type)
    /// </summary>
    public enum ScriptType
    {
        Undefined,
        Null,
        Boolean,
        Number,
        String,
        Character,
        Callback,
        Array,
        Class,
        ClassObject,
        Namespace
    }

    /// <summary>
    /// Operators that a class can overload (LiteScript::Operator)
    /// </summary>
    public enum Operator
    {
        Assign,
        UnaryPlus,
        UnaryMinus,
        PreIncrement,
        PostIncrement,
        PreDecrement,
        PostDecrement,
        Add,
        Subtract,
        Multiply,
        Divide,
        Modulo,
        Equal,
        NotEqual,
        Greater,
        Less,
        GreaterOrEqual,
        LessOrEqual,
        LogicalNot,
        LogicalAnd,
        LogicalOr,
        BitwiseNot,
        BitwiseAnd,
        BitwiseOr,
        BitwiseXor,
        LeftShift,
        RightShift,
        AddAssign,
        SubtractAssign,
        MultiplyAssign,
        DivideAssign,
        Array,
        Call
    }

    /// <summary>
    /// Instruction codes (LiteScript::InstrCode, the order is used by the binary format of instructions)
    /// </summary>
    public enum InstrCode
    {
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
    }

    /// <summary>
    /// The result of a comparison of values (std::partial_ordering)
    /// </summary>
    public enum ValueOrdering
    {
        Less = -1,
        Equivalent = 0,
        Greater = 1,
        /// <summary>The values can't be compared</summary>
        Unordered = 2
    }
}
