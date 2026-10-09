#include "helpers.hpp"

#include <sstream>
#include <stdexcept>

using namespace LiteScript;

namespace {

    // Scripts that create every kind of object and of reference cycle
    constexpr std::string_view SCRIPTS[] = {
        "var a = [1, [2, [3]], { k: \"v\" }]; a.self = a; a[1][1][0] = a;",
        "function f(x) { var inner = function() { return x; }; return inner; } var g = f(3); g();",
        "class A { static var all = []; var me; function A() { this.me = this; A.all[A.all.length] = this; } } "
        "var x = new A(); var y = new A();",
        "class B : (class { var v = 1; }) { operator add(o) { return new B(); } } var b = new B() + new B();",
        "namespace N.M; var v = 1; function f() { return N; } namespace global; var n = N.M.f();",
        "var s = \"abc\"; var c = s[1]; c = \"z\"; s[0] += c; var r = s * 3;",
        "function fibo(n) { return n < 2 ? 1 : fibo(n - 1) + fibo(n - 2); } var f = fibo(15);",
        "var t = 0; for (var i = 0; i < 100; i++) { switch (i % 3) { case 0: continue; case 1: t += i; break; default: t -= 1; } }",
        "function v(a, b...) { return b; } var r = v(1, 2, 3, [4]);",
        "var u = undefined | null | 0; var w = !u && ~5;",
        // Syntax error: nothing is executed
        "var a = [1, 2; var b = 3;",
    };

    void RunScript(std::string_view code, std::size_t gc_threshold) {
        Memory memory(gc_threshold);
        State state(memory);
        state.DeclareVariable("print", memory.RegisterFunction("print", [](State& s, std::vector<Variable>&) {
            return s.GetMemory().CreateUndefined();
        }));
        Script::Execute(state, code);
    }

}

TEST_SUITE("leaks") {

    TEST_CASE("scripts don't leak memory") {
        for (std::string_view code : SCRIPTS) {
            CAPTURE(code);
            // Without garbage collector, the cycles are destroyed with the memory
            CHECK(Tests::LeakedAllocations([&] { RunScript(code, 0); }) == 0);
            // With a garbage collector executed very often
            CHECK(Tests::LeakedAllocations([&] { RunScript(code, 1); }) == 0);
        }
    }

    TEST_CASE("the garbage collector releases the cycles of a running state") {
        Memory memory(0);
        State state(memory);
        Script::Execute(state, "function f() { var a = []; a[0] = a; var g = function() { return a; }; } f(); f(); f();");
        std::size_t before = memory.Count();
        CHECK(memory.CollectGarbage() > 0);
        CHECK(memory.Count() < before);
        // Nothing that the state still uses has been destroyed
        auto result = Script::Execute(state, "return f;");
        CHECK(result.value.Is<Callback>());
        CHECK(memory.CollectGarbage() == 0);
    }

    TEST_CASE("states saved and loaded don't leak memory") {
        CHECK(Tests::LeakedAllocations([] {
            std::string data;
            {
                Memory memory;
                State state(memory);
                Script::Execute(state, SCRIPTS[0]);
                Script::Execute(state, SCRIPTS[2]);
                std::ostringstream out(std::ios::binary);
                state.Save(out);
                data = out.str();
            }
            Memory memory;
            State state(memory);
            std::istringstream in(data, std::ios::binary);
            state.Load(in);
            // Failed loadings don't leak either
            for (std::size_t size = 0; size < data.size(); size += 11) {
                std::istringstream truncated(data.substr(0, size), std::ios::binary);
                try {
                    state.Load(truncated);
                }
                catch (const SerializationError&) {
                }
            }
        }) == 0);
    }

    TEST_CASE("exceptions don't leak memory") {
        CHECK(Tests::LeakedAllocations([] {
            Memory memory;
            State state(memory);
            state.DeclareVariable("fail", memory.CreateCallback([](State&, std::vector<Variable>&) -> Variable {
                throw std::runtime_error("failure");
            }));
            Script::Execute(state, "function f(x) { var a = [x]; a[1] = a; return fail(a); }");
            try {
                state.Call(state.GetVariable("f"), { memory.CreateNumber(1) });
            }
            catch (const std::runtime_error&) {
            }
        }) == 0);
    }

}
