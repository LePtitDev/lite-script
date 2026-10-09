// std::getenv is used to find the documentation
#define _CRT_SECURE_NO_WARNINGS

#include "helpers.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

using namespace LiteScript;

namespace {

    struct CodeBlock {
        std::string file;
        std::size_t line;
        std::string code;
    };

    // Return the code blocks of a language in the Markdown files of the documentation
    std::vector<CodeBlock> FindCodeBlocks(std::string_view language) {
        std::vector<CodeBlock> blocks;
        const char * root = std::getenv("LITESCRIPT_DOCS");
        if (root == nullptr)
            return blocks;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
            if (entry.path().extension() != ".md")
                continue;
            std::ifstream file(entry.path());
            std::string line, code;
            std::size_t number = 0, start = 0;
            bool inside = false;
            while (std::getline(file, line)) {
                number++;
                if (!line.empty() && line.back() == '\r')
                    line.pop_back();
                if (!inside && line == "```" + std::string(language)) {
                    inside = true;
                    start = number;
                    code.clear();
                }
                else if (inside && line == "```") {
                    inside = false;
                    blocks.push_back({ entry.path().filename().string(), start, code });
                }
                else if (inside) {
                    code += line + '\n';
                }
            }
        }
        return blocks;
    }

}

TEST_SUITE("documentation") {

    TEST_CASE("the scripts of the documentation are valid") {
        auto blocks = FindCodeBlocks("javascript");
        if (std::getenv("LITESCRIPT_DOCS") != nullptr)
            CHECK(blocks.size() > 20);
        for (const CodeBlock& block : blocks) {
            CAPTURE(block.file);
            CAPTURE(block.line);
            auto result = Script::Compile(block.code);
            INFO(result.GetMessage(), " (line ", result.line + 1, ")");
            CHECK(result);
        }
    }

    TEST_CASE("the assembly codes of the documentation are valid") {
        auto blocks = FindCodeBlocks("asm");
        for (const CodeBlock& block : blocks) {
            CAPTURE(block.file);
            CAPTURE(block.line);
            // The cheatsheets annotate the lines with "; text" (except "value.string" whose
            // argument is the rest of the line): the annotations are removed
            std::istringstream lines(block.code);
            std::string code, line;
            while (std::getline(lines, line)) {
                std::size_t note = line.find(" ;");
                if (note != std::string::npos && !line.starts_with("value.string"))
                    line.erase(line.find_last_not_of(' ', note) + 1);
                code += line + '\n';
            }
            Tests::Engine e;
            auto result = Assembly::Execute(e.state, code);
            CHECK(result);
            // The Fibonacci example
            if (block.code.find("Fibonacci") != std::string::npos)
                CHECK(result.value.ToString() == "10946");
        }
    }

    TEST_CASE("examples of the documentation") {
        Tests::Engine e;
        Standard::Install(e.state);
        e.Run(R"(
            var a = [10, 20, 30];
            var b = { nom: "Percy", "age": 17 };
            a[5] = 60;
            b.ville = "NY";
            var x = a[10];
            var nom = b.prenom | "inconnu";

            function compteur() { var n = 0; return function() { n++; return n; }; }
            var c = compteur(); c();

            class Compteur {
                static var total = 0;
                function Compteur() { total++; }
                static function creer() { return new Compteur(); }
            }
            Compteur.creer();

            namespace Math2;
            function add(a, b) { return a + b; }
            namespace global;
            Math2.zero = 0;
            Math2 = 5;

            var notes = [12, 8, 15];
            notes.push(10);
            notes.sort();
            var bonnes = notes.filter(function(n) { return n >= 10; });
        )");
        CHECK(e.Eval("a.length") == "6");
        CHECK(e.Eval("b") == "{['nom']:\"Percy\",['age']:17,['ville']:\"NY\"}");
        CHECK(e.Eval("x") == "undefined");
        CHECK(e.Eval("nom") == "\"inconnu\"");
        CHECK(e.Eval("c()") == "2");
        CHECK(e.Eval("Compteur.total") == "1");
        CHECK(e.Eval("Math2.add(3, 2) + Math2.zero") == "5");
        CHECK(e.Eval("notes") == "{[0]:8,[1]:10,[2]:12,[3]:15}");
        CHECK(e.Eval("bonnes") == "{[0]:10,[1]:12,[2]:15}");

        // Methods of types defined by the application
        Variable methods = e.memory.CreateNamespace();
        methods.As<Namespace>().variables.Set("double", e.memory.RegisterFunction("number.double",
            [](State& s, std::vector<Variable>&) {
                Variable self = s.GetThis();
                return s.GetMemory().CreateNumber(self.As<Number>() * Number(2));
            }));
        e.memory.SetMethods(Type::Number, methods);
        CHECK(e.Eval("(21).double()") == "42");
    }

}
