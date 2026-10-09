using System;
using System.Collections.Generic;
using Xunit;

namespace LiteScript.Tests
{
    public class NumberTests
    {
        [Fact]
        public void IntegerOperationsStayInteger()
        {
            Number six = 6, two = 2, five = 5;
            Assert.True((six / two).IsInteger);
            Assert.Equal(3, (six / two).ToInteger());
            Assert.False((five / two).IsInteger);
            Assert.Equal(2.5, (five / two).ToFloat());
            Assert.Equal("2.5", (five / two).ToString());
            Assert.Equal(1, (five % two).ToInteger());
        }

        [Fact]
        public void IntegralFloatsAreIntegers()
        {
            Assert.True(new Number(4.0).IsInteger);
            Assert.False(new Number(0.5).IsInteger);
            Assert.True(default(Number).IsInteger);
            Assert.Equal(0, default(Number).ToInteger());
            Assert.Equal(0, new Number(double.NaN).ToInteger());
            Assert.Equal(long.MaxValue, new Number(1e300).ToInteger());
        }

        [Fact]
        public void Comparisons()
        {
            Number a = 1, b = 1.5;
            Assert.True(a < b);
            Assert.True(b >= a);
            Assert.True(a == new Number(1.0));
            Assert.True(a != b);
        }

        [Fact]
        public void BinaryOperations()
        {
            Number a = 6, b = 3;
            Assert.Equal(2, (a & b).ToInteger());
            Assert.Equal(7, (a | b).ToInteger());
            Assert.Equal(5, (a ^ b).ToInteger());
            Assert.Equal(24, (a << 2).ToInteger());
            Assert.Equal(-7, (~a).ToInteger());
        }
    }

    public class ValueTests
    {
        [Fact]
        public void CreationAndReading()
        {
            using var memory = new Memory();
            using Variable n = memory.CreateNumber(42);
            using Variable s = memory.CreateString("héllo 🌍");
            using Variable b = memory.CreateBoolean(true);
            using Variable u = memory.CreateUndefined();
            using Variable nul = memory.CreateNull();

            Assert.Equal(ScriptType.Number, n.Type);
            Assert.Equal(42, n.AsNumber().ToInteger());
            Assert.Equal("héllo 🌍", s.AsString());
            Assert.Equal(7, s.GetStringLength());
            Assert.Equal("\"héllo 🌍\"", s.ToString());
            Assert.Equal("héllo 🌍", s.ToText());
            Assert.True(b.AsBoolean());
            Assert.Equal(ScriptType.Undefined, u.Type);
            Assert.Equal(ScriptType.Null, nul.Type);
            Assert.Throws<InvalidCastException>(() => n.AsString());
            Assert.Equal("number", Memory.GetTypeName(ScriptType.Number));
        }

        [Fact]
        public void Arrays()
        {
            using var memory = new Memory();
            Variable array = memory.CreateArray(memory.CreateNumber(1), memory.CreateString("a"));
            ArrayValue content = array.AsArray();
            Assert.Equal(2, content.Indexed.Count);
            Assert.Equal(1, content.Indexed[0]!.AsNumber().ToInteger());
            content.Indexed.Add(memory.CreateBoolean(false));
            content.Indexed.RemoveAt(0);
            Assert.Equal("{[0]:\"a\",[1]:false}", array.ToString());

            Variable named = memory.CreateArray(("name", memory.CreateString("Anne")), ("age", memory.CreateNumber(21)));
            MemberList members = named.AsArray().Named;
            Assert.Equal(2, members.Count);
            Assert.Equal("age", members.GetName(1));
            Assert.Equal(21, members.Find("age")!.AsNumber().ToInteger());
            Assert.Null(members.Find("missing"));
            Assert.Equal(-1, members.IndexOf("missing"));
            members["city"] = memory.CreateString("Paris");
            Assert.Equal("Paris", members["city"]!.AsString());
            var names = new List<string>();
            foreach (KeyValuePair<string, Variable?> member in members)
                names.Add(member.Key);
            Assert.Equal(new[] { "name", "age", "city" }, names);
        }

        [Fact]
        public void IdentityAndDuplicates()
        {
            using var memory = new Memory();
            Variable a = memory.CreateNumber(1);
            Variable b = a.Duplicate();
            Assert.True(a.Equals(b));
            Assert.Equal(a.GetHashCode(), b.GetHashCode());
            Assert.Equal(2, a.ReferenceCount);
            b.Dispose();
            Assert.True(b.IsDisposed);
            Assert.Equal(1, a.ReferenceCount);
            Assert.Throws<ObjectDisposedException>(() => b.Type);
        }

        [Fact]
        public void ContentReplacement()
        {
            using var memory = new Memory();
            Variable v = memory.CreateNumber(1);
            v.SetString("text");
            Assert.Equal("text", v.AsString());
            v.SetNumber(2.5);
            Assert.Equal(2.5, v.AsNumber().ToFloat());
            v.SetNull();
            Assert.Equal(ScriptType.Null, v.Type);
        }

        [Fact]
        public void OperationsWithTheLanguageSemantic()
        {
            using var memory = new Memory();
            Variable one = memory.CreateNumber(1), two = memory.CreateNumber(2), text = memory.CreateString("a");
            OperationResult sum = Operations.Operate(memory, Operator.Add, one, two);
            Assert.False(sum.IsOverloadCall);
            Assert.Equal("3", sum.Value!.ToString());
            Assert.Equal(ScriptType.Null, Operations.Operate(memory, Operator.Add, one, text).Value!.Type);
            Assert.Equal("\"a1\"", Operations.Operate(memory, Operator.Add, text, one).Value!.ToString());
            Assert.True(Operations.IsTruthy(one));
            Assert.False(Operations.IsTruthy(memory.CreateString("")));
            Assert.True(Operations.ValuesEqual(one, memory.CreateNumber(1.0)));
            Assert.Equal(ValueOrdering.Less, Operations.CompareValues(one, two));
            Assert.Equal(ValueOrdering.Unordered, Operations.CompareValues(one, text));
            Assert.True(Operations.IsBinary(Operator.Add));
            Assert.False(Operations.IsBinary(Operator.LogicalNot));
            Assert.Equal("\"1\"", Operations.Convert(memory, one, ScriptType.String).ToString());

            Variable array = memory.CreateArray(one);
            Variable shared = Operations.Copy(memory, array);
            Variable cloned = Operations.Clone(memory, array);
            array.AsArray().Indexed.Add(two);
            Assert.Equal("{[0]:1,[1]:2}", shared.ToString());
            Assert.Equal("{[0]:1}", cloned.ToString());
        }

        [Fact]
        public void TypeMethods()
        {
            using var engine = new Engine();
            Memory memory = engine.Memory;
            Variable methods = memory.CreateNamespace();
            methods.AsNamespace().Variables.Set("double", memory.RegisterFunction("number.double", (state, args) =>
                state.Memory.CreateNumber(state.GetThis().AsNumber() * 2)));
            memory.SetMethods(ScriptType.Number, methods);
            Assert.Equal("42", engine.Eval("(21).double()"));
            Assert.NotNull(memory.FindMethod(ScriptType.Number, "double"));
            Assert.Null(memory.FindMethod(ScriptType.String, "double"));
        }
    }
}
