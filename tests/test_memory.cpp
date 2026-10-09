#include "helpers.hpp"

#include <optional>

using namespace LiteScript;

TEST_SUITE("memory") {

    TEST_CASE("objects are destroyed with their last reference") {
        Memory memory;
        {
            Variable a = memory.CreateNumber(1);
            CHECK(memory.Count() == 1);
            Variable b = a;
            CHECK(a->GetReferenceCount() == 2);
            a.Reset();
            CHECK(memory.Count() == 1);
            CHECK(b.As<Number>() == Number(1));
        }
        CHECK(memory.Count() == 0);
    }

    TEST_CASE("variables can be moved and reassigned") {
        Memory memory;
        Variable a = memory.CreateString("a");
        Variable b = std::move(a);
        CHECK_FALSE(a);
        CHECK(b.Is<String>());
        b = memory.CreateBoolean(true);
        CHECK(memory.Count() == 1);
        Variable& alias = b;
        b = alias;
        CHECK(b.Is<bool>());
        b = Variable();
        CHECK(memory.Count() == 0);
    }

    TEST_CASE("content of destroyed objects is released") {
        Memory memory;
        Variable element = memory.CreateNumber(3);
        {
            Variable array = memory.CreateArray({ element });
            CHECK(memory.Count() == 3);
        }
        CHECK(memory.Count() == 1);
    }

    TEST_CASE("long chains are destroyed without recursion") {
        Memory memory;
        {
            Variable head = memory.Create(Array{});
            Variable current = head;
            for (int i = 0; i < 200000; i++) {
                Variable next = memory.Create(Array{});
                current.As<Array>().indexed.push_back(next);
                current = next;
            }
            CHECK(memory.Count() == 200001);
        }
        CHECK(memory.Count() == 0);
    }

    TEST_CASE("cycles are destroyed by the garbage collector") {
        Memory memory(0);
        {
            Variable a = memory.Create(Array{});
            Variable b = memory.Create(Array{});
            a.As<Array>().indexed.push_back(b);
            b.As<Array>().indexed.push_back(a);
            Variable self = memory.Create(Array{});
            self.As<Array>().named.Set("me", self);
        }
        CHECK(memory.Count() == 3);
        CHECK(memory.CollectGarbage() == 3);
        CHECK(memory.Count() == 0);
    }

    TEST_CASE("the garbage collector keeps the objects referenced from outside") {
        Memory memory(0);
        Variable kept = memory.Create(Array{});
        Variable inner = memory.CreateString("inner");
        kept.As<Array>().indexed.push_back(memory.CreateNumber(1));
        kept.As<Array>().named.Set("self", kept);
        kept.As<Array>().named.Set("inner", inner);
        {
            Variable lost = memory.Create(Array{});
            lost.As<Array>().named.Set("self", lost);
            lost.As<Array>().named.Set("kept", kept);
        }
        CHECK(memory.Count() == 4);
        CHECK(memory.CollectGarbage() == 1);
        CHECK(memory.Count() == 3);
        CHECK(kept.As<Array>().indexed[0].As<Number>() == Number(1));
        CHECK(inner.As<String>().ToUTF8() == "inner");
        // An object only referenced by a collected cycle is destroyed with it
        {
            Variable a = memory.Create(Array{});
            a.As<Array>().named.Set("self", a);
            a.As<Array>().named.Set("leaf", memory.CreateNumber(5));
        }
        CHECK(memory.CollectGarbage() == 2);
    }

    TEST_CASE("cycles through every kind of object are collected") {
        Memory memory(0);
        {
            Variable nsp = memory.CreateNamespace();
            Callback callback;
            callback.target = Callback::ScriptFunction{ 0, 0, Namer(nsp) };
            Variable function = memory.Create(std::move(callback));
            nsp.As<Namespace>().variables.Set("f", function);

            Variable cls = memory.Create(Class{});
            cls.As<Class>().statics.Set("self", cls);
            cls.As<Class>().operators[0] = function;
            Variable instance = memory.Create(ClassObject{ cls, {} });
            cls.As<Class>().members.Set("instance", instance);

            Variable str = memory.CreateString("abc");
            Variable character = memory.Create(Character{ str, 1 });
            nsp.As<Namespace>().variables.Set("c", character);
            nsp.As<Namespace>().variables.Set("cls", cls);
        }
        CHECK(memory.Count() == 6);
        CHECK(memory.CollectGarbage() == 6);
        CHECK(memory.Count() == 0);
    }

    TEST_CASE("automatic garbage collection") {
        Memory memory(16);
        State state(memory);
        Script::Execute(state, R"(
            function make() {
                var a = [];
                a[0] = a;
                return 0;
            }
            for (var i = 0; i < 1000; i++)
                make();
        )");
        // Without the collector, each call would leave a cycle in the memory
        CHECK(memory.Count() < 200);
    }

    TEST_CASE("variables can outlive the memory") {
        std::optional<Variable> survivor;
        {
            Memory memory;
            Variable array = memory.CreateArray({ memory.CreateNumber(4) });
            array.As<Array>().named.Set("self", array);
            survivor = array;
        }
        // The content has been released but the object is still valid
        CHECK(survivor->Is<Undefined>());
        survivor.reset();
    }

    TEST_CASE("no memory leak") {
        CHECK(Tests::LeakedAllocations([] {
            Memory memory;
            Variable a = memory.CreateArray({ memory.CreateString("x"), memory.CreateNumber(2) });
            a.As<Array>().named.Set("self", a);
            Variable b = memory.CreateCallback([](State& s, std::vector<Variable>&) { return s.GetMemory().CreateNull(); });
            Variable c = memory.RegisterFunction("f", [](State& s, std::vector<Variable>&) { return s.GetMemory().CreateNull(); });
        }) == 0);
        CHECK(Tests::LeakedAllocations([] {
            std::optional<Variable> survivor;
            {
                Memory memory;
                survivor = memory.CreateString("survivor");
            }
            survivor.reset();
        }) == 0);
    }

}
