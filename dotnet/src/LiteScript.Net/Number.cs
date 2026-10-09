using System;
using LiteScript.Interop;

namespace LiteScript
{
    /// <summary>
    /// Content of a number: a 64 bits integer or a double precision float (LiteScript::Number).
    /// Integer operations stay integer when the result is an integer (6 / 2 == 3, 5 / 2 == 2.5),
    /// the other operations are done in floating point. The default value is the integer 0.
    /// </summary>
    public readonly struct Number : IEquatable<Number>
    {
        private const double IntLimit = 9223372036854775808.0;

        private readonly long integer;
        private readonly double floating;
        private readonly bool isFloat;

        /// <summary>
        /// Integer constructor
        /// </summary>
        public Number(long value)
        {
            this.integer = value;
            this.floating = 0;
            this.isFloat = false;
        }

        /// <summary>
        /// Floating constructor (an integral value is stored as an integer)
        /// </summary>
        public Number(double value)
        {
            if (!double.IsNaN(value) && !double.IsInfinity(value) && Math.Truncate(value) == value && value >= -IntLimit && value < IntLimit)
            {
                this.integer = (long)value;
                this.floating = 0;
                this.isFloat = false;
            }
            else
            {
                this.integer = 0;
                this.floating = value;
                this.isFloat = true;
            }
        }

        internal Number(in NativeNumber n)
        {
            this.integer = n.Integer;
            this.floating = n.Floating;
            this.isFloat = n.IsInteger == 0;
        }

        internal NativeNumber ToNative()
        {
            return new NativeNumber
            {
                Integer = this.integer,
                Floating = this.isFloat ? this.floating : this.integer,
                IsInteger = this.isFloat ? 0 : 1
            };
        }

        /// <summary>
        /// Indicate if the number is represented by an integer
        /// </summary>
        public bool IsInteger => !this.isFloat;

        /// <summary>
        /// Return the number as an integer (truncated, saturated, NaN gives 0)
        /// </summary>
        public long ToInteger()
        {
            if (!this.isFloat)
                return this.integer;
            double d = this.floating;
            if (double.IsNaN(d))
                return 0;
            if (d >= IntLimit)
                return long.MaxValue;
            if (d < -IntLimit)
                return long.MinValue;
            return (long)d;
        }

        /// <summary>
        /// Return the number as a floating number
        /// </summary>
        public double ToFloat()
        {
            return this.isFloat ? this.floating : this.integer;
        }

        /// <summary>
        /// Return the shortest text that represents the number
        /// </summary>
        public override string ToString()
        {
            NativeNumber n = this.ToNative();
            Native.EnsureInitialized();
            Native.Check(NativeMethods.ls_number_to_string(in n, out IntPtr text));
            return Utf8.Take(text);
        }

        private static Number Unary(Operator op, Number a)
        {
            NativeNumber x = a.ToNative();
            Native.EnsureInitialized();
            Native.Check(NativeMethods.ls_number_unary((int)op, in x, out NativeNumber result));
            return new Number(in result);
        }

        private static Number Binary(Operator op, Number a, Number b)
        {
            NativeNumber x = a.ToNative(), y = b.ToNative();
            Native.EnsureInitialized();
            Native.Check(NativeMethods.ls_number_binary((int)op, in x, in y, out NativeNumber result));
            return new Number(in result);
        }

        private static bool Compare(Operator op, Number a, Number b)
        {
            NativeNumber x = a.ToNative(), y = b.ToNative();
            Native.EnsureInitialized();
            return NativeMethods.ls_number_compare((int)op, in x, in y) != 0;
        }

        ////// CONVERSIONS //////

        public static implicit operator Number(int value) => new Number(value);
        public static implicit operator Number(long value) => new Number(value);
        public static implicit operator Number(double value) => new Number(value);
        public static explicit operator long(Number value) => value.ToInteger();
        public static explicit operator double(Number value) => value.ToFloat();

        ////// ARITHMETIC OPERATIONS //////

        public static Number operator +(Number a) => a;
        public static Number operator -(Number a) => Unary(Operator.UnaryMinus, a);
        public static Number operator ++(Number a) => Unary(Operator.PreIncrement, a);
        public static Number operator --(Number a) => Unary(Operator.PreDecrement, a);
        public static Number operator +(Number a, Number b) => Binary(Operator.Add, a, b);
        public static Number operator -(Number a, Number b) => Binary(Operator.Subtract, a, b);
        public static Number operator *(Number a, Number b) => Binary(Operator.Multiply, a, b);
        public static Number operator /(Number a, Number b) => Binary(Operator.Divide, a, b);
        public static Number operator %(Number a, Number b) => Binary(Operator.Modulo, a, b);

        ////// COMPARISON OPERATIONS //////

        public static bool operator ==(Number a, Number b) => Compare(Operator.Equal, a, b);
        public static bool operator !=(Number a, Number b) => !(a == b);
        public static bool operator <(Number a, Number b) => Compare(Operator.Less, a, b);
        public static bool operator >(Number a, Number b) => b < a;
        public static bool operator <=(Number a, Number b) => a < b || a == b;
        public static bool operator >=(Number a, Number b) => b < a || a == b;

        ////// BINARY OPERATIONS (on the integer representation) //////

        public static Number operator ~(Number a) => Unary(Operator.BitwiseNot, a);
        public static Number operator &(Number a, Number b) => Binary(Operator.BitwiseAnd, a, b);
        public static Number operator |(Number a, Number b) => Binary(Operator.BitwiseOr, a, b);
        public static Number operator ^(Number a, Number b) => Binary(Operator.BitwiseXor, a, b);
        public static Number LeftShift(Number a, Number b) => Binary(Operator.LeftShift, a, b);
        public static Number RightShift(Number a, Number b) => Binary(Operator.RightShift, a, b);
        public static Number operator <<(Number a, int b) => LeftShift(a, b);
        public static Number operator >>(Number a, int b) => RightShift(a, b);

        /// <inheritdoc/>
        public bool Equals(Number other) => this == other;

        /// <inheritdoc/>
        public override bool Equals(object? obj) => obj is Number n && this == n;

        /// <inheritdoc/>
        public override int GetHashCode() => this.isFloat ? this.floating.GetHashCode() : ((double)this.integer).GetHashCode();
    }
}
