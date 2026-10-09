#include "helpers.hpp"

#include <sstream>

using namespace LiteScript;

namespace {

    // An engine with the standard library
    struct StandardEngine : Tests::Engine {
        StandardEngine() {
            Standard::Install(this->state);
        }
    };

}

TEST_SUITE("standard") {

    TEST_CASE("the library is optional") {
        Tests::Engine e;
        CHECK(e.Eval("Math") == "undefined");
        CHECK(e.Eval("[1].pop") == "undefined");
        CHECK(e.Eval("\"a\".trim") == "undefined");
        CHECK_THROWS_AS(e.Eval("[1].pop()"), ScriptError);
    }

    TEST_CASE("Math") {
        StandardEngine e;
        CHECK(e.Eval("Math.abs(-3)") == "3");
        CHECK(e.Eval("Math.abs(-2.5)") == "2.5");
        CHECK(e.Eval("Math.floor(2.7)") == "2");
        CHECK(e.Eval("Math.ceil(2.1)") == "3");
        CHECK(e.Eval("Math.round(2.5)") == "3");
        CHECK(e.Eval("Math.trunc(-2.7)") == "-2");
        CHECK(e.Eval("Math.sign(-4)") == "-1");
        CHECK(e.Eval("Math.sqrt(16)") == "4");
        CHECK(e.Eval("Math.pow(2, 10)") == "1024");
        CHECK(e.Eval("Math.min(4, 2, 8)") == "2");
        CHECK(e.Eval("Math.max(4, 2.5, 8)") == "8");
        CHECK(e.Eval("Math.max()") == "null");
        CHECK(e.Eval("Math.max(1, \"a\")") == "null");
        CHECK(e.Eval("Math.sqrt(\"x\")") == "null");
        CHECK(e.Eval("Math.floor(Math.PI * 100)") == "314");
        CHECK(e.Eval("Math.round(Math.cos(0))") == "1");
        CHECK(e.Eval("Math.atan2(0, 1)") == "0");
    }

    TEST_CASE("random numbers") {
        StandardEngine e;
        e.Run(R"(
            Math.seed(42);
            var ok = true;
            for (var i = 0; i < 200; i++) {
                var r = Math.random(), d = Math.random(6), b = Math.random(-2, 2);
                if (r < 0 || r >= 1 || d < 0 || d > 5 || Math.floor(d) != d || b < -2 || b > 2)
                    ok = false;
            }
            Math.seed(7);
            var first = Math.random(1000);
            Math.seed(7);
            var second = Math.random(1000);
        )");
        CHECK(e.Eval("ok") == "true");
        CHECK(e.Eval("first == second") == "true");
        CHECK(e.Eval("Math.random(3, 1)") == "null");
    }

    TEST_CASE("conversions") {
        StandardEngine e;
        CHECK(e.Eval("typeOf(1)") == "\"number\"");
        CHECK(e.Eval("typeOf([])") == "\"array\"");
        CHECK(e.Eval("typeOf(x)") == "\"undefined\"");
        CHECK(e.Eval("toString(12) + toString(\"a\")") == "\"12a\"");
        CHECK(e.Eval("parseNumber(\" 42 \")") == "42");
        CHECK(e.Eval("parseNumber(\"-1.5e2\")") == "-150");
        CHECK(e.Eval("parseNumber(\"12px\")") == "null");
        CHECK(e.Eval("parseNumber(true)") == "1");
        CHECK(e.Eval("parseNumber(null)") == "null");
        CHECK(e.Eval("parseInt(\"ff\", 16)") == "255");
        CHECK(e.Eval("parseInt(\"101\", 2)") == "5");
        CHECK(e.Eval("parseInt(7.9)") == "7");
        CHECK(e.Eval("parseInt(\"1.5\")") == "null");
        CHECK(e.Eval("parseInt(\"1\", 99)") == "null");
        CHECK(e.Eval("isNaN(0 / 0)") == "true");
        CHECK(e.Eval("isNaN(1)") == "false");
        CHECK(e.Eval("char(233) + char(65)") == "\"\xC3\xA9" "A\"");
        CHECK(e.Eval("char(-1)") == "null");
    }

    TEST_CASE("methods of arrays") {
        StandardEngine e;
        e.Run(R"(
            var a = [3, 1, 2];
            var length = a.push(5, 4);
            var last = a.pop();
            a.insert(0, 9);
            var removed = a.remove(1);
            var negative = a.remove(-1);
        )");
        CHECK(e.Eval("length") == "5");
        CHECK(e.Eval("last") == "4");
        CHECK(e.Eval("removed") == "3");
        CHECK(e.Eval("negative") == "5");
        CHECK(e.Eval("a") == "{[0]:9,[1]:1,[2]:2}");
        CHECK(e.Eval("a.indexOf(2)") == "2");
        CHECK(e.Eval("a.indexOf(7)") == "-1");
        CHECK(e.Eval("a.contains(9)") == "true");
        CHECK(e.Eval("a.join(\"-\")") == "\"9-1-2\"");
        CHECK(e.Eval("[1, null, \"x\"].join()") == "\"1,,x\"");
        CHECK(e.Eval("a.slice(1)") == "{[0]:1,[1]:2}");
        CHECK(e.Eval("a.slice(-2, -1)") == "{[0]:1}");
        CHECK(e.Eval("[1, 2, 3].reverse()") == "{[0]:3,[1]:2,[2]:1}");
        CHECK(e.Eval("[].pop()") == "undefined");
        CHECK(e.Eval("[1].remove(5)") == "undefined");
        CHECK(e.Eval("{ a: 1, b: 2 }.keys()") == "{[0]:\"a\",[1]:\"b\"}");
        e.Run("a.clear();");
        CHECK(e.Eval("a.length") == "0");
        // The elements of the array have the priority over the methods
        CHECK(e.Eval("{ push: 5 }.push") == "5");
    }

    TEST_CASE("sort") {
        StandardEngine e;
        CHECK(e.Eval("[5, 2, 9, 1, 2].sort()") == "{[0]:1,[1]:2,[2]:2,[3]:5,[4]:9}");
        CHECK(e.Eval("[\"b\", \"c\", \"a\"].sort()") == "{[0]:\"a\",[1]:\"b\",[2]:\"c\"}");
        CHECK(e.Eval("[5, 2, 9].sort(function(a, b) { return b - a; })") == "{[0]:9,[1]:5,[2]:2}");
        // Objects sorted by a member, the sort is stable
        e.Run(R"(
            var people = [{ n: "a", age: 30 }, { n: "b", age: 20 }, { n: "c", age: 30 }, { n: "d", age: 10 }];
            people.sort(function(x, y) { return x.age - y.age; });
            var names = people.map(function(p) { return p.n; }).join("");
        )");
        CHECK(e.Eval("names") == "\"dbac\"");
        // An inconsistent comparison doesn't break the array
        CHECK(e.Eval("[3, 1, 2, 5, 4].sort(function(a, b) { return Math.random(-1, 1); }).length") == "5");
    }

    TEST_CASE("functional methods") {
        StandardEngine e;
        e.Run(R"(
            var squares = [1, 2, 3].map(function(x) { return x * x; });
            var even = [1, 2, 3, 4].filter(function(x, i) { return x % 2 == 0; });
            var sum = 0;
            [10, 20].forEach(function(x, i) { sum += x + i; });
        )");
        CHECK(e.Eval("squares") == "{[0]:1,[1]:4,[2]:9}");
        CHECK(e.Eval("even") == "{[0]:2,[1]:4}");
        CHECK(e.Eval("sum") == "31");
        CHECK_THROWS_AS(e.Eval("[1].map(5)"), ScriptError);
        // An error in the callback can be caught by the script
        e.Run("var m; try { [1].forEach(function(x) { throw \"stop\"; }); } catch (err) { m = err; }");
        CHECK(e.Eval("m") == "\"stop\"");
    }

    TEST_CASE("methods of strings") {
        StandardEngine e;
        CHECK(e.Eval("\"hello\".substring(1, 3)") == "\"el\"");
        CHECK(e.Eval("\"hello\".substring(3)") == "\"lo\"");
        CHECK(e.Eval("\"hello\".substring(4, 1)") == "\"ell\"");
        CHECK(e.Eval("\"hello\".indexOf(\"l\")") == "2");
        CHECK(e.Eval("\"hello\".indexOf(\"l\", 3)") == "3");
        CHECK(e.Eval("\"hello\".indexOf(\"z\")") == "-1");
        CHECK(e.Eval("\"hello\".contains(\"ell\")") == "true");
        CHECK(e.Eval("\"hello\".startsWith(\"he\")") == "true");
        CHECK(e.Eval("\"hello\".endsWith(\"lo\")") == "true");
        CHECK(e.Eval("\"a,b,,c\".split(\",\")") == "{[0]:\"a\",[1]:\"b\",[2]:\"\",[3]:\"c\"}");
        CHECK(e.Eval("\"abc\".split(\"\")") == "{[0]:\"a\",[1]:\"b\",[2]:\"c\"}");
        CHECK(e.Eval("\"abc\".split()") == "{[0]:\"abc\"}");
        CHECK(e.Eval("\"a-b-c\".replace(\"-\", \"+\")") == "\"a+b+c\"");
        CHECK(e.Eval("\"  x y \\n\".trim()") == "\"x y\"");
        CHECK(e.Eval("\"\xC3\xA9t\xC3\xA9 Ok\".toUpperCase()") == "\"\xC3\x89T\xC3\x89 OK\"");
        CHECK(e.Eval("\"ABC\".toLowerCase()") == "\"abc\"");
        CHECK(e.Eval("\"A\".charCode()") == "65");
        CHECK(e.Eval("\"A\".charCode(3)") == "undefined");
        // The characters have the methods of the strings
        CHECK(e.Eval("\"abc\"[1].toUpperCase()") == "\"B\"");
        // The string isn't modified
        e.Run("var s = \"abc\"; var t = s.toUpperCase();");
        CHECK(e.Eval("s + t") == "\"abcABC\"");
    }

    TEST_CASE("a state that uses the library can be saved") {
        std::string data;
        {
            StandardEngine e;
            e.Run("var a = [3, 1, 2]; var f = a.sort; var m = Math.max;");
            std::ostringstream stream(std::ios::binary);
            e.state.Save(stream);
            data = stream.str();
        }
        StandardEngine e;
        std::istringstream stream(data, std::ios::binary);
        e.state.Load(stream);
        CHECK(e.Eval("f()") == "{[0]:1,[1]:2,[2]:3}");
        CHECK(e.Eval("m(1, 7)") == "7");
        CHECK(e.Eval("Math.min(1, 7)") == "1");
    }

    TEST_CASE("the library doesn't leak memory") {
        CHECK(Tests::LeakedAllocations([] {
            Memory memory;
            State state(memory);
            Standard::Install(state);
            Script::Execute(state, R"(
                var a = [3, 1, 2];
                a.push(a);
                a.sort(function(x, y) { return 0; });
                var s = "a,b".split(",").map(function(x) { return x.toUpperCase(); }).join("");
            )");
        }) == 0);
    }

}
