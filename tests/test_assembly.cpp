#include "helpers.hpp"

using namespace LiteScript;

namespace {

    // The fibonacci example of the repository
    constexpr std::string_view FIBONACCI =
        "namespace Program\n"
        "jump-to 31\n"
        "value.arg 0\n"
        "define.variable n\n"
        "value.variable n\n"
        "value.number 1\n"
        "operator.less-equ\n"
        "jump-else 12\n"
        "value.number 1\n"
        "define.return\n"
        "return\n"
        "value.variable Fibonacci\n"
        "push.args\n"
        "value.variable n\n"
        "value.number 1\n"
        "operator.sub\n"
        "define.arg 0\n"
        "operator.call\n"
        "pop.args\n"
        "value.variable Fibonacci\n"
        "push.args\n"
        "value.variable n\n"
        "value.number 2\n"
        "operator.sub\n"
        "define.arg 0\n"
        "operator.call\n"
        "pop.args\n"
        "operator.add\n"
        "define.return\n"
        "return\n"
        "value.callback 3\n"
        "define.variable Fibonacci\n"
        "namespace global\n"
        "value.variable Program\n"
        "operator.member Fibonacci\n"
        "push.args\n"
        "value.number 20\n"
        "define.arg 0\n"
        "operator.call\n"
        "pop.args\n";

}

TEST_SUITE("assembly") {

    TEST_CASE("fibonacci example") {
        Tests::Engine e;
        auto result = Assembly::Execute(e.state, FIBONACCI);
        REQUIRE(result);
        CHECK(result.instructions.size() == 40);
        CHECK(result.value.ToString() == "10946");
    }

    TEST_CASE("every command") {
        auto result = Assembly::Compile(
            "define.variable a\n"
            "define.arg 2\n"
            "value.boolean true\n"
            "value.boolean false\n"
            "value.number -2.5\n"
            "value.number +12\n"
            "value.string  two  spaces\n"
            "value.callback 1\n"
            "jump-to 3\n"
            "class.push-operator 7\n"
            "namespace a.b\n"
            "\n"
            "  operator.add  \r\n");
        REQUIRE(result);
        REQUIRE(result.instructions.size() == 13);
        const auto& i = result.instructions;
        CHECK(i[0] == Instruction(InstrCode::DefineVariable, "a"));
        CHECK(i[1] == Instruction(InstrCode::DefineArg, 2));
        CHECK(i[2] == Instruction(InstrCode::ValueBoolean, true));
        CHECK(i[3] == Instruction(InstrCode::ValueBoolean, false));
        CHECK(i[4] == Instruction(InstrCode::ValueNumber, -2.5));
        CHECK(i[5] == Instruction(InstrCode::ValueNumber, 12));
        CHECK(i[6] == Instruction(InstrCode::ValueString, " two  spaces"));
        CHECK(i[7] == Instruction(InstrCode::ValueCallback, 0));
        CHECK(i[8] == Instruction(InstrCode::JumpTo, 2));
        CHECK(i[9] == Instruction(InstrCode::ClassPushOperator, 7));
        CHECK(i[10] == Instruction(InstrCode::NamespaceUse, "a.b"));
        CHECK(i[11] == Instruction(InstrCode::Invalid));
        CHECK(i[12] == Instruction(InstrCode::OpAdd));
    }

    TEST_CASE("errors") {
        auto check = [](std::string_view code, Assembly::Error error, std::size_t line) {
            auto result = Assembly::Compile(code);
            CHECK(result.error == error);
            CHECK(result.line == line);
            CHECK(result.instructions.empty());
        };
        check("value.pop\nvalue.unknown", Assembly::Error::UnknownCommand, 1);
        check("operator.addx", Assembly::Error::UnknownCommand, 0);
        check("value.boolean yes", Assembly::Error::ExpectedBoolean, 0);
        check("define.arg -1", Assembly::Error::ExpectedInteger, 0);
        check("define.arg 1x", Assembly::Error::ExpectedInteger, 0);
        check("jump-to 0", Assembly::Error::ExpectedInteger, 0);
        check("jump-if", Assembly::Error::ExpectedInteger, 0);
        check("value.number abc", Assembly::Error::ExpectedNumber, 0);
        check("value.number inf", Assembly::Error::ExpectedNumber, 0);
        check("value.variable", Assembly::Error::ExpectedString, 0);
        check("define.variable   ", Assembly::Error::ExpectedString, 0);
        CHECK(Assembly::GetErrorMessage(Assembly::Error::ExpectedNumber) == "Number argument expected");
    }

    TEST_CASE("disassembly gives the same instructions") {
        auto compiled = Script::Compile(R"(
            class A : B { static var s = "a\nb\\c"; var x = 1.5; operator add(o) { return this; } }
            function f(a, b...) { for (var i = 0; i < 3; i++) { switch (i) { case 1: break; } } return a ? b : null; }
            namespace X.Y;
            var t = [true, false, { k: -1 }];
        )");
        REQUIRE(compiled);
        std::string text = Assembly::Disassemble(compiled.instructions);
        auto reassembled = Assembly::Compile(text);
        REQUIRE(reassembled);
        CHECK(reassembled.instructions == compiled.instructions);
    }

    TEST_CASE("execution after the previous instructions") {
        Tests::Engine e;
        REQUIRE(Assembly::ExecuteAfter(e.state, "value.number 1\ndefine.variable a"));
        // Line numbers continue in the same list
        auto result = Assembly::ExecuteAfter(e.state, "jump-to 5\nvalue.number 2\nvalue.variable a");
        REQUIRE(result);
        CHECK(result.value.ToString() == "1");
        CHECK(e.state.GetInstructionListCount() == 1);
        CHECK(e.state.GetInstructions(0).size() == 5);
    }

    TEST_CASE("manual scopes") {
        Tests::Engine e;
        auto result = Assembly::Execute(e.state,
            "push.nsp\n"
            "value.number 3\n"
            "define.variable local\n"
            "value.variable local\n"
            "define.return\n"
            "pop.nsp\n"
            "value.variable local\n");
        REQUIRE(result);
        REQUIRE(e.state.GetStack().size() == 2);
        CHECK(e.state.GetStack()[0].ToString() == "3");
        CHECK(e.state.GetStack()[1].ToString() == "undefined");
    }

    TEST_CASE("classes in assembly") {
        Tests::Engine e;
        auto result = Assembly::Execute(e.state,
            "value.class\n"
            "value.number 5\n"
            "class.push-unstatic v\n"
            "value.null\n"
            "class.push-static s\n"
            "define.variable C\n"
            "value.variable C\n"
            "push.args\n"
            "value.object\n"
            "operator.member v\n");
        REQUIRE(result);
        CHECK(result.value.ToString() == "5");
        CHECK(e.Eval("C.s") == "null");
    }

}
