using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Xunit;

namespace LiteScript.Tests
{
    public class AssemblyTests
    {
        [Fact]
        public void CompileAndDisassemble()
        {
            Assembly.Result result = Assembly.Compile("value.number 5\nvalue.string a b\nvalue.pop");
            Assert.True(result);
            Assert.Equal(3, result.Instructions.Count);
            Assert.Equal(InstrCode.ValueNumber, result.Instructions[0].Code);
            Assert.Equal(5L, result.Instructions[0].GetInteger());
            Assert.Equal("a b", result.Instructions[1].GetString());
            string code = Assembly.Disassemble(result.Instructions);
            Assert.Equal(result.Instructions, Assembly.Compile(code).Instructions);

            Instruction instruction = Assembly.CompileInstruction("unknown.command", out Assembly.Error error);
            Assert.Equal(Assembly.Error.UnknownCommand, error);
            Assert.Equal(InstrCode.Invalid, instruction.Code);
            Assert.False(string.IsNullOrEmpty(Assembly.GetErrorMessage(error)));
        }

        [Fact]
        public void ExecuteAssembly()
        {
            using var memory = new Memory();
            using var state = new State(memory);
            Assembly.Result result = Assembly.Execute(state, "value.number 2\nvalue.number 3\noperator.add");
            Assert.Equal("5", result.Value!.ToString());
            Assert.Equal("5", state.ExecuteInstruction(new Instruction(InstrCode.ValueNumber, 5)).ToString());
        }

        [Fact]
        public void ScriptToAssembly()
        {
            Script.Result result = Script.Compile("var a = 1.5;");
            string code = Assembly.Disassemble(result.Instructions);
            Assert.Contains("1.5", code);
            Assert.Equal(result.Instructions, Assembly.Compile(code).Instructions);
        }

        [Fact]
        public void BinaryInstructions()
        {
            Script.Result result = Script.Compile("function f(x) { return x + \"é\"; } var b = true;");
            byte[] data = Serialization.SaveInstructions(result.Instructions);
            Assert.Equal(result.Instructions, Serialization.LoadInstructions(data));
            var stream = new MemoryStream();
            Serialization.SaveInstructions(stream, result.Instructions);
            stream.Position = 0;
            Assert.Equal(result.Instructions, Serialization.LoadInstructions(stream));
            Assert.Throws<SerializationException>(() => Serialization.LoadInstructions(new byte[] { 0xFF, 0xFF, 0xFF }));
        }

        [Fact]
        public void Instructions()
        {
            var a = new Instruction(InstrCode.ValueNumber, 1) { Position = new SourcePosition(3, 4) };
            var b = new Instruction(InstrCode.ValueNumber, 1L);
            Assert.Equal(a, b);
            Assert.True(a.HasInteger);
            Assert.Throws<InvalidCastException>(() => a.GetString());
            Assert.Throws<ArgumentException>(() => new Instruction(InstrCode.ValueNumber, (object)1.5f));
            Assert.Equal("value.number 1", a.ToString());
        }
    }

    public class DebugTests
    {
        [Fact]
        public void Analysis()
        {
            var analysis = new Script.Analysis();
            Script.Result result = Script.Compile("function add(a, b) {\n    return a + b;\n}\nvar x = add(1, 2);", analysis);
            Assert.True(result);
            Script.Analysis.Symbol add = analysis.Symbols.First(s => s.Name == "add");
            Assert.Equal(Script.Analysis.SymbolKind.Function, add.Kind);
            Assert.Equal(new[] { "a", "b" }, add.Parameters);
            Assert.Equal(0, add.Range.Start.Line);
            Assert.NotEqual(Script.Analysis.None, add.Body);
            Assert.True(add.FirstInstruction >= 0 && add.EndInstruction > add.FirstInstruction);
            Assert.Contains(analysis.Symbols, s => s.Name == "a" && s.Kind == Script.Analysis.SymbolKind.Parameter);
            Assert.Contains(analysis.References, r => r.Name == "add" && r.Range.Start.Line == 3);
            Assert.Equal(Script.Analysis.ScopeKind.Script, analysis.Scopes[0].Kind);
            Assert.Equal(Script.Analysis.None, analysis.Scopes[0].Parent);
            Assert.NotEmpty(analysis.Statements);
        }

        [Fact]
        public void DebugHookAndEvaluation()
        {
            using var e = new Engine();
            int calls = 0;
            var seen = new List<string>();
            e.State.SetDebugHook(state =>
            {
                calls++;
                List<FrameInfo> frames = state.GetFrames();
                if (frames.Count == 2)
                {
                    // The local variables of the function are visible
                    Script.Result expression = Script.Compile("(n * 10);");
                    expression.Instructions.RemoveAt(expression.Instructions.Count - 1);
                    seen.Add(state.Evaluate(expression.Instructions).ToString());
                    Assert.Single(frames[1].Args);
                }
            });
            e.Run("function f(n) { var m = n + 1; return m; }\nf(4);");
            Assert.True(calls > 0);
            Assert.Contains("40", seen);
            e.State.SetDebugHook(null);
            calls = 0;
            e.Run("f(1);");
            Assert.Equal(0, calls);
        }

