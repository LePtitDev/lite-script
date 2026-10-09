#include "helpers.hpp"

#include <sstream>

using namespace LiteScript;

namespace {

    // A script that uses every kind of object
    constexpr std::string_view PROGRAM = R"js(
        namespace Shapes;
        class Point {
            static var count = 0;
            var x = 0, y = 0;
            function Point(x, y) { this.x = x; this.y = y; count++; }
            operator add(p) { return new Point(this.x + p.x, this.y + p.y); }
            function text() { return "(" + this.x + "," + this.y + ")"; }
        }
        namespace global;
        var s = "héllo";
        var c = s[1];
        var list = [1, 2.5, true, null, { name: "n" }];
        list.self = list;
        function counter() { var n = 0; return function() { n++; return n; }; }
        var next = counter();
        var total = 0;
        for (var i = 0; i < 10; i++) {
            var p = new Shapes.Point(i, 2 * i) + new Shapes.Point(1, 1);
            total += p.x + p.y + next();
            print(p.text(), total);
        }
        print(s, c, list[4].name, Shapes.Point.count);
    )js";

    std::string Save(const State& state) {
        std::ostringstream stream(std::ios::binary);
        state.Save(stream);
        return stream.str();
    }

    void Load(State& state, const std::string& data) {
        std::istringstream stream(data, std::ios::binary);
        state.Load(stream);
    }

}

TEST_SUITE("serialization") {

    TEST_CASE("a state saved in the middle of the execution continues the same way") {
        // Reference execution
        Tests::Engine reference;
        reference.Run(PROGRAM);

        auto compiled = Script::Compile(PROGRAM);
        REQUIRE(compiled);
        // Save at several positions of the execution and continue in a new memory
        for (std::size_t stop : { 0u, 1u, 50u, 200u, 555u, 1000u }) {
            CAPTURE(stop);
            std::string data;
            std::vector<std::string> first_part;
            {
                Tests::Engine e;
                e.state.AddInstructions(compiled.instructions);
                for (std::size_t i = 0; i < stop && !e.state.Finished(); i++)
                    e.state.ExecuteSingle();
                data = Save(e.state);
                first_part = e.output;
            }
            Tests::Engine e;
            Load(e.state, data);
            e.state.Execute();
            std::vector<std::string> output = first_part;
            output.insert(output.end(), e.output.begin(), e.output.end());
            CHECK(output == reference.output);
        }
    }

    TEST_CASE("saving twice gives the same data") {
        Tests::Engine e;
        e.Run(PROGRAM);
        std::string data = Save(e.state);
        Tests::Engine other;
        Load(other.state, data);
        CHECK(Save(other.state) == data);
        CHECK(other.Eval("total") == e.Eval("total"));
        CHECK(other.Eval("list.self.self[1]") == "2.5");
        CHECK(other.Eval("c") == "\"\xC3\xA9\"");
    }

    TEST_CASE("a state saved by a native function continues after the call") {
        std::string data;
        {
            Tests::Engine e;
            e.state.DeclareVariable("save", e.memory.RegisterFunction("save", [&data](State& s, std::vector<Variable>&) {
                std::ostringstream stream(std::ios::binary);
                s.Save(stream);
                data = stream.str();
                return s.GetMemory().CreateString("saved");
            }));
            e.Run(R"(
                function f(x) {
                    var r = save();
                    print(r, x);
                    return x * 2;
                }
                print(f(21));
            )");
            CHECK(e.Output() == "saved 21\n42");
        }
        Tests::Engine e;
        e.memory.RegisterFunction("save", [](State& s, std::vector<Variable>&) { return s.GetMemory().CreateNull(); });
        Load(e.state, data);
        e.state.Execute();
        // The call of the native function gives undefined after the loading
        CHECK(e.Output() == "undefined 21\n42");
    }

    TEST_CASE("native functions must be registered") {
        Tests::Engine e;
        e.state.DeclareVariable("anonymous", e.memory.CreateCallback([](State& s, std::vector<Variable>&) { return s.GetMemory().CreateNull(); }));
        std::ostringstream stream;
        CHECK_THROWS_AS(e.state.Save(stream), SerializationError);

        Tests::Engine saved;
        saved.state.DeclareVariable("custom", saved.memory.RegisterFunction("custom", [](State& s, std::vector<Variable>&) { return s.GetMemory().CreateNull(); }));
        std::string data = Save(saved.state);
        Memory memory;
        State state(memory);
        CHECK_THROWS_AS(Load(state, data), SerializationError);
    }

    TEST_CASE("invalid streams are rejected without modifying the state") {
        Tests::Engine e;
        e.Run(PROGRAM);
        std::string data = Save(e.state);

        Tests::Engine target;
        target.Run("var kept = 1;");
        CHECK_THROWS_AS(Load(target.state, ""), SerializationError);
        CHECK_THROWS_AS(Load(target.state, "LSST"), SerializationError);
        CHECK_THROWS_AS(Load(target.state, "XXXX" + data.substr(4)), SerializationError);
        // Every truncation is detected
        for (std::size_t size = 0; size < data.size(); size += 7)
            CHECK_THROWS_AS(Load(target.state, data.substr(0, size)), SerializationError);
        CHECK(target.Eval("kept") == "1");
        // Altered bytes never crash the loading
        for (std::size_t i = 8; i < data.size(); i += 3) {
            std::string altered = data;
            altered[i] = static_cast<char>(altered[i] ^ 0x5A);
            Tests::Engine victim;
            try {
                Load(victim.state, altered);
                for (int step = 0; step < 10000 && !victim.state.Finished(); step++)
                    victim.state.ExecuteSingle();
            }
            catch (const SerializationError&) {
            }
        }
        // Two objects that reference each other
        std::string cycle("LSST\x02\x00\x00\x00" "\x00\x00\x00\x00" "\x02\x00\x00\x00"
                          "\x0B\x02\x00\x00\x00" "\x0B\x01\x00\x00\x00", 26);
        CHECK_THROWS_WITH_AS(Load(target.state, cycle), doctest::Contains("cycle"), SerializationError);
        // A huge number of objects doesn't allocate them all
        std::string huge = data.substr(0, 8);
        huge += std::string("\x00\x00\x00\x00", 4);
        huge += std::string("\xFF\xFF\xFF\xFF", 4);
        CHECK_THROWS_AS(Load(target.state, huge), SerializationError);
    }

    TEST_CASE("instructions in binary format") {
        auto compiled = Script::Compile(PROGRAM);
        REQUIRE(compiled);
        std::ostringstream out(std::ios::binary);
        SaveInstructions(out, compiled.instructions);
        std::istringstream in(out.str(), std::ios::binary);
        CHECK(LoadInstructions(in) == compiled.instructions);
        std::istringstream truncated(out.str().substr(0, 100), std::ios::binary);
        CHECK_THROWS_AS(LoadInstructions(truncated), SerializationError);
    }

}
