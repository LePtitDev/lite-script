#include "helpers.hpp"

#include <sstream>
#include <stdexcept>

using namespace LiteScript;

TEST_SUITE("state") {

    TEST_CASE("execution instruction by instruction") {
        Tests::Engine e;
        auto result = Script::Compile("var a = 1; a += 2; print(a);");
        REQUIRE(result);
        e.state.AddInstructions(result.instructions);
        std::size_t steps = 0;
        while (!e.state.Finished()) {
            e.state.ExecuteSingle();
            steps++;
        }
        CHECK(steps == result.instructions.size());
        CHECK(e.Output() == "3");
        CHECK(e.state.Finished());
        // Executing a finished state does nothing
        CHECK(e.state.ExecuteSingle().Is<Undefined>());
    }

    TEST_CASE("several lists of instructions share the variables") {
        Tests::Engine e;
        e.Run("function twice(x) { return x * 2; }");
        e.Run("var r = twice(4);");
        CHECK(e.Eval("r") == "8");
        CHECK(e.state.GetInstructionListCount() == 3);
    }

    TEST_CASE("an empty state is finished") {
        Memory memory;
        State state(memory);
        CHECK(state.Finished());
        CHECK(state.Execute().Is<Undefined>());
        state.AddInstructions({});
        CHECK(state.Finished());
    }

    TEST_CASE("native functions") {
        Tests::Engine e;
        e.state.DeclareVariable("sum", e.memory.CreateCallback([](State& s, std::vector<Variable>& args) {
            Number total;
            for (const Variable& v : args) {
                if (v.Is<Number>())
                    total += v.As<Number>();
            }
            CHECK(s.GetArgsCount() == args.size());
            return s.GetMemory().CreateNumber(total);
        }));
        e.state.DeclareVariable("self", e.memory.CreateCallback([](State& s, std::vector<Variable>&) {
            return s.GetThis();
        }));
        e.state.DeclareVariable("empty", e.memory.CreateCallback([](State&, std::vector<Variable>&) {
            return Variable();
        }));
        CHECK(e.Eval("sum(1, 2, 3.5)") == "6.5");
        CHECK(e.Eval("sum()") == "0");
        CHECK(e.Eval("self()") == "undefined");
        CHECK(e.Eval("empty()") == "undefined");
        e.Run("var a = { f: self, v: 3 };");
        CHECK(e.Eval("a.f().v") == "3");
    }

    TEST_CASE("the application can call script functions") {
        Tests::Engine e;
        e.Run(R"(
            function add(a, b) { return a + b; }
            function fact(n) { return n <= 1 ? 1 : n * fact(n - 1); }
            class P {
                var x = 1;
                operator call(y) { return this.x + y; }
            }
            var p = new P();
        )");
        Variable add = e.state.GetVariable("add");
        REQUIRE(add.Is<Callback>());
        CHECK(e.state.Call(add, { e.memory.CreateNumber(2), e.memory.CreateNumber(5) }).ToString() == "7");
        CHECK(e.state.Call(e.state.GetVariable("fact"), { e.memory.CreateNumber(10) }).ToString() == "3628800");
        CHECK(e.state.Call(e.state.GetVariable("p"), { e.memory.CreateNumber(41) }).ToString() == "42");
        CHECK_THROWS_AS(e.state.Call(e.memory.CreateNumber(3)), ScriptError);
        CHECK(e.state.Call(e.state.GetVariable("print"), { e.memory.CreateString("native") }).Is<Undefined>());
        CHECK(e.Output() == "native");
        CHECK_FALSE(e.state.GetVariable("unknown"));
    }

    TEST_CASE("native functions can call script functions") {
        Tests::Engine e;
        e.state.DeclareVariable("map", e.memory.CreateCallback([](State& s, std::vector<Variable>& args) {
            Memory& memory = s.GetMemory();
            Variable result = memory.Create(Array{});
            if (args.size() < 2 || !args[0].Is<Array>())
                return result;
            for (const Variable& v : args[0].As<Array>().indexed)
                result.As<Array>().indexed.push_back(s.Call(args[1], { v }));
            return result;
        }));
        e.Run("var r = map([1, 2, 3], function(x) { return x * x; });");
        CHECK(e.Eval("r") == "{[0]:1,[1]:4,[2]:9}");
        e.Run("function twice(x) { return map([x], function(y) { return y * 2; })[0]; }");
        CHECK(e.Eval("twice(21) + 1") == "43");
    }

    TEST_CASE("exceptions of native functions leave the state usable") {
        Tests::Engine e;
        e.state.DeclareVariable("fail", e.memory.CreateCallback([](State&, std::vector<Variable>&) -> Variable {
            throw std::runtime_error("failure");
        }));
        e.Run("function f(x) { return fail(x); }");
        std::size_t depth = e.state.GetDepth();
        CHECK_THROWS_AS(e.state.Call(e.state.GetVariable("f"), { e.memory.CreateNumber(1) }), std::runtime_error);
        CHECK(e.state.GetDepth() == depth);
        CHECK(e.Eval("1 + 1") == "2");
    }

    TEST_CASE("infinite recursion is stopped") {
        Tests::Engine e;
        e.state.SetMaxCallDepth(1000);
        e.Run("function f(n) { return f(n + 1); }");
        CHECK_THROWS_WITH_AS(e.Eval("f(0)"), "maximum call depth exceeded", ScriptError);
        CHECK(e.state.GetDepth() == 1);
    }

    TEST_CASE("deep recursion uses the heap") {
        Tests::Engine e;
        e.Run("function depth(n) { return n == 0 ? 0 : 1 + depth(n - 1); }");
        CHECK(e.Eval("depth(50000)") == "50000");
    }

    TEST_CASE("variables of the application") {
        Tests::Engine e;
        e.state.DeclareVariable("answer", e.memory.CreateNumber(42));
        e.state.DeclareVariable("config", e.memory.CreateArray({ { "name", e.memory.CreateString("test") } }));
        CHECK(e.Eval("answer") == "42");
        CHECK(e.Eval("config.name") == "\"test\"");
        e.Run("answer = 0;");
        CHECK(e.state.GetVariable("answer").As<Number>() == Number(0));
        e.state.UseNamespace("App.Data");
        e.Run("var x = 5;");
        e.state.UseNamespace();
        CHECK(e.Eval("App.Data.x") == "5");
    }

    TEST_CASE("execution of single instructions") {
        Tests::Engine e;
        e.state.ExecuteInstruction(Instruction(InstrCode::ValueNumber, 5));
        Variable top = e.state.ExecuteInstruction(Instruction(InstrCode::OpUnaryMinus));
        CHECK(top.ToString() == "-5");
        CHECK(e.state.GetStack().size() == 1);
        e.state.ExecuteInstruction(Instruction(InstrCode::DefineVariable, "v"));
        CHECK(e.state.GetVariable("v").ToString() == "-5");
        CHECK(e.state.GetStack().empty());
    }

    TEST_CASE("invalid instructions don't break the state") {
        Tests::Engine e;
        // Instructions on an empty stack, invalid operands, invalid jumps and calls
        std::vector<Instruction> instructions;
        for (std::size_t code = 0; code < INSTRUCTION_COUNT; code++)
            instructions.emplace_back(static_cast<InstrCode>(code));
        for (std::size_t code = 0; code < INSTRUCTION_COUNT; code++)
            instructions.emplace_back(static_cast<InstrCode>(code), std::int64_t{ -1 });
        for (std::size_t code = 0; code < INSTRUCTION_COUNT; code++) {
            if (static_cast<InstrCode>(code) != InstrCode::JumpTo)
                instructions.emplace_back(static_cast<InstrCode>(code), std::int64_t{ 1000000 });
        }
        instructions.emplace_back(InstrCode::ValueCallback, std::int64_t{ 99999 });
        instructions.emplace_back(InstrCode::PushArgs);
        instructions.emplace_back(InstrCode::OpCall);
        instructions.emplace_back(InstrCode::Return);
        e.state.AddInstructions(instructions);
        e.state.Execute();
        CHECK(e.state.Finished());
        CHECK(e.Eval("1") == "1");
    }

    TEST_CASE("instructions added during the execution") {
        Tests::Engine e;
        // A native function that adds instructions to the list being executed
        e.state.DeclareVariable("grow", e.memory.RegisterFunction("grow", [](State& s, std::vector<Variable>&) {
            for (int i = 0; i < 1000; i++)
                s.AddInstruction(Instruction(InstrCode::ValueNumber, i));
            s.AddInstruction(Instruction(InstrCode::DefineVariable, "added"));
            return s.GetMemory().CreateUndefined();
        }));
        REQUIRE(Assembly::ExecuteAfter(e.state, "value.variable grow\npush.args\noperator.call\nvalue.pop"));
        CHECK(e.Eval("added") == "999");
        // A state loaded during the execution of an instruction
        std::ostringstream saved(std::ios::binary);
        e.state.Save(saved);
        std::string data = saved.str();
        e.state.DeclareVariable("reload", e.memory.RegisterFunction("reload", [data](State& s, std::vector<Variable>&) {
            std::istringstream stream(data, std::ios::binary);
            s.Load(stream);
            return s.GetMemory().CreateUndefined();
        }));
        e.Run("reload();");
        CHECK(e.Eval("added") == "999");
    }

    TEST_CASE("a function that reaches the end of its list returns") {
        Tests::Engine e;
        // The function starts at the last line, it returns undefined at the end of the list
        auto result = Assembly::Compile("value.callback 4\npush.args\noperator.call\nvalue.number 7");
        REQUIRE(result);
        e.state.AddInstructions(result.instructions);
        CHECK(e.state.Execute().ToString() == "7");
        CHECK(e.state.GetDepth() == 1);
        REQUIRE(e.state.GetStack().size() == 2);
        CHECK(e.state.GetStack()[0].Is<Undefined>());
    }

}
