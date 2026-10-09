using System;
using System.Collections.Generic;
using LiteScript.Interop;

namespace LiteScript
{
    /// <summary>
    /// The object-oriented script language, similar to Javascript (LiteScript::Script)
    /// </summary>
    public static class Script
    {
        /// <summary>
        /// List of errors
        /// </summary>
        public enum Error
        {
            None,
            SimpleArrayEnd,
            NamedArrayColon,
            NamedArrayExpression,
            NamedArrayEnd,
            CallbackArguments,
            CallbackArgumentName,
            CallbackArgumentEnd,
            CallbackInstructions,
            ClassInherit,
            ClassBegin,
            ClassInvalid,
            ClassOperatorName,
            ClassOperatorInvalid,
            ClassEnd,
            ForInitialisation,
            ForCondition,
            SwitchKeywords,
            SwitchEnd,
            Name,
            Value,
            Operator,
            Expression,
            ParenthesisOpen,
            ParenthesisClose,
            BracketClose,
            BraceOpen,
            BraceClose,
            Colon,
            Semicolon,
            Instruction,
            Return,
            While,
            Namespace,
            Unknown,
            UnterminatedString,
            BreakOutsideLoop,
            ContinueOutsideLoop,
            Nesting,
            Catch
        }

        /// <summary>
        /// The result of a compilation
        /// </summary>
        public sealed class Result
        {
            internal Result(Error error, int line, int column, List<Instruction> instructions, Variable? value)
            {
                this.Error = error;
                this.Line = line;
                this.Column = column;
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
            /// The column of the error in characters (starts at 0)
            /// </summary>
            public int Column { get; }

            /// <summary>
            /// The value on the top of the stack after the execution (for Execute)
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

        /// <summary>
        /// Information about the declarations and the names of a script, for the development tools
        /// (editors, debuggers). Filled during the compilation, partially if there is an error.
        /// </summary>
        public sealed class Analysis
        {
            /// <summary>
            /// The value of the indexes that don't exist
            /// </summary>
            public const int None = -1;

            /// <summary>
            /// A position in the code (line and column start at 0, the column is counted in characters)
            /// </summary>
            public readonly struct Position
            {
                public Position(int line, int column, int offset)
                {
                    this.Line = line;
                    this.Column = column;
                    this.Offset = offset;
                }

                public int Line { get; }
                public int Column { get; }

                /// <summary>
                /// The offset in bytes (UTF-8)
                /// </summary>
                public int Offset { get; }

                public override string ToString() => $"{this.Line}:{this.Column}";
            }

            public readonly struct Range
            {
                public Range(Position start, Position end)
                {
                    this.Start = start;
                    this.End = end;
                }

                public Position Start { get; }
                public Position End { get; }

                public override string ToString() => $"{this.Start}-{this.End}";
            }

            public enum ScopeKind
            {
                /// <summary>The whole script</summary>
                Script,
                /// <summary>The arguments and the body of a function</summary>
                Function,
                /// <summary>The members of a class (between the braces)</summary>
                Class
            }

            /// <summary>
            /// A part of the code where variables are declared
            /// </summary>
            public sealed class Scope
            {
                public ScopeKind Kind { get; set; }
                public Range Range { get; set; }

                /// <summary>
                /// The scope that contains this one (None for the script)
                /// </summary>
                public int Parent { get; set; } = None;
            }

            public enum SymbolKind
            {
                Variable,
                Function,
                Class,
                Parameter,
                /// <summary>Attribute of a class</summary>
                Attribute,
                /// <summary>Method of a class</summary>
                Method,
                /// <summary>Overloaded operator of a class</summary>
                Operator,
                /// <summary>Variable of a "catch" block</summary>
                CatchVariable,
                /// <summary>Namespace selected by the "namespace" instruction</summary>
                Namespace
            }

            /// <summary>
            /// A declaration
            /// </summary>
            public sealed class Symbol
            {
                public string Name { get; set; } = string.Empty;
                public SymbolKind Kind { get; set; }

                /// <summary>
                /// Static member of a class
                /// </summary>
                public bool IsStatic { get; set; }

                /// <summary>
                /// The range of the name
                /// </summary>
                public Range Range { get; set; }

                /// <summary>
                /// The range of the whole declaration (with the body of a function or a class)
                /// </summary>
                public Range Full { get; set; }

                /// <summary>
                /// The scope that contains the declaration
                /// </summary>
                public int Scope { get; set; } = None;

                /// <summary>
                /// The scope created by a function or a class (None for the others)
                /// </summary>
                public int Body { get; set; } = None;

                /// <summary>
                /// The namespace selected when the symbol is declared (empty for the global namespace)
                /// </summary>
                public string Namespace { get; set; } = string.Empty;

                /// <summary>
                /// The arguments of a function (the last one ends with "..." if it gathers the remaining arguments)
                /// </summary>
                public List<string> Parameters { get; } = new List<string>();

                /// <summary>
                /// The first instruction of a function (None for the others)
                /// </summary>
                public int FirstInstruction { get; set; } = None;

                /// <summary>
                /// The end of the instructions of a function, excluded (None for the others)
                /// </summary>
                public int EndInstruction { get; set; } = None;
            }

            /// <summary>
            /// A use of a name
            /// </summary>
            public sealed class Reference
            {
                public string Name { get; set; } = string.Empty;
                public Range Range { get; set; }

                /// <summary>
                /// The scope that contains the use
                /// </summary>
                public int Scope { get; set; } = None;

                /// <summary>
                /// Member access ("object.name")
                /// </summary>
                public bool Member { get; set; }

                /// <summary>
                /// The namespace selected at the position of the use (empty for the global namespace)
                /// </summary>
                public string Namespace { get; set; } = string.Empty;
            }

            /// <summary>
            /// The beginning of an instruction of the language (used for the breakpoints and the steps)
            /// </summary>
            public sealed class Statement
            {
                public Position Position { get; set; }

                /// <summary>
                /// The index of the first instruction generated by the statement
                /// </summary>
                public int Instruction { get; set; }
            }

            public List<Scope> Scopes { get; } = new List<Scope>();
            public List<Symbol> Symbols { get; } = new List<Symbol>();
            public List<Reference> References { get; } = new List<Reference>();
            public List<Statement> Statements { get; } = new List<Statement>();

            private static Position ReadPosition(BufferReader r) => new Position(r.Index(), r.Index(), r.Index());

            private static Range ReadRange(BufferReader r)
            {
                Position start = ReadPosition(r);
                return new Range(start, ReadPosition(r));
            }

            internal void Read(BufferReader r)
            {
                this.Scopes.Clear();
                this.Symbols.Clear();
                this.References.Clear();
                this.Statements.Clear();
                uint count = r.U32();
                for (uint i = 0; i < count; i++)
                    this.Scopes.Add(new Scope { Kind = (ScopeKind)r.U8(), Range = ReadRange(r), Parent = r.Index() });
                count = r.U32();
                for (uint i = 0; i < count; i++)
                {
                    var symbol = new Symbol
                    {
                        Name = r.Str(),
                        Kind = (SymbolKind)r.U8(),
                        IsStatic = r.U8() != 0,
                        Range = ReadRange(r),
                        Full = ReadRange(r),
                        Scope = r.Index(),
                        Body = r.Index(),
                        Namespace = r.Str()
                    };
                    uint parameters = r.U32();
                    for (uint p = 0; p < parameters; p++)
                        symbol.Parameters.Add(r.Str());
                    symbol.FirstInstruction = r.Index();
                    symbol.EndInstruction = r.Index();
                    this.Symbols.Add(symbol);
                }
                count = r.U32();
                for (uint i = 0; i < count; i++)
                {
                    this.References.Add(new Reference
                    {
                        Name = r.Str(),
                        Range = ReadRange(r),
                        Scope = r.Index(),
                        Member = r.U8() != 0,
                        Namespace = r.Str()
                    });
                }
                count = r.U32();
                for (uint i = 0; i < count; i++)
                    this.Statements.Add(new Statement { Position = ReadPosition(r), Instruction = r.Index() });
            }
        }

        private static Result ReadResult(BufferReader r, Variable? value)
        {
            var error = (Error)r.U8();
            int line = r.Index();
            int column = r.Index();
            return new Result(error, line, column, r.Instructions(), value);
        }

        /// <summary>
        /// Convert a script code into instructions
        /// </summary>
        public static Result Compile(string code)
        {
            return Compile(code, null);
        }

        /// <summary>
        /// Convert a script code into instructions
        /// </summary>
        /// <param name="code">The code</param>
        /// <param name="analysis">If not null, receives the declarations and the names of the script</param>
        public static Result Compile(string code, Analysis? analysis)
        {
            Native.EnsureInitialized();
            byte[] bytes = Utf8.EncodeData(code, out UIntPtr length);
            Native.Check(NativeMethods.ls_script_compile(bytes, length, analysis != null ? 1 : 0, out IntPtr buffer, out UIntPtr size));
            var reader = new BufferReader(buffer, size);
            Result result = ReadResult(reader, null);
            if (reader.U8() != 0)
                analysis?.Read(reader);
            return result;
        }

        /// <summary>
        /// Compile a code, add it in a new list of the state and execute it
        /// </summary>
        /// <returns>The result of the compilation and the value returned by the script</returns>
        /// <exception cref="ScriptException">An error isn't caught by the script</exception>
        public static Result Execute(State state, string code)
        {
            if (state == null)
                throw new ArgumentNullException(nameof(state));
            byte[] bytes = Utf8.EncodeData(code, out UIntPtr length);
            int status = NativeMethods.ls_script_execute(state.Handle, bytes, length, out IntPtr buffer, out UIntPtr size, out IntPtr value);
            GC.KeepAlive(state);
            state.Memory.Check(status);
            return ReadResult(new BufferReader(buffer, size), state.Memory.WrapOptional(value));
        }

        /// <summary>
        /// Return the message of an error
        /// </summary>
        public static string GetErrorMessage(Error error)
        {
            Native.EnsureInitialized();
            Native.Check(NativeMethods.ls_script_error_message((int)error, out IntPtr message));
            return Utf8.Take(message);
        }
    }
}
