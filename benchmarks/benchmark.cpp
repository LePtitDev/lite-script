#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <litescript/litescript.hpp>

using namespace LiteScript;

namespace {

    struct Benchmark {
        std::string_view name;
        std::string_view code;
        std::string_view expected;
    };

    constexpr Benchmark BENCHMARKS[] = {
        { "recursive calls", R"(
            function fibo(n) { return n < 2 ? n : fibo(n - 1) + fibo(n - 2); }
            return fibo(24);
        )", "46368" },
        { "loops and arithmetic", R"(
            var sum = 0;
            for (var i = 0; i < 500000; i++)
                sum += i % 7 * 2;
            return sum;
        )", "2999988" },
        { "local variables", R"(
            function work(n) {
                var a = 1, b = 2, c = 3, d = 4, total = 0;
                for (var i = 0; i < n; i++) {
                    var t = a + b * c - d;
                    total += t + i;
                }
                return total;
            }
            return work(200000);
        )", "20000500000" },
        { "arrays", R"(
            var a = [];
            for (var i = 0; i < 100000; i++)
                a[i] = i * 2;
            var sum = 0;
            for (var i = 0; i < a.length; i++)
                sum += a[i];
            return sum;
        )", "9999900000" },
        { "objects and methods", R"(
            class Point {
                var x = 0, y = 0;
                function Point(x, y) { this.x = x; this.y = y; }
                function length2() { return this.x * this.x + this.y * this.y; }
                operator add(p) { return new Point(this.x + p.x, this.y + p.y); }
            }
            var total = 0;
            var p = new Point(1, 1);
            for (var i = 0; i < 30000; i++) {
                p = p + new Point(1, 2);
                total += p.length2() % 10;
            }
            return total;
        )", "135000" },
        { "strings", R"(
            var s = "";
            for (var i = 0; i < 20000; i++)
                s += "x" + i % 10;
            var count = 0;
            for (var i = 0; i < s.length; i++)
                if (s[i] == "x") count++;
            return count;
        )", "20000" },
        { "closures and namespaces", R"(
            namespace Bench.Inner;
            function make(k) { return function(x) { return x + k; }; }
            namespace global;
            var total = 0;
            for (var i = 0; i < 50000; i++) {
                var f = Bench.Inner.make(i);
                total += f(1);
            }
            return total;
        )", "1250025000" },
    };

}

int main(int argc, char * argv[]) {
    int repetitions = argc > 1 ? std::max(1, std::atoi(argv[1])) : 5;
    auto compiled_total = std::chrono::duration<double, std::milli>::zero();
    bool failed = false;
    std::cout << std::left << std::setw(26) << "benchmark" << std::right << std::setw(12) << "best (ms)" << std::endl;
    for (const Benchmark& benchmark : BENCHMARKS) {
        auto compiled = Script::Compile(benchmark.code);
        if (!compiled) {
            std::cout << benchmark.name << ": syntax error " << compiled.GetMessage() << std::endl;
            return 1;
        }
        double best = 1e300;
        std::string result;
        for (int i = 0; i < repetitions; i++) {
            Memory memory;
            State state(memory);
            state.AddInstructions(compiled.instructions);
            auto start = std::chrono::steady_clock::now();
            result = state.Execute().ToString();
            std::chrono::duration<double, std::milli> elapsed = std::chrono::steady_clock::now() - start;
            best = std::min(best, elapsed.count());
        }
        compiled_total += std::chrono::duration<double, std::milli>(best);
        std::cout << std::left << std::setw(26) << benchmark.name << std::right << std::setw(12) << std::fixed << std::setprecision(1) << best;
        if (result != benchmark.expected) {
            std::cout << "  WRONG RESULT: " << result << " (expected " << benchmark.expected << ")";
            failed = true;
        }
        std::cout << std::endl;
    }
    std::cout << std::left << std::setw(26) << "total" << std::right << std::setw(12) << compiled_total.count() << std::endl;
    return failed ? 1 : 0;
}
