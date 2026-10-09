#include "helpers.hpp"

#include <sstream>
#include <stdexcept>

using namespace LiteScript;

namespace {

    // Execute a script that must raise an uncaught error and return it
    ScriptError ExpectError(Tests::Engine& e, std::string_view code) {
        auto compiled = Script::Compile(code);
        REQUIRE(compiled);
        e.state.AddInstructions(compiled.instructions);
        try {
            e.state.Execute();
        }
        catch (const ScriptError& error) {
            return error;
        }
        FAIL("no error raised");
        throw std::logic_error("unreachable");
    }

}

TEST_SUITE("errors") {

    TEST_CASE("errors of the engine") {
        Tests::Engine e;
        auto check = [&](std::string_view code, std::string_view message) {
            CAPTURE(code);
            ScriptError error = ExpectError(e, code);
            CHECK(std::string_view(error.what()) == message);
        };
        check("var a; a();", "a value of type null isn't a function");
        check("unknown(1);", "a value of type undefined isn't a function");
        check("var x = undefined; x.y;", "can't read the member \"y\" of undefined");
        check("var n = null; n[0];", "can't read an element of null");
        check("new (5)();", "a value of type number isn't a class");
        check("class P {} var p = new P(); p();", "the instance doesn't overload the call operator");
        // Values of other types give undefined
        CHECK(e.Eval("(5).x") == "undefined");
        CHECK(e.Eval("true[1]") == "undefined");
        CHECK(e.Eval("1 + \"a\"") == "null");
    }

    TEST_CASE("the position of the error") {
        Tests::Engine e;
        ScriptError error = ExpectError(e, "var a = 1;\nvar b = [1, 2];\nvar c = a + b.x.y;");
        CHECK(error.GetPosition().line == 2);
        CHECK(error.GetPosition().column == 15);
        Variable value = error.GetValue();
        REQUIRE(value.Is<Array>());
        CHECK(value.As<Array>().named.Find("line").ToString() == "3");
        CHECK(value.As<Array>().named.Find("column").ToString() == "16");
        CHECK(value.As<Array>().named.Find("message").ToString() == "\"can't read the member \"y\" of undefined\"");
    }

    TEST_CASE("the state is usable after an uncaught error") {
        Tests::Engine e;
        e.Run("function f(x) { print(\"in\"); return x.y.z; }");
        ExpectError(e, "print(\"before\"); f(1); print(\"not executed\");");
        CHECK(e.state.GetDepth() == 1);
        CHECK(e.state.GetStack().empty());
        CHECK(e.state.Finished());
        CHECK(e.Eval("1 + 1") == "2");
        CHECK(e.Output() == "before\nin");
    }

    TEST_CASE("throw") {
        Tests::Engine e;
        ScriptError error = ExpectError(e, "throw \"failure\";");
        CHECK(std::string_view(error.what()) == "failure");
        CHECK(error.GetValue().ToString() == "\"failure\"");
        error = ExpectError(e, "throw { message: \"custom\", code: 3 };");
        CHECK(std::string_view(error.what()) == "custom");
        CHECK(error.GetValue().As<Array>().named.Find("code").ToString() == "3");
        error = ExpectError(e, "class MyError { var message = \"from class\"; } throw new MyError();");
        CHECK(std::string_view(error.what()) == "from class");
    }

    TEST_CASE("try and catch") {
        Tests::Engine e;
        e.Run(R"(
            var log = "";
            try {
                log += "a";
                throw "oops";
                log += "b";
            }
            catch (e) {
                log += "c" + e;
            }
            log += "d";
            try {
                log += "e";
            }
            catch (e) {
                log += "f";
            }
            try {
                undefined.x;
            }
            catch {
                log += "g";
            }
            var message;
            try {
                var n = null;
                n();
            }
            catch (error) {
                message = error.message + " at " + error.line + ":" + error.column;
            }
        )");
        CHECK(e.Eval("log") == "\"acoopsdeg\"");
        CHECK(e.Eval("message") == "\"a value of type null isn't a function at 27:18\"");
        CHECK(e.state.GetStack().size() <= 1);
    }

    TEST_CASE("errors cross the functions") {
        Tests::Engine e;
        e.Run(R"(
            function inner(x) { if (x > 2) throw "too big: " + x; return x; }
            function middle(x) { var r = [1, 2, inner(x)]; return r; }
            function outer(x) {
                try {
                    return middle(x).length;
                }
                catch (e) {
                    return e;
                }
            }
            var ok = outer(1), ko = outer(5);
            var depth = 0;
            function recurse() { depth++; recurse(); }
            var stopped;
            try { recurse(); } catch (e) { stopped = e.message; }
        )");
        CHECK(e.Eval("ok") == "3");
        CHECK(e.Eval("ko") == "\"too big: 5\"");
        CHECK(e.Eval("stopped") == "\"maximum call depth exceeded\"");
        CHECK(e.state.GetDepth() == 1);
    }

    TEST_CASE("nested try and rethrow") {
        Tests::Engine e;
        e.Run(R"(
            var log = "";
            try {
                try {
                    throw 1;
                }
                catch (e) {
                    log += "inner" + e;
                    throw e + 1;
                }
            }
            catch (e) {
                log += " outer" + e;
            }
            try {
                try { log += " x"; } catch (e) { log += " never"; }
                throw 3;
            }
            catch (e) {
                log += " outer" + e;
            }
        )");
        CHECK(e.Eval("log") == "\"inner1 outer2 x outer3\"");
    }

    TEST_CASE("leaving a try block with return, break or continue") {
        Tests::Engine e;
        e.Run(R"(
            function f() { try { return 1; } catch (e) { return 2; } }
            var log = "";
            for (var i = 0; i < 4; i++) {
                try {
                    if (i == 1) continue;
                    if (i == 3) break;
                    log += i;
                }
                catch (e) {
                    log += "!";
                }
            }
            switch (1) {
                case 1:
                    try { break; } catch (e) {}
            }
            var r = f();
        )");
        CHECK(e.Eval("log") == "\"02\"");
        CHECK(e.Eval("r") == "1");
        // The try blocks that were left don't catch the next errors
        CHECK_THROWS_AS(e.Run("throw \"after\";"), ScriptError);
    }

    TEST_CASE("exceptions of native functions") {
        Tests::Engine e;
        e.state.DeclareVariable("fail", e.memory.CreateCallback([](State&, std::vector<Variable>&) -> Variable {
            throw std::invalid_argument("invalid argument");
        }));
        e.Run("var m; try { fail(); } catch (e) { m = e.message; }");
        CHECK(e.Eval("m") == "\"invalid argument\"");
        ScriptError error = ExpectError(e, "fail();");
        CHECK(std::string_view(error.what()) == "invalid argument");
    }

    TEST_CASE("errors in functions called by the application") {
        Tests::Engine e;
        e.state.DeclareVariable("apply", e.memory.CreateCallback([](State& s, std::vector<Variable>& args) {
            return s.Call(args.at(0), { args.at(1) });
        }));
        e.Run(R"(
            function check(x) { if (x < 0) throw "negative"; return x * 2; }
            var results = [];
            try {
                results[0] = apply(check, 4);
                results[1] = apply(check, -1);
            }
            catch (e) {
                results[2] = e;
            }
        )");
        CHECK(e.Eval("results") == "{[0]:8,[2]:\"negative\"}");
        // The application receives the errors of its calls
        CHECK_THROWS_WITH_AS(e.state.Call(e.state.GetVariable("check"), { e.memory.CreateNumber(-3) }), "negative", ScriptError);
        CHECK(e.state.GetDepth() == 1);
        CHECK(e.state.Call(e.state.GetVariable("check"), { e.memory.CreateNumber(3) }).ToString() == "6");
    }

    TEST_CASE("a state saved inside a try block catches the errors after the loading") {
        std::string data;
        {
            Tests::Engine e;
            e.state.DeclareVariable("save", e.memory.RegisterFunction("save", [&data](State& s, std::vector<Variable>&) {
                std::ostringstream stream(std::ios::binary);
                s.Save(stream);
                data = stream.str();
                return s.GetMemory().CreateUndefined();
            }));
            e.Run("function f() { try { save(); throw \"error\"; } catch (e) { print(\"caught\", e); } } f();");
            CHECK(e.Output() == "caught error");
        }
        Tests::Engine e;
        e.memory.RegisterFunction("save", [](State& s, std::vector<Variable>&) { return s.GetMemory().CreateUndefined(); });
        std::istringstream stream(data, std::ios::binary);
        e.state.Load(stream);
        e.state.Execute();
        CHECK(e.Output() == "caught error");
    }

    TEST_CASE("errors in assembly") {
        Tests::Engine e;
        auto result = Assembly::Compile(
            "try.begin 5\n"
            "value.string bad\n"
            "throw\n"
            "try.end\n"
            "define.variable caught\n"
            "value.null\n"
            "operator.call\n");
        REQUIRE(result);
        e.state.AddInstructions(result.instructions);
        try {
            e.state.Execute();
            FAIL("no error raised");
        }
        catch (const ScriptError& error) {
            // The line of the assembly code is the position of the error
            CHECK(error.GetPosition().line == 6);
        }
        CHECK(e.Eval("caught") == "\"bad\"");
        CHECK(Assembly::Disassemble(result.instructions).find("try.begin 5\n") == 0);
    }

    TEST_CASE("syntax of try and throw") {
        CHECK(Script::Compile("try { } catch (e) { }"));
        CHECK(Script::Compile("try { } catch { }"));
        CHECK(Script::Compile("try { } ").error == Script::Error::Catch);
        CHECK(Script::Compile("try throw 1; catch { }").error == Script::Error::BraceOpen);
        CHECK(Script::Compile("try { } catch (1) { }").error == Script::Error::Name);
        CHECK(Script::Compile("throw;").error == Script::Error::Expression);
        CHECK(Script::Compile("var try = 1;").error == Script::Error::Name);
    }

    TEST_CASE("errors don't leak memory") {
        CHECK(Tests::LeakedAllocations([] {
            Memory memory;
            State state(memory);
            auto compiled = Script::Compile(R"(
                function f(x) { var a = [x]; a[1] = a; if (x > 3) throw a; return f(x + 1); }
                try { f(0); } catch (e) { e[2] = e; }
                f(0);
            )");
            state.AddInstructions(compiled.instructions);
            try {
                state.Execute();
            }
            catch (const ScriptError&) {
            }
        }) == 0);
    }

}
