using System;
using System.Collections.Generic;
using System.IO;
using Xunit;

namespace LiteScript.Tests
{
    public class StateTests
    {
        [Fact]
        public void ExecuteScript()
        {
            using var e = new Engine();
            e.Run(@"
                function fibo(x) { return x < 2 ? 1 : fibo(x - 1) + fibo(x - 2); }
                print(fibo(5));
            ");
            Assert.Equal("8", e.Output);
            Assert.Equal("6.5", e.Eval("1 + 2 + 3.5"));
        }

        [Fact]
        public void CompilationErrors()
        {
            using var e = new Engine();
            Script.Result result = Script.Execute(e.State, "var a = ;");
            Assert.False(result);
            Assert.NotEqual(Script.Error.None, result.Error);
            Assert.Equal(0, result.Line);
            Assert.False(string.IsNullOrEmpty(result.GetMessage()));
            Assert.Empty(result.Instructions);
        }

        [Fact]
        public void InstructionByInstruction()
        {
            using var e = new Engine();
            Script.Result result = Script.Compile("var a = 1; a += 2; print(a);");
            Assert.True(result);
            int list = e.State.AddInstructions(result.Instructions);
            Assert.Equal(list, e.State.InstructionListCount - 1);
            Assert.Equal(result.Instructions, e.State.GetInstructions(list));
            int steps = 0;
            while (!e.State.Finished)
            {
                e.State.ExecuteSingle();
                steps++;
            }
            Assert.Equal(result.Instructions.Count, steps);
            Assert.Equal("3", e.Output);
        }

        [Fact]
        public void NativeFunctions()
        {
            using var e = new Engine();
            e.State.DeclareVariable("sum", e.Memory.CreateCallback((state, args) =>
            {
                Number total = 0;
                foreach (Variable? v in args)
                {
                    if (v != null && v.Is(ScriptType.Number))
                        total += v.AsNumber();
                }
                Assert.Equal(args.Count, state.ArgsCount);
                return state.Memory.CreateNumber(total);
            }));
            e.State.DeclareVariable("self", e.Memory.CreateCallback((state, args) => state.GetThis()));
            e.State.DeclareVariable("first", e.Memory.CreateCallback((state, args) => args.Count > 0 ? args[0] : null));
            Assert.Equal("6.5", e.Eval("sum(1, 2, 3.5)"));
            Assert.Equal("0", e.Eval("sum()"));
            Assert.Equal("undefined", e.Eval("self()"));
            Assert.Equal("\"x\"", e.Eval("first(\"x\")"));
            Assert.Equal("undefined", e.Eval("first()"));
            e.Run("var a = { f: self, v: 3 };");
            Assert.Equal("3", e.Eval("a.f().v"));
        }

        [Fact]
        public void CallScriptFunctions()
        {
            using var e = new Engine();
            e.Run(@"
                function add(a, b) { return a + b; }
                class P {
                    var x = 1;
                    operator call(y) { return this.x + y; }
                }
                var p = new P();
            ");
            Variable add = e.State.GetVariable("add")!;
            Assert.Equal("5", e.State.Call(add, e.Memory.CreateNumber(2), e.Memory.CreateNumber(3)).ToString());
            Variable p = e.State.GetVariable("p")!;
            Assert.Equal(ScriptType.ClassObject, p.Type);
            Assert.Equal("11", e.State.Call(p, e.Memory.CreateNumber(10)).ToString());
            Assert.Null(e.State.GetVariable("missing"));
        }

        [Fact]
        public void NativeFunctionCallsScriptFunction()
        {
            using var e = new Engine();
            e.State.DeclareVariable("apply", e.Memory.CreateCallback((state, args) => state.Call(args[0]!, args[1])));
            Assert.Equal("4", e.Eval("apply(function(x) { return x * 2; }, 2)"));
        }

        [Fact]
        public void UncaughtErrors()
        {
            using var e = new Engine();
            var error = Assert.Throws<ScriptException>(() => e.Run("var x = 1;\nx();"));
            Assert.Equal("a value of type number isn't a function", error.Message);
            Assert.Equal(1u, error.Position.Line);
            Assert.Equal(ScriptType.Array, error.Value!.Type);
            Assert.Equal("\"a value of type number isn't a function\"", error.Value.AsArray().Named.Find("message")!.ToString());
            // The state remains usable
            Assert.Equal("1", e.Eval("x"));

            var thrown = Assert.Throws<ScriptException>(() => e.Run("throw { code: 3 };"));
            Assert.Equal("3", thrown.Value!.AsArray().Named.Find("code")!.ToString());
        }

        [Fact]
        public void ExceptionsOfNativeFunctionsBecomeScriptErrors()
        {
            using var e = new Engine();
            e.State.DeclareVariable("fail", e.Memory.CreateCallback((state, args) => throw new ArgumentException("invalid argument")));
            e.State.DeclareVariable("raise", e.Memory.CreateCallback((state, args) =>
                throw new ScriptException(state.Memory.CreateArray(("code", state.Memory.CreateNumber(7))))));
            Assert.Equal("\"invalid argument\"", e.Eval("(function() { try { fail(); } catch (err) { return err.message; } })()"));
            Assert.Equal("7", e.Eval("(function() { try { raise(); } catch (err) { return err.code; } })()"));
            var error = Assert.Throws<ScriptException>(() => e.Run("fail();"));
            Assert.Equal("invalid argument", error.Message);
        }

        [Fact]
        public void ErrorsOfNestedCallsCrossNativeFunctions()
        {
            using var e = new Engine();
            e.State.DeclareVariable("apply", e.Memory.CreateCallback((state, args) => state.Call(args[0]!)));
            Assert.Equal("\"inner\"", e.Eval("(function() { try { apply(function() { throw \"inner\"; }); } catch (err) { return err; } })()"));
        }

        [Fact]
        public void VariablesAndNamespaces()
        {
            using var e = new Engine();
            e.State.DeclareVariable("config", e.Memory.CreateArray(("debug", e.Memory.CreateBoolean(true))));
            Assert.Equal("true", e.Eval("config.debug"));
            // The script declares its variables in the selected namespace
            e.State.UseNamespace("Game.Engine");
            e.Run("var version = 2;");
            Assert.Equal("2", e.Eval("Game.Engine.version"));
            Variable global = e.State.GlobalNamespace;
            Assert.NotNull(global.AsNamespace().Variables.Find("config"));
            Assert.Equal("2", Operations.GetMember(e.Memory, Operations.GetMember(e.Memory, e.State.GetVariable("Game"), "Engine"), "version").ToString());
        }

        [Fact]
        public void SaveAndLoad()
        {
            byte[] saved;
            using (var e = new Engine())
            {
                Standard.Install(e.State);
                e.Run("var total = 0; function add(n) { total += n; return total; }");
                e.Run("add(5);");
                saved = e.State.Save();
            }
            using (var e = new Engine())
            {
                Standard.Install(e.State);
                e.State.Load(new MemoryStream(saved));
                Assert.Equal("5", e.Eval("total"));
                Assert.Equal("8", e.Eval("add(3)"));
                Assert.Equal("2", e.Eval("Math.sqrt(4)"));
            }
        }

        [Fact]
        public void UnregisteredFunctionsCantBeSaved()
        {
            using var e = new Engine();
            e.State.DeclareVariable("f", e.Memory.CreateCallback((state, args) => null));
            Assert.Throws<SerializationException>(() => e.State.Save());
            Assert.Throws<SerializationException>(() => e.State.Load(new byte[] { 1, 2, 3 }));
        }

        [Fact]
        public void StandardLibrary()
        {
            using var e = new Engine();
            Standard.Install(e.State);
            Assert.Equal("3", e.Eval("Math.max(1, 3, 2)"));
            Assert.Equal("\"number\"", e.Eval("typeOf(1)"));
            Assert.Equal("{[0]:2,[1]:4}", e.Eval("[1, 2].map(function(x) { return x * 2; })"));
            Assert.Equal("\"ABC\"", e.Eval("\"abc\".toUpperCase()"));
            Assert.True(e.Memory.HasFunction("std.Math.max") || e.Memory.HasFunction("std.math.max") || e.Memory.Count > 0);
        }

        [Fact]
        public void Classes()
        {
            using var e = new Engine();
            e.Run(@"
                class Point {
                    var x = 0, y = 0;
                    function Point(x, y) { this.x = x; this.y = y; }
                    operator add(p) { return new Point(this.x + p.x, this.y + p.y); }
                    static var count = 0;
                }
                var a = new Point(1, 2);
                var b = new Point(3, 4);
            ");
            Variable pointClass = e.State.GetVariable("Point")!;
            ClassValue content = pointClass.AsClass();
            Assert.Equal("Point", content.Constructor);
            Assert.NotNull(content.GetConstructor());
            Assert.NotNull(content.GetOperator(Operator.Add));
            Assert.Null(content.GetOperator(Operator.Subtract));
            Assert.Equal(0, content.Statics.IndexOf("count"));
            Assert.True(content.Members.IndexOf("x") >= 0);

            Variable a = e.State.GetVariable("a")!;
            ClassObjectValue instance = a.AsClassObject();
            Assert.True(Variable.SameObject(instance.Base!.Target(), pointClass.Target()));
            Assert.Equal("1", instance.Members.Find("x")!.ToString());

            OperationResult result = Operations.Operate(e.Memory, Operator.Add, a, e.State.GetVariable("b"));
            Assert.True(result.IsOverloadCall);
            Assert.Single(result.Call!.Args);
            Variable sum = Operations.Operate(e.State, Operator.Add, a, e.State.GetVariable("b"));
            Assert.Equal("4", sum.AsClassObject().Members.Find("x")!.ToString());
            Assert.Equal("6", sum.AsClassObject().Members.Find("y")!.ToString());

            // Instance created by the application
            Variable created = e.Memory.CreateClassObject(pointClass);
            created.AsClassObject().Members.Set("x", e.Memory.CreateNumber(9));
            e.State.DeclareVariable("c", created);
            Assert.Equal("9", e.Eval("c.x"));
        }

        [Fact]
        public void Callbacks()
        {
            using var e = new Engine();
            e.Run("function f(a, b) { return a; }");
            CallbackValue f = e.State.GetVariable("f")!.AsCallback();
            Assert.False(f.IsNative);
            Assert.NotNull(f.ScriptFunction);
            Assert.NotNull(f.ScriptFunction!.Scope.Find("f"));
            CallbackValue print = e.State.GetVariable("print")!.AsCallback();
            Assert.True(print.IsNative);
            Assert.Equal("print", print.NativeName);
            Assert.Null(print.ScriptFunction);
            Assert.True(print.SameTarget(e.Memory.CreateCallback("print").AsCallback()));
            Assert.False(print.SameTarget(f));
            Assert.Equal(ScriptType.Undefined, e.Memory.CreateCallback("unknown").Type);
        }

        [Fact]
        public void Characters()
        {
            using var e = new Engine();
            e.Run("var s = \"abc\";");
            Variable c = e.Run("return s[1];")!;
            Assert.Equal(ScriptType.Character, c.Type);
            Assert.Equal(1, c.AsCharacter().Index);
            Assert.Equal("b", c.ToText());
        }

        [Fact]
        public void MaxCallDepth()
        {
            using var e = new Engine();
            e.State.SetMaxCallDepth(50);
            var error = Assert.Throws<ScriptException>(() => e.Run("function f(n) { return f(n + 1); } f(0);"));
            Assert.Equal("maximum call depth exceeded", error.Message);
        }
    }
}
