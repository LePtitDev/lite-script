using System;
using System.Collections.Generic;
using LiteScript.Interop;

namespace LiteScript
{
    /// <summary>
    /// A call to an overloaded operator of a class that must be executed by a state (LiteScript::OverloadCall)
    /// </summary>
    public sealed class OverloadCall
    {
        internal OverloadCall(Variable? callback, Variable? self, IReadOnlyList<Variable?> args)
        {
            this.Callback = callback;
            this.Self = self;
            this.Args = args;
        }

        /// <summary>
        /// The callback of the operator
        /// </summary>
        public Variable? Callback { get; }

        /// <summary>
        /// The instance (value of "this")
        /// </summary>
        public Variable? Self { get; }

        /// <summary>
        /// The arguments
        /// </summary>
        public IReadOnlyList<Variable?> Args { get; }
    }

    /// <summary>
    /// The result of an operation: a value or a call to execute (LiteScript::OperationResult)
    /// </summary>
    public sealed class OperationResult
    {
        internal OperationResult(Variable? value, OverloadCall? call)
        {
            this.Value = value;
            this.Call = call;
        }

        /// <summary>
        /// The value (null if the operation is a call of an overloaded operator)
        /// </summary>
        public Variable? Value { get; }

        /// <summary>
        /// The call of an overloaded operator (null if the result is a value)
        /// </summary>
        public OverloadCall? Call { get; }

        /// <summary>
        /// Indicate if the operation is a call of an overloaded operator
        /// </summary>
        public bool IsOverloadCall => this.Call != null;
    }

    /// <summary>
    /// Manipulation of values with the semantic of the language (functions of operations.hpp)
    /// </summary>
    public static class Operations
    {
        /// <summary>
        /// Indicate if a value is considered as true in a condition
        /// (false for undefined, null, false, 0, NaN and empty strings)
        /// </summary>
        public static bool IsTruthy(Variable? v)
        {
            Native.EnsureInitialized();
            bool truthy = NativeMethods.ls_is_truthy(Memory.HandleOf(v)) != 0;
            GC.KeepAlive(v);
            return truthy;
        }

        /// <summary>
        /// Return a string that describes the content of an object (strings are quoted)
        /// </summary>
        public static string ToString(Variable? v)
        {
            return v?.ToString() ?? "undefined";
        }

        /// <summary>
        /// Return the text of a value: the content of strings and characters, the description of the other types
        /// </summary>
        public static string ToText(Variable? v)
        {
            return v?.ToText() ?? "undefined";
        }

        /// <summary>
        /// Create a new object that contains a copy of a value (this is the semantic of the assignment):
        /// arrays, instances, classes and namespaces are shared (the new object references them),
        /// characters become strings, the other values are copied
        /// </summary>
        public static Variable Copy(Memory memory, Variable? v)
        {
            memory.Check(NativeMethods.ls_copy(memory.Handle, Memory.HandleOf(v), out IntPtr result));
            GC.KeepAlive(v);
            return memory.Wrap(result);
        }

        /// <summary>
        /// Create a new independent value: a new array with copies of the elements, a new instance or
        /// namespace with the same members (the other values are copied like Copy)
        /// </summary>
        public static Variable Clone(Memory memory, Variable? v)
        {
            memory.Check(NativeMethods.ls_clone(memory.Handle, Memory.HandleOf(v), out IntPtr result));
            GC.KeepAlive(v);
            return memory.Wrap(result);
        }

        /// <summary>
        /// Convert a value to another type
        /// </summary>
        /// <returns>The converted value or a null object if the conversion isn't possible</returns>
        public static Variable Convert(Memory memory, Variable? v, ScriptType type)
        {
            memory.Check(NativeMethods.ls_convert(memory.Handle, Memory.HandleOf(v), (int)type, out IntPtr result));
            GC.KeepAlive(v);
            return memory.Wrap(result);
        }

        /// <summary>
        /// Apply an operator (every operator except Operator.Call).
        /// Unary operators ignore the second operand.
        /// If the first operand is an instance of a class that overloads the operator, an OverloadCall is returned.
        /// </summary>
        /// <param name="memory">The memory where results are created</param>
        /// <param name="op">The operator</param>
        /// <param name="a">The first operand (modified by assignment operators)</param>
        /// <param name="b">The second operand</param>
        public static OperationResult Operate(Memory memory, Operator op, Variable? a, Variable? b = null)
        {
            memory.Check(NativeMethods.ls_operate(memory.Handle, (int)op, Memory.HandleOf(a), Memory.HandleOf(b), out IntPtr value,
                                                  out IntPtr callback, out IntPtr self, out IntPtr args, out UIntPtr argsSize));
            GC.KeepAlive(a);
            GC.KeepAlive(b);
            if (args == IntPtr.Zero)
                return new OperationResult(memory.WrapOptional(value), null);
            List<Variable?> arguments = new BufferReader(args, argsSize, memory).Variables();
            return new OperationResult(null, new OverloadCall(memory.WrapOptional(callback), memory.WrapOptional(self), arguments));
        }

        /// <summary>
        /// Apply an operator and execute the call of an overloaded operator with a state
        /// </summary>
        /// <returns>The result of the operation</returns>
        public static Variable Operate(State state, Operator op, Variable? a, Variable? b = null)
        {
            OperationResult result = Operate(state.Memory, op, a, b);
            if (result.Call == null)
                return result.Value ?? state.Memory.CreateUndefined();
            OverloadCall call = result.Call;
            if (call.Callback == null)
                return state.Memory.CreateUndefined();
            // The callback is bound to the instance
            using (Variable callback = Copy(state.Memory, call.Callback))
            {
                callback.AsCallback().Self = call.Self;
                var args = new Variable?[call.Args.Count];
                for (int i = 0; i < args.Length; i++)
                    args[i] = call.Args[i];
                return state.Call(callback, args);
            }
        }

        /// <summary>
        /// Return a member of a value ("a.name"), methods are bound to the value
        /// </summary>
        /// <returns>The member or an undefined object if it doesn't exist (assigning a value to this
        /// object creates the member in arrays, instances, classes and namespaces)</returns>
        public static Variable GetMember(Memory memory, Variable? v, string name)
        {
            memory.Check(NativeMethods.ls_get_member(memory.Handle, Memory.HandleOf(v), Utf8.Encode(name), out IntPtr result));
            GC.KeepAlive(v);
            return memory.Wrap(result);
        }

        /// <summary>
        /// Indicate if an operator takes two operands
        /// </summary>
        public static bool IsBinary(Operator op)
        {
            Native.EnsureInitialized();
            return NativeMethods.ls_is_binary((int)op) != 0;
        }

        /// <summary>
        /// Compare two values with the semantic of "==" (without overloaded operators)
        /// </summary>
        public static bool ValuesEqual(Variable? a, Variable? b)
        {
            Native.EnsureInitialized();
            Native.Check(NativeMethods.ls_values_equal(Memory.HandleOf(a), Memory.HandleOf(b), out int equal));
            GC.KeepAlive(a);
            GC.KeepAlive(b);
            return equal != 0;
        }

        /// <summary>
        /// Compare two values with the semantic of "&lt;" and "&gt;" (without overloaded operators)
        /// </summary>
        /// <returns>Unordered if the values can't be compared</returns>
        public static ValueOrdering CompareValues(Variable? a, Variable? b)
        {
            Native.EnsureInitialized();
            Native.Check(NativeMethods.ls_compare_values(Memory.HandleOf(a), Memory.HandleOf(b), out int order));
            GC.KeepAlive(a);
            GC.KeepAlive(b);
            return (ValueOrdering)order;
        }
    }
}
