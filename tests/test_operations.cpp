#include "helpers.hpp"

#include <limits>

using namespace LiteScript;

TEST_SUITE("operations") {

    TEST_CASE("arithmetic") {
        Tests::Engine e;
        CHECK(e.Eval("1 + 2 * 3") == "7");
        CHECK(e.Eval("(1 + 2) * 3") == "9");
        CHECK(e.Eval("10 - 4 - 3") == "3");
        CHECK(e.Eval("7 / 2") == "3.5");
        CHECK(e.Eval("6 / 2") == "3");
        CHECK(e.Eval("7.0 / 2") == "3.5");
        CHECK(e.Eval("7 / 2.0") == "3.5");
        CHECK(e.Eval("7 % 3") == "1");
        CHECK(e.Eval("-3 + +5") == "2");
        CHECK(e.Eval("1 + true") == "2");
        CHECK(e.Eval("1 + null") == "null");
        CHECK(e.Eval("1 + \"2\"") == "null");
        CHECK(e.Eval("null * 2") == "null");
        CHECK(e.Eval("1e3 + .5") == "1000.5");
    }

    TEST_CASE("strings") {
        Tests::Engine e;
        CHECK(e.Eval("\"a\" + \"b\"") == "\"ab\"");
        CHECK(e.Eval("\"a\" + 1") == "\"a1\"");
        CHECK(e.Eval("\"a\" + 1.5") == "\"a1.5\"");
        CHECK(e.Eval("\"a\" + true + null + undefined") == "\"atruenullundefined\"");
        CHECK(e.Eval("\"ab\" * 3") == "\"ababab\"");
        CHECK(e.Eval("\"ab\" * \"x\"") == "null");
        CHECK(e.Eval("\"x\" * 100000000000") == "null");
        CHECK(e.Eval("\"abc\".length") == "3");
        CHECK(e.Eval("\"h\\u00e9\".length") == "2");
        CHECK(e.Eval("\"abc\"[1]") == "b");
        CHECK(e.Eval("\"abc\"[3]") == "undefined");
        CHECK(e.Eval("\"abc\"[-1]") == "undefined");
        CHECK(e.Eval("\"abc\" + [1, \"x\"]") == "\"abc{[0]:1,[1]:\"x\"}\"");
    }

    TEST_CASE("characters modify their string") {
        Tests::Engine e;
        e.Run(R"(
            var s = "hello";
            s[0] = "J";
            s[4] += "!";
            s[1] *= 2;
            var c = s[2];
            c = "z";
        )");
        CHECK(e.Eval("s") == "\"Jeello!\"");
        CHECK(e.Eval("c") == "\"z\"");
        CHECK(e.Eval("s[0] == \"J\"") == "true");
        CHECK(e.Eval("\"J\" == s[0]") == "true");
        CHECK(e.Eval("s[0] + s[1]") == "\"Je\"");
    }

    TEST_CASE("comparisons") {
        Tests::Engine e;
        CHECK(e.Eval("1 < 2") == "true");
        CHECK(e.Eval("2 <= 2") == "true");
        CHECK(e.Eval("3 > 4") == "false");
        CHECK(e.Eval("1 == 1.0") == "true");
        CHECK(e.Eval("1 == \"1\"") == "false");
        CHECK(e.Eval("1 != \"1\"") == "true");
        CHECK(e.Eval("\"abc\" < \"abd\"") == "true");
        CHECK(e.Eval("null == null") == "true");
        CHECK(e.Eval("null == undefined") == "false");
        CHECK(e.Eval("undefined == undefined") == "true");
        CHECK(e.Eval("x == undefined") == "true");
        CHECK(e.Eval("true == true") == "true");
        CHECK(e.Eval("true == 1") == "false");
        CHECK(e.Eval("1 < null") == "false");
        CHECK(e.Eval("1 >= null") == "false");
        CHECK(e.Eval("null < 1") == "false");
    }

    TEST_CASE("identity comparison of containers") {
        Tests::Engine e;
        e.Run("var a = [1]; var b = a; var c = [1]; var f = function() {}; var g = f;");
        CHECK(e.Eval("a == a") == "true");
        // The assignment shares the array
        CHECK(e.Eval("a == b") == "true");
        CHECK(e.Eval("a != b") == "false");
        CHECK(e.Eval("a == c") == "false");
        CHECK(e.Eval("f == g") == "true");
        CHECK(e.Eval("print == print") == "true");
        CHECK(e.Eval("f == print") == "false");
    }

    TEST_CASE("logical operations") {
        Tests::Engine e;
        CHECK(e.Eval("true && false") == "false");
        CHECK(e.Eval("1 && 2") == "true");
        CHECK(e.Eval("0 || \"\"") == "false");
        CHECK(e.Eval("!0") == "true");
        CHECK(e.Eval("!!\"x\"") == "true");
        CHECK(e.Eval("!null") == "true");
        CHECK(e.Eval("![]") == "false");
    }

    TEST_CASE("binary operations") {
        Tests::Engine e;
        CHECK(e.Eval("6 & 3") == "2");
        CHECK(e.Eval("6 | 3") == "7");
        CHECK(e.Eval("6 ^ 3") == "5");
        CHECK(e.Eval("~5") == "-6");
        CHECK(e.Eval("1 << 3 + 1") == "16");
        CHECK(e.Eval("-8 >> 1") == "-4");
        // "|" gives the second operand when the first one is undefined or null
        CHECK(e.Eval("undefined | 5") == "5");
        CHECK(e.Eval("null | \"x\"") == "\"x\"");
        CHECK(e.Eval("\"y\" | \"x\"") == "\"y\"");
    }

    TEST_CASE("assignments") {
        Tests::Engine e;
        e.Run(R"(
            var a = 1, b, c;
            b = c = a + 1;
            a += 10;
            var s = "x";
            s += 1;
            s *= 2;
            var n = 5;
            n -= 1;
            n *= 3;
            n /= 2;
            var u = 3;
            u += "a";
        )");
        CHECK(e.Eval("a") == "11");
        CHECK(e.Eval("b") == "2");
        CHECK(e.Eval("c") == "2");
        CHECK(e.Eval("s") == "\"x1x1\"");
        CHECK(e.Eval("n") == "6");
        CHECK(e.Eval("u") == "null");
        // A declared variable without value is null, undefined can be assigned
        e.Run("var v = 1; v = undefined; var w;");
        CHECK(e.Eval("v") == "undefined");
        CHECK(e.Eval("w") == "null");
    }

    TEST_CASE("increments") {
        Tests::Engine e;
        e.Run("var i = 1; var a = i++; var b = ++i; var c = i--; var d = --i; var f = 1.5; f++;");
        CHECK(e.Eval("i") == "1");
        CHECK(e.Eval("a") == "1");
        CHECK(e.Eval("b") == "3");
        CHECK(e.Eval("c") == "3");
        CHECK(e.Eval("d") == "1");
        CHECK(e.Eval("f") == "2.5");
        CHECK(e.Eval("\"x\"++") == "null");
    }

    TEST_CASE("arrays") {
        Tests::Engine e;
        e.Run(R"(
            var a = [1, 2, 3];
            a[5] = 6;
            a["name"] = "x";
            var b = { first: 1, "second": [true], };
            var c = [1, 2] + { x: 3 };
        )");
        CHECK(e.Eval("a") == "{[0]:1,[1]:2,[2]:3,[5]:6,['name']:\"x\"}");
        CHECK(e.Eval("a.length") == "6");
        CHECK(e.Eval("a[4]") == "undefined");
        CHECK(e.Eval("a.name") == "\"x\"");
        CHECK(e.Eval("b.second[0]") == "true");
        CHECK(e.Eval("b[\"first\"]") == "1");
        CHECK(e.Eval("c") == "{[0]:1,[1]:2,['x']:3}");
        CHECK(e.Eval("[]") == "{}");
        CHECK(e.Eval("[1,2,][1]") == "2");
        CHECK(e.Eval("a[-1]") == "undefined");
        CHECK(e.Eval("a[1.5]") == "undefined");
        CHECK(e.Eval("a[1e12]") == "undefined");
        CHECK(e.Eval("a[null]") == "undefined");
    }

    TEST_CASE("arrays, instances and namespaces are shared by assignment") {
        Tests::Engine e;
        e.Run(R"(
            var a = [1, 2];
            var b = a;
            b[0] = 10;
            b[2] = 3;
            var n = 5;
            var m = n;
            m++;
            var s = "ab";
            var t = s;
            t[0] = "x";
            b = [7];
            class P { var v = 0; }
            var p = new P();
            function set(o) { o.v = 42; o = null; }
            set(p);
        )");
        CHECK(e.Eval("a") == "{[0]:10,[1]:2,[2]:3}");
        // Assigning the variable doesn't modify the shared array
        CHECK(e.Eval("b") == "{[0]:7}");
        // Numbers and strings are copied
        CHECK(e.Eval("n") == "5");
        CHECK(e.Eval("s") == "\"ab\"");
        CHECK(e.Eval("t") == "\"xb\"");
        CHECK(e.Eval("p.v") == "42");
    }

    TEST_CASE("reading a missing element doesn't create it") {
        Tests::Engine e;
        e.Run(R"(
            var a = [1, 2];
            var x = a[5], y = a.name;
            var len = a.length;
            if (a[3] == undefined) a[4] = "four";
            a.name = "n";
            class P { var v = 1; }
            var p = new P();
            var before = p.other;
            p.other = 2;
            namespace Space;
            namespace global;
            Space.value = 3;
        )");
        CHECK(e.Eval("x") == "undefined");
        CHECK(e.Eval("y") == "undefined");
        CHECK(e.Eval("len") == "2");
        CHECK(e.Eval("a.length") == "5");
        CHECK(e.Eval("a") == "{[0]:1,[1]:2,[4]:\"four\",['name']:\"n\"}");
        CHECK(e.Eval("before") == "undefined");
        CHECK(e.Eval("p") == "{v:1,other:2}");
        CHECK(e.Eval("Space.value") == "3");
        // Elements of other types are not created
        e.Run("var num = 5; num.x = 1; var str = \"s\"; str[9] = \"z\";");
        CHECK(e.Eval("num") == "5");
        CHECK(e.Eval("str") == "\"s\"");
    }

    TEST_CASE("each instance has its own default values") {
        Tests::Engine e;
        e.Run(R"(
            class Bag {
                var items = [];
                function add(x) { this.items[this.items.length] = x; }
            }
            var b1 = new Bag(), b2 = new Bag();
            b1.add(1);
            b1.add(2);
            b2.add(3);
        )");
        CHECK(e.Eval("b1.items") == "{[0]:1,[1]:2}");
        CHECK(e.Eval("b2.items") == "{[0]:3}");
    }

    TEST_CASE("containers can't be replaced, only the variables") {
        Tests::Engine e;
        e.Run(R"(
            class C { static var s = 1; }
            C = 5;
            namespace N;
            namespace global;
            N = 5;
            var a = [1];
            a = a;
            var b = a;
            a = b;
        )");
        CHECK(e.Eval("C.s") == "1");
        CHECK(e.Eval("N") == "{}");
        CHECK(e.Eval("a") == "{[0]:1}");
        CHECK(e.Eval("a == b") == "true");
    }

    TEST_CASE("description of recursive structures") {
        Tests::Engine e;
        e.Run("var a = [1]; a.self = a;");
        CHECK(e.Eval("a") == "{[0]:1,['self']:{...}}");
    }

    TEST_CASE("conversions") {
        Memory memory;
        CHECK(Convert(memory, memory.CreateNumber(0), Type::Boolean).As<bool>() == false);
        CHECK(Convert(memory, memory.CreateString("x"), Type::Boolean).As<bool>() == true);
        CHECK(Convert(memory, memory.CreateBoolean(true), Type::Number).As<Number>() == Number(1));
        CHECK(Convert(memory, memory.CreateString("1"), Type::Number).Is<Null>());
        CHECK(Convert(memory, memory.CreateNumber(2.5), Type::String).As<String>().ToUTF8() == "2.5");
        CHECK(Convert(memory, memory.CreateString("s"), Type::String).As<String>().ToUTF8() == "s");
        CHECK(Convert(memory, memory.Create(Array{}), Type::Array).Is<Array>());
        CHECK(Convert(memory, memory.Create(Array{}), Type::Namespace).Is<Null>());
        CHECK(IsTruthy(memory.CreateNumber(Number(std::numeric_limits<double>::quiet_NaN()))) == false);
        CHECK(IsTruthy(Variable()) == false);
    }

    TEST_CASE("operations on values of the application") {
        Memory memory;
        Variable a = memory.CreateNumber(2), b = memory.CreateNumber(3);
        auto result = Operate(memory, Operator::Multiply, a, b);
        REQUIRE(std::holds_alternative<Variable>(result));
        CHECK(std::get<Variable>(result).As<Number>() == Number(6));
        Operate(memory, Operator::Assign, a, b);
        CHECK(a.As<Number>() == Number(3));
        CHECK(GetMember(memory, memory.CreateString("abcd"), "length").As<Number>() == Number(4));
        CHECK(GetMember(memory, memory.CreateNumber(1), "x").Is<Undefined>());
        CHECK(Copy(memory, memory.CreateUndefined()).Is<Undefined>());
        Variable array = memory.CreateArray({ memory.CreateNumber(1) });
        Variable shared = Copy(memory, array);
        Variable cloned = Clone(memory, array);
        CHECK(shared->IsReference());
        CHECK(&shared->Target() == array.Get());
        CHECK(cloned.As<Array>().indexed.size() == 1);
        CHECK(cloned.As<Array>().indexed[0] != array.As<Array>().indexed[0]);
    }

}
