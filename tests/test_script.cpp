#include "helpers.hpp"

using namespace LiteScript;

namespace {

    // Compile a code that must fail and return the error
    Script::Result CompileError(std::string_view code) {
        Script::Result result = Script::Compile(code);
        CHECK_FALSE(result);
        CHECK(result.instructions.empty());
        return result;
    }

}

TEST_SUITE("script") {

    TEST_CASE("variables") {
        Tests::Engine e;
        e.Run("var a; var b = 5.2, c, d = \"x\";");
        CHECK(e.Eval("a") == "null");
        CHECK(e.Eval("b") == "5.2");
        CHECK(e.Eval("c") == "null");
        CHECK(e.Eval("d") == "\"x\"");
        CHECK(e.Eval("unknown") == "undefined");
        // A variable can be declared again
        e.Run("var a = 1; var a = 2;");
        CHECK(e.Eval("a") == "2");
    }

    TEST_CASE("comments, line breaks and unicode") {
        Tests::Engine e;
        e.Run("// comment\r\nvar a = 1; /* block\r\n comment */ var b = 'h\\u00e9\\u{1F600}';\r\nvar c = \"\xE3\x81\x93\";");
        CHECK(e.Eval("a") == "1");
        CHECK(e.Eval("b") == "\"h\xC3\xA9\xF0\x9F\x98\x80\"");
        CHECK(e.Eval("c.length") == "1");
        CHECK(e.Eval("'a\\'b\\n'") == "\"a'b\n\"");
    }

    TEST_CASE("keywords are whole words") {
        Tests::Engine e;
        e.Run(R"(
            var iffy = 1, newValue = 2, elsewhere = 3, variable = 4, returned = 5, classes = 6, functional = 7;
            if (iffy == 1) iffy = 10;
            elsewhere = 30;
            var r = newValue + variable + returned + classes + functional;
        )");
        CHECK(e.Eval("iffy") == "10");
        CHECK(e.Eval("elsewhere") == "30");
        CHECK(e.Eval("r") == "24");
    }

    TEST_CASE("functions") {
        Tests::Engine e;
        e.Run(R"(
            function add(a, b) {
                return a + b;
            }
            var sub = function(a, b) { return a - b; };
            function nothing() {}
            function early(x) {
                if (x)
                    return "yes";
                return;
            }
            var immediate = (function(x) { return x * 2; })(21);
        )");
        CHECK(e.Eval("add(2, 3)") == "5");
        CHECK(e.Eval("sub(2, 3)") == "-1");
        CHECK(e.Eval("nothing()") == "undefined");
        CHECK(e.Eval("early(true)") == "\"yes\"");
        CHECK(e.Eval("early(false)") == "undefined");
        CHECK(e.Eval("immediate") == "42");
        CHECK(e.Eval("add(1)") == "null");
        CHECK(e.Eval("add(1, 2, 3)") == "3");
        CHECK(e.Eval("add(add(1, 2), add(3, 4))") == "10");
        CHECK_THROWS_AS(e.Eval("5()"), ScriptError);
    }

    TEST_CASE("recursion") {
        Tests::Engine e;
        e.Run(R"(
            function fibo(x) {
                if (x < 2)
                    return 1;
                else
                    return fibo(x - 1) + fibo(x - 2);
            }
            print(fibo(5));
        )");
        CHECK(e.Output() == "8");
        CHECK(e.Eval("fibo(20)") == "10946");
    }

    TEST_CASE("arguments are local copies") {
        Tests::Engine e;
        e.Run(R"(
            var x = 1;
            function change(x) { x = 5; x++; return x; }
            var r = change(x);
            function shadow() { var x = 100; return x; }
            var s = shadow();
        )");
        CHECK(e.Eval("x") == "1");
        CHECK(e.Eval("r") == "6");
        CHECK(e.Eval("s") == "100");
    }

    TEST_CASE("variadic arguments") {
        Tests::Engine e;
        e.Run(R"(
            function f1(args...) {
                for (var i = 0; i < args.length; i++)
                    print(args[i]);
            }
            function f2(a, b, c...) {
                print(a, b, c.length, c[0]);
            }
            f1(1, 2, 3);
            f2(1, 2, 3);
            f2(1);
        )");
        CHECK(e.Output() == "1\n2\n3\n1 2 1 3\n1 undefined 0 undefined");
    }

    TEST_CASE("closures see the variables of their scope") {
        Tests::Engine e;
        e.Run(R"(
            function counter() {
                var count = 0;
                return function() { count++; return count; };
            }
            var c1 = counter(), c2 = counter();
            c1(); c1();
            var r = c1() * 10 + c2();
        )");
        CHECK(e.Eval("r") == "31");
    }

    TEST_CASE("methods of arrays are bound to the array") {
        // Example of the documentation
        Tests::Engine e;
        e.Run(R"(
            var tab = [
                3, 2, 4, 1
            ] + {
                trier: function() {
                    for (var i = 0; i < this.length - 1; i++) {
                        var min = i;
                        for (var j = i + 1; j < this.length; j++) {
                            if (this[j] < this[min])
                                min = j;
                        }
                        if (min != i) {
                            var tmp = this[i];
                            this[i] = this[min];
                            this[min] = tmp;
                        }
                    }
                }
            };
            tab.trier();
        )");
        CHECK(e.Eval("[tab[0], tab[1], tab[2], tab[3]]") == "{[0]:1,[1]:2,[2]:3,[3]:4}");
    }

    TEST_CASE("conditions") {
        Tests::Engine e;
        e.Run(R"(
            function sign(x) {
                if (x > 0) return 1;
                else if (x < 0) return -1;
                else return 0;
            }
            var t = 5 > 3 ? "yes" : "no";
            var n = 0 ? 1 : null ? 2 : 3;
            var a;
            a = true ? 1 + 1 : 3;
        )");
        CHECK(e.Eval("[sign(5), sign(-2), sign(0)]") == "{[0]:1,[1]:-1,[2]:0}");
        CHECK(e.Eval("t") == "\"yes\"");
        CHECK(e.Eval("n") == "3");
        CHECK(e.Eval("a") == "2");
        CHECK(e.Eval("1 + (false ? 10 : 20)") == "21");
    }

    TEST_CASE("loops") {
        Tests::Engine e;
        e.Run(R"(
            var s = 0;
            for (var i = 0; i < 10; i++) {
                if (i == 2) continue;
                if (i == 6) break;
                s += i;
            }
            var w = 0;
            while (w < 100) w += 7;
            var d = 0, n = 0;
            do {
                n++;
                if (n == 3) continue;
                d += n;
            } while (n < 5);
            var k = 0;
            for (;;) { if (++k >= 4) break; }
            var nested = 0;
            for (var x = 0; x < 3; x++)
                for (var y = 0; y < 3; y++) {
                    if (y == 1) break;
                    nested++;
                }
        )");
        CHECK(e.Eval("s") == "13");
        CHECK(e.Eval("w") == "105");
        CHECK(e.Eval("d") == "12");
        CHECK(e.Eval("k") == "4");
        CHECK(e.Eval("nested") == "3");
    }

    TEST_CASE("switch") {
        Tests::Engine e;
        e.Run(R"(
            function name(v) {
                var r = "";
                switch (v) {
                    case 1:
                    case 2:
                        r += "small";
                        break;
                    case 1 + 2:
                        r += "three";
                    case "four":
                        r += "+";
                        break;
                    default:
                        r += "other";
                }
                return r;
            }
            var total = 0;
            for (var i = 0; i < 5; i++) {
                switch (i) {
                    case 1: continue;
                    case 3: total += 100;
                }
                total += i;
            }
        )");
        CHECK(e.Eval("name(1)") == "\"small\"");
        CHECK(e.Eval("name(2)") == "\"small\"");
        CHECK(e.Eval("name(3)") == "\"three+\"");
        CHECK(e.Eval("name(\"four\")") == "\"+\"");
        CHECK(e.Eval("name(9)") == "\"other\"");
        CHECK(e.Eval("total") == "109");
        // The switch values don't stay on the stack
        CHECK(e.state.GetStack().size() <= 1);
    }

    TEST_CASE("classes") {
        Tests::Engine e;
        e.Run(R"(
            class Counter {
                static var instances = 0;
                var value = 0, step;
                function Counter(step) {
                    this.step = step;
                    instances++;
                }
                function next() {
                    this.value += this.step;
                    return this.value;
                }
                static function create() {
                    return new Counter(10);
                }
            }
            var a = new Counter(1), b = Counter.create();
            a.next(); a.next(); b.next();
        )");
        CHECK(e.Eval("a.value") == "2");
        CHECK(e.Eval("b.value") == "10");
        CHECK(e.Eval("Counter.instances") == "2");
        CHECK(e.Eval("a") == "{value:2,step:1}");
        CHECK(e.Eval("a.unknown") == "undefined");
        CHECK(e.Eval("a == a") == "true");
    }

    TEST_CASE("inheritance") {
        Tests::Engine e;
        e.Run(R"(
            class Alive {
                var life = 0;
                function Alive() { this.life = 100; }
                function describe() { return "alive"; }
                function hurt(x) { this.life -= x; }
            }
            class Person : Alive {
                var name;
                function Person(name) {
                    this.Alive();
                    this.name = name;
                }
                function describe() { return this.name + " is " + this.life; }
            }
            var p = new Person("Anne");
            p.hurt(30);
        )");
        CHECK(e.Eval("p.describe()") == "\"Anne is 70\"");
        CHECK(e.Eval("(new Alive()).describe()") == "\"alive\"");
    }

    TEST_CASE("overloaded operators") {
        // Example of the documentation
        Tests::Engine e;
        e.Run(R"(
            class Point {
                var x = 0,
                    y = 0;
                function Point(x, y) {
                    if (x != undefined)
                        this.x = x;
                    if (y != undefined)
                        this.y = y;
                }
                operator add(p) {
                    if (p.x == undefined || p.y == undefined)
                        return null;
                    else
                        return new Point(this.x + p.x, this.y + p.y);
                }
                operator equal(p) { return this.x == p.x && this.y == p.y; }
                operator not_equal(p) { return "not equal called"; }
                operator get(i) { return i == 0 ? this.x : this.y; }
                operator call(a, b) { return a * b + this.x; }
                operator pre_increment() { this.x++; return this; }
                operator unary_minus() { return new Point(-this.x, -this.y); }
                operator add_assign(p) { this.x += p; return this; }
            }
            var a = new Point(3),
                b = new Point(6, 4),
                c = a + b;
            var d = 5, e = new Point(4), f = d + e;
            var g = a + 1;
        )");
        CHECK(e.Eval("c") == "{x:9,y:4}");
        CHECK(e.Eval("f") == "null");
        CHECK(e.Eval("g") == "null");
        CHECK(e.Eval("c == new Point(9, 4)") == "true");
        CHECK(e.Eval("c != b") == "\"not equal called\"");
        CHECK(e.Eval("[c[0], c[1]]") == "{[0]:9,[1]:4}");
        CHECK(e.Eval("c(2, 5)") == "19");
        CHECK(e.Eval("(-c).x") == "-9");
        e.Run("++a; a += 10;");
        CHECK(e.Eval("a.x") == "14");
        // An overloaded operator inside an expression of a function
        e.Run("function sum(p, q) { var r = p + q; return r.x * 100 + r.y; }");
        CHECK(e.Eval("sum(a, b)") == "2004");
    }

    TEST_CASE("assignment operator overloading") {
        Tests::Engine e;
        e.Run(R"(
            class Box {
                var content = 0;
                operator assign(v) { this.content = v; return this; }
            }
            var b = new Box();
            b = 5;
            var copy = b;
        )");
        CHECK(e.Eval("b.content") == "5");
        CHECK(e.Eval("copy.content") == "5");
    }

    TEST_CASE("anonymous and nested classes") {
        Tests::Engine e;
        e.Run(R"(
            var A = class {
                var v;
                function constructor(v) { this.v = v; }
            };
            var instance = new (class {
                var a = 1;
                function constructor() { this.a = 2; }
            })();
            class Outer {
                class Inner {
                    var n = 7;
                }
                static function make() { return new Inner(); }
            }
            var inner = Outer.make();
            var other = new Outer.Inner;
        )");
        CHECK(e.Eval("(new A(4)).v") == "4");
        CHECK(e.Eval("instance.a") == "2");
        CHECK(e.Eval("inner.n") == "7");
        CHECK(e.Eval("other.n") == "7");
        CHECK_THROWS_AS(e.Eval("new 5"), ScriptError);
    }

    TEST_CASE("namespaces") {
        // Example of the documentation
        Tests::Engine e;
        e.Run(R"(
            namespace Math;
            function add(a, b) {
                return a + b;
            }
            namespace Game.Engine;
            var version = 2;
            namespace global;
            var a = Math.add(3, 2);
        )");
        CHECK(e.Eval("a") == "5");
        CHECK(e.Eval("Game.Engine.version") == "2");
        CHECK(e.Eval("add") == "undefined");
        e.Run("namespace Math; var inside = add(1, 1); namespace global;");
        CHECK(e.Eval("Math.inside") == "2");
    }

    TEST_CASE("return at top level stops the script") {
        Tests::Engine e;
        Variable v = e.Run("print(1); return 42; print(2);");
        CHECK(v.ToString() == "42");
        CHECK(e.Output() == "1");
    }

    TEST_CASE("syntax errors") {
        auto r = CompileError("var a = 1;\nvar b = ;");
        CHECK(r.error == Script::Error::Expression);
        CHECK(r.line == 1);
        CHECK(r.column == 8);

        CHECK(CompileError("var a = 1").error == Script::Error::Semicolon);
        CHECK(CompileError("var = 1;").error == Script::Error::Name);
        CHECK(CompileError("var if = 1;").error == Script::Error::Name);
        CHECK(CompileError("f(1, 2;").error == Script::Error::ParenthesisClose);
        CHECK(CompileError("a[1;").error == Script::Error::BracketClose);
        CHECK(CompileError("if 1;").error == Script::Error::ParenthesisOpen);
        CHECK(CompileError("if (1;").error == Script::Error::ParenthesisClose);
        CHECK(CompileError("{ var a;").error == Script::Error::BraceClose);
        CHECK(CompileError("var s = \"abc;").error == Script::Error::UnterminatedString);
        CHECK(CompileError("break;").error == Script::Error::BreakOutsideLoop);
        CHECK(CompileError("while (1) { function f() { continue; } }").error == Script::Error::ContinueOutsideLoop);
        CHECK(CompileError("switch (1) { case 1: continue; }").error == Script::Error::ContinueOutsideLoop);
        CHECK(CompileError("switch (1) { foo; }").error == Script::Error::SwitchKeywords);
        CHECK(CompileError("do {} until (1);").error == Script::Error::While);
        CHECK(CompileError("class A { var a }").error == Script::Error::Semicolon);
        CHECK(CompileError("class A { operator plus() {} }").error == Script::Error::ClassOperatorInvalid);
        CHECK(CompileError("class A { 5; }").error == Script::Error::ClassInvalid);
        CHECK(CompileError("class A ").error == Script::Error::ClassBegin);
        CHECK(CompileError("function f(a, ) {}").error == Script::Error::CallbackArgumentName);
        CHECK(CompileError("function f(a b) {}").error == Script::Error::CallbackArgumentEnd);
        CHECK(CompileError("function f() return 1;").error == Script::Error::CallbackInstructions);
        CHECK(CompileError("var a = { x 1 };").error == Script::Error::NamedArrayColon);
        CHECK(CompileError("var a = [1 2];").error == Script::Error::SimpleArrayEnd);
        CHECK(CompileError("a ! b;").error == Script::Error::Operator);
        CHECK(CompileError("a + ;").error == Script::Error::Value);
        CHECK(CompileError("a ? b ;").error == Script::Error::Colon);
        CHECK(CompileError("namespace a.;").error == Script::Error::Namespace);
        CHECK(CompileError("@").error == Script::Error::Unknown);
        CHECK(CompileError("if (1)").error == Script::Error::Instruction);
        CHECK_FALSE(Script::GetErrorMessage(Script::Error::Semicolon).empty());
    }

    TEST_CASE("the column of errors is counted in characters") {
        auto r = CompileError("var \xC3\xA9t\xC3\xA9 = 1;");
        CHECK(r.line == 0);
        CHECK(r.column == 4);
    }

    TEST_CASE("deep nesting is rejected without crashing") {
        std::string code = "var a = " + std::string(10000, '(') + "1" + std::string(10000, ')') + ";";
        CHECK(CompileError(code).error == Script::Error::Nesting);
        std::string blocks = std::string(10000, '{') + std::string(10000, '}');
        CHECK(CompileError(blocks).error == Script::Error::Nesting);
        // A reasonable nesting is accepted
        std::string ok = "var a = " + std::string(50, '(') + "1" + std::string(50, ')') + ";";
        CHECK(Script::Compile(ok));
    }

    TEST_CASE("an error doesn't execute anything") {
        Tests::Engine e;
        auto result = Script::Execute(e.state, "print(1); var = ;");
        CHECK_FALSE(result);
        CHECK(e.output.empty());
        CHECK(e.state.GetInstructionListCount() == 0);
    }

}
