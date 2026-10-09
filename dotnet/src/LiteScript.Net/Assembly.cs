using System;
using System.Collections.Generic;
using LiteScript.Interop;

namespace LiteScript
{
    /// <summary>
    /// The assembly language: one instruction per line ("value.number 5", "jump-to 12"...) (LiteScript::Assembly).
    /// Line numbers used by jumps and callbacks start at 1.
    /// </summary>
    public static class Assembly
    {
        /// <summary>
        /// List of errors
        /// </summary>
        public enum Error
        {
            None,
            UnknownCommand,
            ExpectedBoolean,
            ExpectedInteger,
            ExpectedNumber,
            ExpectedString
        }

        /// <summary>
        /// The result of a compilation
        /// </summary>
        public sealed class Result
        {
            internal Result(Error error, int line, List<Instruction> instructions, Variable? value)
            {
                this.Error = error;
                this.Line = line;
                this.Instructions = instructions;
                this.Value = value;
            }

            /// <summary>
            /// The instructions (empty if there is an error)
            /// </summary>
            public List<Instruction> Instructions { get; }

            /// <summary>
            /// The error
            /// </summary>
            public Error Error { get; }

            /// <summary>
            /// The line of the error (starts at 0)
            /// </summary>
            public int Line { get; }

            /// <summary>
            /// The value on the top of the stack after the execution (for Execute and ExecuteAfter)
            /// </summary>
            public Variable? Value { get; }

            /// <summary>
            /// Indicate if there is no error
            /// </summary>
            public bool Success => this.Error == Error.None;

            /// <summary>
            /// Return the message of the error
            /// </summary>
            public string GetMessage() => GetErrorMessage(this.Error);

            public static implicit operator bool(Result result) => result != null && result.Success;
        }

        private static Result ReadResult(BufferReader r, Variable? value)
        {
            var error = (Error)r.U8();
            int line = r.Index();
            r.Index();
            return new Result(error, line, r.Instructions(), value);
        }

        /// <summary>
        /// Convert an assembly code into instructions
        /// (empty lines are converted into invalid instructions that do nothing)
        /// </summary>
        public static Result Compile(string code)
        {
            Native.EnsureInitialized();
            byte[] bytes = Utf8.EncodeData(code, out UIntPtr length);
            Native.Check(NativeMethods.ls_assembly_compile(bytes, length, out IntPtr buffer, out UIntPtr size));
            return ReadResult(new BufferReader(buffer, size), null);
        }

        /// <summary>
        /// Convert a line of assembly code into an instruction
        /// </summary>
        public static Instruction CompileInstruction(string line, out Error error)
        {
            Native.EnsureInitialized();
            byte[] bytes = Utf8.EncodeData(line, out UIntPtr length);
            Native.Check(NativeMethods.ls_assembly_compile_instruction(bytes, length, out int err, out IntPtr buffer, out UIntPtr size));
            error = (Error)err;
            return new BufferReader(buffer, size).Instruction();
        }

        /// <summary>
        /// Convert instructions into assembly code
        /// </summary>
        public static string Disassemble(IEnumerable<Instruction> instructions)
        {
            Native.EnsureInitialized();
            byte[] buffer = BufferWriter.FromInstructions(instructions);
            Native.Check(NativeMethods.ls_assembly_disassemble(buffer, (UIntPtr)buffer.Length, out IntPtr code));
            return Utf8.Take(code);
        }

        /// <summary>
        /// Convert an instruction into a line of assembly code (without line break)
        /// </summary>
        public static string DisassembleInstruction(Instruction instruction)
        {
            Native.EnsureInitialized();
            byte[] buffer = BufferWriter.FromInstruction(instruction);
            Native.Check(NativeMethods.ls_assembly_disassemble_instruction(buffer, (UIntPtr)buffer.Length, out IntPtr code));
            return Utf8.Take(code);
        }

        private static Result Run(State state, string code, bool after)
        {
            if (state == null)
                throw new ArgumentNullException(nameof(state));
            byte[] bytes = Utf8.EncodeData(code, out UIntPtr length);
            int status = NativeMethods.ls_assembly_execute(state.Handle, bytes, length, after ? 1 : 0, out IntPtr buffer, out UIntPtr size, out IntPtr value);
            GC.KeepAlive(state);
            state.Memory.Check(status);
            return ReadResult(new BufferReader(buffer, size), state.Memory.WrapOptional(value));
        }

