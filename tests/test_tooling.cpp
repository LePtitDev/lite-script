#include "helpers.hpp"

#include <algorithm>

using namespace LiteScript;

namespace {

    using Analysis = Script::Analysis;

    const Analysis::Symbol * FindSymbol(const Analysis& analysis, std::string_view name) {
        for (const auto& symbol : analysis.symbols) {
            if (symbol.name == name)
                return &symbol;
        }
        return nullptr;
    }

    // Compile an expression for State::Evaluate (without its final "value.pop")
    std::vector<Instruction> Expression(std::string_view code) {
        auto result = Script::Compile("(" + std::string(code) + ");");
        REQUIRE(result);
        result.instructions.pop_back();
        return result.instructions;
    }

}

TEST_SUITE("tooling") {

    TEST_CASE("declarations of the analysis") {
        Analysis analysis;
        auto result = Script::Compile(
            "var a = 1, f = function(x) { return x; };\n"
            "function add(x, y, rest...) {\n"
            "    var sum = x + y;\n"
            "    return sum;\n"
            "}\n"
            "class Point : Base {\n"
            "    var x = 0;\n"
            "    static var count;\n"
            "    function Point(x) { this.x = x; }\n"
            "    operator add(p) { return p; }\n"
            "    class Inner {}\n"
            "}\n"
            "namespace Game.Engine;\n"
            "var version = 2;\n"
            "try { add(1, 2); } catch (e) { }\n", &analysis);
        REQUIRE(result);

        const auto * a = FindSymbol(analysis, "a");
        REQUIRE(a != nullptr);
        CHECK(a->kind == Analysis::SymbolKind::Variable);
        CHECK(a->range.start.line == 0);
        CHECK(a->range.start.column == 4);
        CHECK(a->range.end.column == 5);
        CHECK(a->scope == 0);
        CHECK(analysis.scopes[0].kind == Analysis::Scope::Kind::Script);

        // A variable initialized with a function is a function
        const auto * f = FindSymbol(analysis, "f");
        REQUIRE(f != nullptr);
        CHECK(f->kind == Analysis::SymbolKind::Function);
        CHECK(f->parameters == std::vector<std::string>{ "x" });

        const auto * add = FindSymbol(analysis, "add");
        REQUIRE(add != nullptr);
        CHECK(add->kind == Analysis::SymbolKind::Function);
        CHECK(add->parameters == std::vector<std::string>{ "x", "y", "rest..." });
        CHECK(add->full.start.line == 1);
        CHECK(add->full.end.line == 4);
        REQUIRE(add->body != Analysis::NONE);
        CHECK(analysis.scopes[add->body].kind == Analysis::Scope::Kind::Function);
        CHECK(analysis.scopes[add->body].parent == 0);
        // The instructions of the body: the first one reads the first argument
        REQUIRE(add->first_instruction < add->end_instruction);
        CHECK(result.instructions[add->first_instruction].code == InstrCode::ValueArg);
        CHECK(result.instructions[add->end_instruction - 1].code == InstrCode::Return);

        const auto * sum = FindSymbol(analysis, "sum");
        REQUIRE(sum != nullptr);
        CHECK(sum->scope == add->body);

        const auto * point = FindSymbol(analysis, "Point");
        REQUIRE(point != nullptr);
        CHECK(point->kind == Analysis::SymbolKind::Class);
        REQUIRE(point->body != Analysis::NONE);
        CHECK(analysis.scopes[point->body].kind == Analysis::Scope::Kind::Class);
        int attributes = 0, methods = 0, operators = 0, statics = 0;
        for (const auto& symbol : analysis.symbols) {
            if (symbol.scope != point->body)
                continue;
            attributes += symbol.kind == Analysis::SymbolKind::Attribute;
            methods += symbol.kind == Analysis::SymbolKind::Method;
            operators += symbol.kind == Analysis::SymbolKind::Operator;
            statics += symbol.is_static;
        }
        CHECK(attributes == 2);
        CHECK(methods == 1);
        CHECK(operators == 1);
        // "count" and the nested class
        CHECK(statics == 2);

        const auto * nsp = FindSymbol(analysis, "Game.Engine");
        REQUIRE(nsp != nullptr);
        CHECK(nsp->kind == Analysis::SymbolKind::Namespace);
        const auto * version = FindSymbol(analysis, "version");
        REQUIRE(version != nullptr);
        CHECK(version->nspace == "Game.Engine");
        CHECK(FindSymbol(analysis, "e")->kind == Analysis::SymbolKind::CatchVariable);
    }

    TEST_CASE("references and statements of the analysis") {
        Analysis analysis;
        auto result = Script::Compile("var s = 1;\nprint(\"é\", s, obj.member);\n", &analysis);
        REQUIRE(result);
        std::vector<std::string> names;
        for (const auto& reference : analysis.references)
            names.push_back(reference.name + (reference.member ? "!" : ""));
        CHECK(names == std::vector<std::string>{ "print", "s", "obj", "member!" });
        // The columns are counted in characters, the offsets in bytes
        const auto& member = analysis.references[3];
        CHECK(member.range.start.line == 1);
        CHECK(member.range.start.column == 18);
        CHECK(member.range.start.offset == 30);

        REQUIRE(analysis.statements.size() == 2);
        CHECK(analysis.statements[0].instruction == 0);
        CHECK(analysis.statements[1].position.line == 1);
        CHECK(result.instructions[analysis.statements[1].instruction].code == InstrCode::ValueVariable);
    }

    TEST_CASE("the analysis is partial after an error") {
        Analysis analysis;
        auto result = Script::Compile("function f(a) {\n    var b = a +;\n", &analysis);
        CHECK(!result);
        CHECK(FindSymbol(analysis, "f") != nullptr);
        CHECK(FindSymbol(analysis, "b") != nullptr);
        // The scopes left open end at the error
        for (const auto& scope : analysis.scopes)
            CHECK(scope.range.end.line == 1);
    }

    TEST_CASE("the analysis doesn't change the instructions") {
        std::string code = "class A { var x = 1; function m() { return this.x; } }\n"
                           "function f(n) { for (var i = 0; i < n; i++) { if (i) continue; } return n ? 1 : 2; }\n"
                           "try { f(2); } catch (e) {}";
        Analysis analysis;
        auto with = Script::Compile(code, &analysis);
        auto without = Script::Compile(code);
        REQUIRE(with);
        CHECK(with.instructions == without.instructions);
    }

    TEST_CASE("debug hook and frames") {
        Tests::Engine e;
        std::vector<std::size_t> depths;
        std::vector<std::string> locals;
        e.state.SetDebugHook([&](State& s) {
            auto frames = s.GetFrames();
            depths.push_back(frames.size());
            // Inside f: the local variables are in the last namespace of the namer
            if (frames.size() == 2) {
                const Variable& scope = frames.back().namer.scopes.back();
                for (const auto& [name, value] : scope.As<Namespace>().variables)
                    locals.push_back(name + "=" + value.ToString());
            }
        });
        e.Run("function f(a) { var b = a * 2; return b; }\nvar r = f(4);");
        CHECK(e.Eval("r") == "8");
        CHECK(*std::max_element(depths.begin(), depths.end()) == 2);
        CHECK(std::find(locals.begin(), locals.end(), "b=8") != locals.end());
        e.state.SetDebugHook({});
    }

    TEST_CASE("positions of the frames") {
        Tests::Engine e;
        Analysis analysis;
        auto compiled = Script::Compile("function f() {\n    var x = 1;\n}\nf();\n", &analysis);
        REQUIRE(compiled);
        e.state.AddInstructions(compiled.instructions);
        std::vector<std::pair<std::size_t, std::size_t>> positions;
        e.state.SetDebugHook([&](State& s) {
            auto frames = s.GetFrames();
            if (frames.size() == 2 && positions.empty()) {
                const auto& list = s.GetInstructions(frames[0].list);
                // The global frame is calling f (line 3), the function is at its first instruction (line 1)
                positions.emplace_back(list[frames[0].line].position.line, list[frames[1].line].position.line);
            }
        });
        e.state.Execute();
        REQUIRE(positions.size() == 1);
        CHECK(positions[0].first == 3);
        CHECK(positions[0].second == 1);
    }

    TEST_CASE("evaluation in the context of a function") {
        Tests::Engine e;
        std::vector<std::string> values;
        e.state.SetDebugHook([&](State& s) {
            if (s.GetFrames().size() != 2 || !values.empty() || !s.GetVariable("b"))
                return;
            values.push_back(s.Evaluate(Expression("a + b")).ToString());
            // Functions can be called and the script isn't disturbed
            values.push_back(s.Evaluate(Expression("[1, 2, 3].map(function(v) { return v * a; })[2]")).ToString());
            values.push_back(s.Evaluate(Expression("g(10)")).ToString());
            // The errors of the evaluation aren't caught by the script
            CHECK_THROWS_AS(s.Evaluate(Expression("undefined.x")), ScriptError);
            // A return would leave the function
            auto ret = Script::Compile("return 1;");
            CHECK_THROWS_AS(s.Evaluate(ret.instructions), std::runtime_error);
        });
        Standard::Install(e.state);
        e.Run("function g(n) { return n + 1; }\n"
              "function f(a) { var b = 2; var c = 0; try { c = a + b; } catch (x) { c = -1; } return c; }\n"
              "var r = f(3);");
        e.state.SetDebugHook({});
        CHECK(values == std::vector<std::string>{ "5", "9", "11" });
        CHECK(e.Eval("r") == "5");
    }

    TEST_CASE("error hook before the stacks are cleared") {
        Tests::Engine e;
        std::size_t depth = 0;
        std::string message, local;
        e.state.SetErrorHook([&](State& s, const ScriptError& error) {
            depth = s.GetFrames().size();
            message = error.what();
            local = s.Evaluate(Expression("n")).ToString();
        });
        auto result = Script::Compile("function f(n) { return n.x.y; }\nf(5);");
        REQUIRE(result);
        e.state.AddInstructions(result.instructions);
        CHECK_THROWS_AS(e.state.Execute(), ScriptError);
        CHECK(depth == 2);
        CHECK(local == "5");
        CHECK(message.find("can't read the member") != std::string::npos);
        // The state is usable after the error
        CHECK(e.Eval("1 + 1") == "2");
    }

}
