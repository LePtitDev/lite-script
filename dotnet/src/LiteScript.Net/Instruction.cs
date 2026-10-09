using System;
using System.Globalization;

namespace LiteScript
{
    /// <summary>
    /// A position in a source code (the line and the column start at 0, the column is counted in characters)
    /// </summary>
    public readonly struct SourcePosition : IEquatable<SourcePosition>
    {
        public SourcePosition(uint line, uint column)
        {
            this.Line = line;
            this.Column = column;
        }

        public uint Line { get; }

        public uint Column { get; }

        public bool Equals(SourcePosition other) => this.Line == other.Line && this.Column == other.Column;
        public override bool Equals(object? obj) => obj is SourcePosition p && this.Equals(p);
        public override int GetHashCode() => unchecked((int)(this.Line * 397) ^ (int)this.Column);
        public static bool operator ==(SourcePosition a, SourcePosition b) => a.Equals(b);
        public static bool operator !=(SourcePosition a, SourcePosition b) => !a.Equals(b);
        public override string ToString() => string.Format(CultureInfo.InvariantCulture, "{0}:{1}", this.Line, this.Column);
    }

    /// <summary>
    /// A reduced instruction (LiteScript::Instruction)
    /// </summary>
    public sealed class Instruction : IEquatable<Instruction>
    {
        private object? operand;

        public Instruction()
        {
        }

        public Instruction(InstrCode code)
        {
            this.Code = code;
        }

        /// <param name="code">The instruction code</param>
        /// <param name="operand">The complement value: null, a bool, a long (or an int), a double or a string</param>
        public Instruction(InstrCode code, object? operand)
        {
            this.Code = code;
            this.Operand = operand;
        }

        public Instruction(InstrCode code, bool value) : this(code, (object)value) { }
        public Instruction(InstrCode code, int value) : this(code, (object)(long)value) { }
        public Instruction(InstrCode code, long value) : this(code, (object)value) { }
        public Instruction(InstrCode code, double value) : this(code, (object)value) { }
        public Instruction(InstrCode code, string value) : this(code, (object)value) { }

        /// <summary>
        /// The instruction code
        /// </summary>
        public InstrCode Code { get; set; }

        /// <summary>
        /// The complement value: null, a bool, a long, a double or a string (an int is stored as a long)
        /// </summary>
        public object? Operand
        {
            get => this.operand;
            set
            {
                switch (value)
                {
                    case null:
                    case bool _:
                    case long _:
                    case double _:
                    case string _:
                        this.operand = value;
                        break;
                    case int i:
                        this.operand = (long)i;
                        break;
                    default:
                        throw new ArgumentException("the operand must be null, a bool, a long, a double or a string", nameof(value));
                }
            }
        }

        /// <summary>
        /// The position of the code that generated the instruction (used by the errors)
        /// </summary>
        public SourcePosition Position { get; set; }

        public bool HasBoolean => this.operand is bool;
        public bool HasInteger => this.operand is long;
        public bool HasFloat => this.operand is double;
        public bool HasString => this.operand is string;

        public bool GetBoolean() => this.operand is bool b ? b : throw new InvalidCastException("the operand isn't a boolean");
        public long GetInteger() => this.operand is long i ? i : throw new InvalidCastException("the operand isn't an integer");
        public double GetFloat() => this.operand is double d ? d : throw new InvalidCastException("the operand isn't a float");
        public string GetString() => this.operand is string s ? s : throw new InvalidCastException("the operand isn't a string");

        /// <summary>
        /// Compare the code and the operand (the position is ignored)
        /// </summary>
        public bool Equals(Instruction? other)
        {
            if (other is null)
                return false;
            return this.Code == other.Code && Equals(this.operand, other.operand);
        }

        public override bool Equals(object? obj) => this.Equals(obj as Instruction);

        public override int GetHashCode() => ((int)this.Code * 397) ^ (this.operand?.GetHashCode() ?? 0);

        /// <summary>
        /// Return the line of assembly code of the instruction
        /// </summary>
        public override string ToString() => Assembly.DisassembleInstruction(this);
    }
}