        /// <summary>
        /// Compile a code, add it in a new list of the state and execute it
        /// </summary>
        /// <exception cref="ScriptException">An error isn't caught by the code</exception>
        public static Result Execute(State state, string code) => Run(state, code, false);

        /// <summary>
        /// Compile a code, add it at the end of the last list of the state and execute it
        /// (line numbers continue after the previous instructions of the list)
        /// </summary>
        /// <exception cref="ScriptException">An error isn't caught by the code</exception>
        public static Result ExecuteAfter(State state, string code) => Run(state, code, true);

        /// <summary>
        /// Return the message of an error
        /// </summary>
        public static string GetErrorMessage(Error error)
        {
            Native.EnsureInitialized();
            Native.Check(NativeMethods.ls_assembly_error_message((int)error, out IntPtr message));
            return Utf8.Take(message);
        }
    }

    /// <summary>
    /// Binary format of the instructions (functions of serialization.hpp)
    /// </summary>
    public static class Serialization
    {
        /// <summary>
        /// Return the binary format of instructions
        /// </summary>
        public static byte[] SaveInstructions(IEnumerable<Instruction> instructions)
        {
            Native.EnsureInitialized();
            byte[] buffer = BufferWriter.FromInstructions(instructions);
            Native.Check(NativeMethods.ls_save_instructions(buffer, (UIntPtr)buffer.Length, out IntPtr result, out UIntPtr size));
            return BufferReader.Take(result, size);
        }

        /// <summary>
        /// Write instructions in a binary stream
        /// </summary>
        public static void SaveInstructions(System.IO.Stream stream, IEnumerable<Instruction> instructions)
        {
            if (stream == null)
                throw new ArgumentNullException(nameof(stream));
            byte[] data = SaveInstructions(instructions);
            stream.Write(data, 0, data.Length);
        }

        /// <summary>
        /// Read instructions from their binary format
        /// </summary>
        /// <exception cref="SerializationException">The data is invalid</exception>
        public static List<Instruction> LoadInstructions(byte[] data)
        {
            if (data == null)
                throw new ArgumentNullException(nameof(data));
            Native.EnsureInitialized();
            Native.Check(NativeMethods.ls_load_instructions(data, (UIntPtr)data.Length, out IntPtr result, out UIntPtr size));
            return new BufferReader(result, size).Instructions();
        }

        /// <summary>
        /// Read instructions from a binary stream (read until its end)
        /// </summary>
        /// <exception cref="SerializationException">The stream is invalid</exception>
        public static List<Instruction> LoadInstructions(System.IO.Stream stream)
        {
            if (stream == null)
                throw new ArgumentNullException(nameof(stream));
            using (var data = new System.IO.MemoryStream())
            {
                stream.CopyTo(data);
                return LoadInstructions(data.ToArray());
            }
        }
    }

    /// <summary>
    /// The standard library: optional functions that the application can give to its scripts (LiteScript::Standard).
    /// The functions are registered in the memory with names starting with "std.", the states that use
    /// them can be saved and loaded (the library must be installed in the memory before the loading).
    /// </summary>
    public static class Standard
    {
        private static void Install(State state, int module)
        {
            if (state == null)
                throw new ArgumentNullException(nameof(state));
            state.Memory.Check(NativeMethods.ls_standard_install(state.Handle, module));
            GC.KeepAlive(state);
        }

        /// <summary>
        /// Install every module
        /// </summary>
        public static void Install(State state) => Install(state, 0);

        /// <summary>
        /// The namespace "Math": abs, floor, ceil, round, trunc, sign, min, max, pow, sqrt, exp, log,
        /// sin, cos, tan, asin, acos, atan, atan2, random, seed, and the constants PI and E
        /// </summary>
        public static void InstallMath(State state) => Install(state, 1);

        /// <summary>
        /// The global functions typeOf, toString, parseNumber, parseInt, isNaN and char
        /// </summary>
        public static void InstallConversions(State state) => Install(state, 2);

        /// <summary>
        /// The methods of the arrays: push, pop, insert, remove, clear, indexOf, contains, join,
        /// slice, reverse, sort, keys, map, filter, forEach
        /// </summary>
        public static void InstallArrays(State state) => Install(state, 3);

        /// <summary>
        /// The methods of the strings: substring, indexOf, contains, startsWith, endsWith, split,
        /// replace, trim, toUpperCase, toLowerCase, charCode
        /// </summary>
        public static void InstallStrings(State state) => Install(state, 4);
    }
}