        [Fact]
        public void ErrorHook()
        {
            using var e = new Engine();
            ScriptException? reported = null;
            int depth = 0;
            e.State.SetErrorHook((state, error) =>
            {
                reported = error;
                depth = state.GetFrames().Count;
            });
            Assert.Throws<ScriptException>(() => e.Run("function f() { throw \"oops\"; }\nf();"));
            Assert.NotNull(reported);
            Assert.Equal("oops", reported!.Message);
            Assert.Equal("\"oops\"", reported.Value!.ToString());
            Assert.Equal(2, depth);
        }

        private sealed class StopException : Exception
        {
        }

        [Fact]
        public void ExceptionsOfHooksReachTheCaller()
        {
            using var e = new Engine();
            bool nested = false;
            e.State.DeclareVariable("apply", e.Memory.CreateCallback((state, args) =>
            {
                nested = true;
                try
                {
                    return state.Call(args[0]!);
                }
                finally
                {
                    nested = false;
                }
            }));
            e.Run("function inner() { var a = 1; return a; }");
            e.State.SetDebugHook(state =>
            {
                if (nested)
                    throw new StopException();
            });
            // The exception crosses the native function "apply" and isn't caught by the script
            Assert.Throws<StopException>(() => e.Run("try { apply(inner); } catch { print(\"caught\"); }"));
            Assert.Equal(string.Empty, e.Output);
            e.State.SetDebugHook(null);
            Assert.Equal("1", e.Eval("inner()"));
        }

        [Fact]
        public void StackAndPosition()
        {
            using var e = new Engine();
            e.State.AddInstructions(Assembly.Compile("value.number 1\nvalue.number 2").Instructions);
            e.State.ExecuteSingle();
            e.State.ExecuteSingle();
            List<Variable?> stack = e.State.GetStack();
            Assert.Equal(new[] { "1", "2" }, stack.Select(v => v!.ToString()));
            Assert.Equal(2, e.State.Line);
            Assert.Equal(e.State.InstructionListCount - 1, e.State.ListIndex);
            Assert.Equal(1, e.State.Depth);
        }
    }

    public class MemoryTests
    {
        [Fact]
        public void GarbageCollector()
        {
            using var memory = new Memory(0);
            Variable a = memory.CreateArray();
            Variable b = memory.CreateArray();
            a.AsArray().Indexed.Add(b);
            b.AsArray().Indexed.Add(a);
            long count = memory.Count;
            Assert.Equal(0, memory.CollectGarbage());
            a.Dispose();
            b.Dispose();
            Assert.Equal(count, memory.Count);
            Assert.True(memory.CollectGarbage() > 0);
            Assert.True(memory.Count < count);
        }

        private static void CreateGarbage(Memory memory)
        {
            for (int i = 0; i < 1000; i++)
                memory.CreateString("value " + i);
        }

        [Fact]
        public void FinalizedVariablesAreReleased()
        {
            using var memory = new Memory();
            long initial = memory.Count;
            CreateGarbage(memory);
            Assert.True(memory.Count >= initial + 1000);
            GC.Collect();
            GC.WaitForPendingFinalizers();
            // The released variables are destroyed by the next operation of the memory
            Assert.Equal(initial, memory.Count);
        }

        [Fact]
        public void DisposingTheMemoryDisposesTheStates()
        {
            var memory = new Memory();
            var state = new State(memory);
            Variable v = memory.CreateNumber(3);
            memory.Dispose();
            Assert.True(state.IsDisposed);
            Assert.Throws<ObjectDisposedException>(() => state.Execute());
            Assert.Throws<ObjectDisposedException>(() => memory.CreateNull());
            // The variables can still be released
            v.Dispose();
            state.Dispose();
        }

        [Fact]
        public void UnusedMemoriesAreFinalized()
        {
            static WeakReference Create()
            {
                var memory = new Memory();
                var state = new State(memory);
                state.DeclareVariable("twice", memory.RegisterFunction("twice", (s, args) => s.Memory.CreateNumber(args[0]!.AsNumber() * 2)));
                Script.Execute(state, "var a = [1, 2, 3]; a.push = twice(5);");
                return new WeakReference(memory);
            }
            WeakReference reference = Create();
            for (int i = 0; i < 3 && reference.IsAlive; i++)
            {
                GC.Collect();
                GC.WaitForPendingFinalizers();
            }
            Assert.False(reference.IsAlive);
        }
    }
}
