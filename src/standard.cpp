#include <litescript/standard.hpp>

#include "charconv.hpp"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <limits>
#include <memory>
#include <optional>
#include <random>
#include <stdexcept>

namespace {

    using namespace LiteScript;
    using Args = std::vector<Variable>;

    ////// HELPERS //////

    // Return an argument or undefined
    Variable Arg(State& state, const Args& args, std::size_t index) {
        return index < args.size() ? args[index] : state.GetMemory().CreateUndefined();
    }

    // Return a numeric argument (nullopt if it's not a number)
    std::optional<double> NumberArg(const Args& args, std::size_t index) {
        if (index < args.size() && args[index].Is<Number>())
            return args[index].As<Number>().ToFloat();
        return std::nullopt;
    }

    // Return an integer argument or a default value
    std::int64_t IntegerArg(const Args& args, std::size_t index, std::int64_t default_value) {
        if (index < args.size() && args[index].Is<Number>())
            return args[index].As<Number>().ToInteger();
        return default_value;
    }

    // Return the text of an argument (empty for undefined)
    String TextArg(const Args& args, std::size_t index) {
        if (index >= args.size() || args[index].Is<Undefined>())
            return String();
        return ToText(args[index]);
    }

    // Return the array bound to "this" (the methods of arrays are called on an array)
    Variable ThisArray(State& state) {
        Variable self = state.GetThis();
        if (!self.Is<Array>())
            throw std::invalid_argument("the method must be called on an array");
        return self;
    }

    // Return the string bound to "this"
    String ThisString(State& state) {
        Variable self = state.GetThis();
        if (self.Is<String>() || self.Is<Character>())
            return ToText(self);
        throw std::invalid_argument("the method must be called on a string");
    }

    // Convert an index that can be negative (from the end) into a position in [0, size]
    std::size_t ClampIndex(std::int64_t index, std::size_t size) {
        auto n = static_cast<std::int64_t>(size);
        if (index < 0)
            index += n;
        return static_cast<std::size_t>(std::clamp<std::int64_t>(index, 0, n));
    }

    Variable CreateArray(Memory& memory, std::vector<Variable> elements) {
        Array array;
        array.indexed = std::move(elements);
        return memory.Create(std::move(array));
    }

    // Add a function to a namespace (or the global namespace) and register it for the saving
    void Define(State& state, const Variable& nsp, std::string_view prefix, std::string_view name, NativeFunction::Signature function) {
        Variable callback = state.GetMemory().RegisterFunction("std." + std::string(prefix) + std::string(name), std::move(function));
        nsp.As<Namespace>().variables.Set(name, std::move(callback));
    }

    // Create a function of Math that takes a number and returns a number
    NativeFunction::Signature UnaryMath(double (*f)(double)) {
        return [f](State& state, Args& args) {
            auto x = NumberArg(args, 0);
            if (!x)
                return state.GetMemory().CreateNull();
            return state.GetMemory().CreateNumber(Number(f(*x)));
        };
    }

    // Sort with a merge sort (a comparison that isn't consistent can't break the algorithm)
    template <typename Less>
    void MergeSort(std::vector<Variable>& values, Less&& less) {
        if (values.size() < 2)
            return;
        std::vector<Variable> buffer(values.size());
        for (std::size_t width = 1; width < values.size(); width *= 2) {
            for (std::size_t left = 0; left < values.size(); left += 2 * width) {
                std::size_t middle = std::min(left + width, values.size());
                std::size_t right = std::min(left + 2 * width, values.size());
                std::size_t i = left, j = middle, k = left;
                while (i < middle && j < right)
                    buffer[k++] = less(values[j], values[i]) ? values[j++] : values[i++];
                while (i < middle)
                    buffer[k++] = values[i++];
                while (j < right)
                    buffer[k++] = values[j++];
            }
            values.swap(buffer);
        }
    }

    // Change the case of the ASCII and Latin-1 letters
    char32_t ToUpper(char32_t c) {
        if ((c >= U'a' && c <= U'z') || (c >= 0xE0 && c <= 0xFE && c != 0xF7))
            return c - 0x20;
        return c;
    }

    char32_t ToLower(char32_t c) {
        if ((c >= U'A' && c <= U'Z') || (c >= 0xC0 && c <= 0xDE && c != 0xD7))
            return c + 0x20;
        return c;
    }

    bool IsSpace(char32_t c) {
        return c == U' ' || c == U'\t' || c == U'\n' || c == U'\r' || c == U'\f' || c == U'\v' || c == 0xA0;
    }

    // Parse a number that covers the whole text (spaces around are ignored)
    std::optional<Number> ParseNumber(std::string_view text, int base, bool integer_only) {
        while (!text.empty() && IsSpace(static_cast<unsigned char>(text.front())))
            text.remove_prefix(1);
        while (!text.empty() && IsSpace(static_cast<unsigned char>(text.back())))
            text.remove_suffix(1);
        if (!text.empty() && text.front() == '+')
            text.remove_prefix(1);
        if (text.empty())
            return std::nullopt;
        const char * first = text.data();
        const char * last = first + text.size();
        std::int64_t integer;
        auto ir = std::from_chars(first, last, integer, base);
        if (ir.ec == std::errc() && ir.ptr == last)
            return Number(integer);
        if (base != 10 || integer_only)
            return std::nullopt;
        double value;
        if (Internal::ParseDouble(text, value))
            return Number(value);
        return std::nullopt;
    }

}

void LiteScript::Standard::Install(State& state) {
    InstallMath(state);
    InstallConversions(state);
    InstallArrays(state);
    InstallStrings(state);
}

void LiteScript::Standard::InstallMath(State& state) {
    Memory& memory = state.GetMemory();
    Variable math = memory.CreateNamespace();
    Define(state, math, "Math.", "abs", UnaryMath([](double x) { return std::fabs(x); }));
    Define(state, math, "Math.", "floor", UnaryMath([](double x) { return std::floor(x); }));
    Define(state, math, "Math.", "ceil", UnaryMath([](double x) { return std::ceil(x); }));
    Define(state, math, "Math.", "round", UnaryMath([](double x) { return std::round(x); }));
    Define(state, math, "Math.", "trunc", UnaryMath([](double x) { return std::trunc(x); }));
    Define(state, math, "Math.", "sign", UnaryMath([](double x) { return x > 0 ? 1.0 : x < 0 ? -1.0 : x; }));
    Define(state, math, "Math.", "sqrt", UnaryMath([](double x) { return std::sqrt(x); }));
    Define(state, math, "Math.", "exp", UnaryMath([](double x) { return std::exp(x); }));
    Define(state, math, "Math.", "log", UnaryMath([](double x) { return std::log(x); }));
    Define(state, math, "Math.", "sin", UnaryMath([](double x) { return std::sin(x); }));
    Define(state, math, "Math.", "cos", UnaryMath([](double x) { return std::cos(x); }));
    Define(state, math, "Math.", "tan", UnaryMath([](double x) { return std::tan(x); }));
    Define(state, math, "Math.", "asin", UnaryMath([](double x) { return std::asin(x); }));
    Define(state, math, "Math.", "acos", UnaryMath([](double x) { return std::acos(x); }));
    Define(state, math, "Math.", "atan", UnaryMath([](double x) { return std::atan(x); }));
    Define(state, math, "Math.", "atan2", [](State& s, Args& args) {
        auto y = NumberArg(args, 0), x = NumberArg(args, 1);
        return y && x ? s.GetMemory().CreateNumber(Number(std::atan2(*y, *x))) : s.GetMemory().CreateNull();
    });
    Define(state, math, "Math.", "pow", [](State& s, Args& args) {
        auto x = NumberArg(args, 0), y = NumberArg(args, 1);
        return x && y ? s.GetMemory().CreateNumber(Number(std::pow(*x, *y))) : s.GetMemory().CreateNull();
    });
    auto extremum = [](bool maximum) {
        return [maximum](State& s, Args& args) {
            std::optional<Number> best;
            for (const Variable& v : args) {
                if (!v.Is<Number>())
                    return s.GetMemory().CreateNull();
                const Number& n = v.As<Number>();
                if (!best || (maximum ? n > *best : n < *best))
                    best = n;
            }
            return best ? s.GetMemory().CreateNumber(*best) : s.GetMemory().CreateNull();
        };
    };
    Define(state, math, "Math.", "min", extremum(false));
    Define(state, math, "Math.", "max", extremum(true));
    // random() gives a number in [0, 1[, random(n) an integer in [0, n[, random(a, b) an integer in [a, b]
    auto generator = std::make_shared<std::mt19937_64>(std::random_device{}());
    Define(state, math, "Math.", "random", [generator](State& s, Args& args) {
        auto a = NumberArg(args, 0), b = NumberArg(args, 1);
        if (!a)
            return s.GetMemory().CreateNumber(Number(std::uniform_real_distribution<double>(0, 1)(*generator)));
        std::int64_t low = b ? Number(*a).ToInteger() : 0;
        std::int64_t high = b ? Number(*b).ToInteger() : Number(*a).ToInteger() - 1;
        if (high < low)
            return s.GetMemory().CreateNull();
        return s.GetMemory().CreateNumber(Number(std::uniform_int_distribution<std::int64_t>(low, high)(*generator)));
    });
    Define(state, math, "Math.", "seed", [generator](State& s, Args& args) {
        generator->seed(static_cast<std::uint64_t>(IntegerArg(args, 0, 0)));
        return s.GetMemory().CreateUndefined();
    });
    math.As<Namespace>().variables.Set("PI", memory.CreateNumber(Number(3.14159265358979323846)));
    math.As<Namespace>().variables.Set("E", memory.CreateNumber(Number(2.71828182845904523536)));
    state.DeclareVariable("Math", math);
}

void LiteScript::Standard::InstallConversions(State& state) {
    const Variable& global = state.GetGlobalNamespace();
    Define(state, global, "", "typeOf", [](State& s, Args& args) {
        return s.GetMemory().CreateString(GetTypeName(Arg(s, args, 0).GetType()));
    });
    Define(state, global, "", "toString", [](State& s, Args& args) {
        return s.GetMemory().Create(ToText(Arg(s, args, 0)));
    });
    Define(state, global, "", "parseNumber", [](State& s, Args& args) {
        Variable v = Arg(s, args, 0);
        std::optional<Number> n;
        if (v.Is<Number>())
            n = v.As<Number>();
        else if (v.Is<bool>())
            n = Number(v.As<bool>() ? 1 : 0);
        else if (v.Is<String>() || v.Is<Character>())
            n = ParseNumber(ToText(v).ToUTF8(), 10, false);
        return n ? s.GetMemory().CreateNumber(*n) : s.GetMemory().CreateNull();
    });
    Define(state, global, "", "parseInt", [](State& s, Args& args) {
        Variable v = Arg(s, args, 0);
        std::int64_t base = IntegerArg(args, 1, 10);
        if (base < 2 || base > 36)
            return s.GetMemory().CreateNull();
        if (v.Is<Number>())
            return s.GetMemory().CreateNumber(Number(v.As<Number>().ToInteger()));
        std::optional<Number> n;
        if (v.Is<String>() || v.Is<Character>())
            n = ParseNumber(ToText(v).ToUTF8(), static_cast<int>(base), true);
        if (!n)
            return s.GetMemory().CreateNull();
        return s.GetMemory().CreateNumber(Number(n->ToInteger()));
    });
    Define(state, global, "", "isNaN", [](State& s, Args& args) {
        Variable v = Arg(s, args, 0);
        return s.GetMemory().CreateBoolean(!v.Is<Number>() || std::isnan(v.As<Number>().ToFloat()));
    });
    Define(state, global, "", "char", [](State& s, Args& args) {
        std::int64_t code = IntegerArg(args, 0, -1);
        if (code < 0 || code > 0x10FFFF || (code >= 0xD800 && code <= 0xDFFF))
            return s.GetMemory().CreateNull();
        return s.GetMemory().Create(String(static_cast<char32_t>(code)));
    });
}

void LiteScript::Standard::InstallArrays(State& state) {
    Memory& memory = state.GetMemory();
    Variable methods = memory.CreateNamespace();
    Define(state, methods, "Array.", "push", [](State& s, Args& args) {
        Variable self = ThisArray(s);
        for (const Variable& v : args)
            self.As<Array>().indexed.push_back(Copy(s.GetMemory(), v));
        return s.GetMemory().CreateNumber(static_cast<std::int64_t>(self.As<Array>().indexed.size()));
    });
    Define(state, methods, "Array.", "pop", [](State& s, Args&) {
        Variable self = ThisArray(s);
        auto& elements = self.As<Array>().indexed;
        if (elements.empty())
            return s.GetMemory().CreateUndefined();
        Variable last = std::move(elements.back());
        elements.pop_back();
        return last;
    });
    Define(state, methods, "Array.", "insert", [](State& s, Args& args) {
        Variable self = ThisArray(s);
        auto& elements = self.As<Array>().indexed;
        std::size_t index = ClampIndex(IntegerArg(args, 0, 0), elements.size());
        Variable value = Copy(s.GetMemory(), Arg(s, args, 1));
        elements.insert(elements.begin() + static_cast<std::ptrdiff_t>(index), std::move(value));
        return s.GetMemory().CreateNumber(static_cast<std::int64_t>(elements.size()));
    });
    Define(state, methods, "Array.", "remove", [](State& s, Args& args) {
        Variable self = ThisArray(s);
        auto& elements = self.As<Array>().indexed;
        std::int64_t index = IntegerArg(args, 0, -1);
        if (index < 0)
            index += static_cast<std::int64_t>(elements.size());
        if (index < 0 || index >= static_cast<std::int64_t>(elements.size()))
            return s.GetMemory().CreateUndefined();
        Variable removed = elements[static_cast<std::size_t>(index)];
        elements.erase(elements.begin() + index);
        return removed;
    });
    Define(state, methods, "Array.", "clear", [](State& s, Args&) {
        ThisArray(s).As<Array>().indexed.clear();
        return s.GetMemory().CreateUndefined();
    });
    Define(state, methods, "Array.", "indexOf", [](State& s, Args& args) {
        Variable self = ThisArray(s);
        Variable value = Arg(s, args, 0);
        const auto& elements = self.As<Array>().indexed;
        for (std::size_t i = 0; i < elements.size(); i++) {
            if (ValuesEqual(elements[i], value))
                return s.GetMemory().CreateNumber(static_cast<std::int64_t>(i));
        }
        return s.GetMemory().CreateNumber(-1);
    });
    Define(state, methods, "Array.", "contains", [](State& s, Args& args) {
        Variable self = ThisArray(s);
        Variable value = Arg(s, args, 0);
        for (const Variable& e : self.As<Array>().indexed) {
            if (ValuesEqual(e, value))
                return s.GetMemory().CreateBoolean(true);
        }
        return s.GetMemory().CreateBoolean(false);
    });
    Define(state, methods, "Array.", "join", [](State& s, Args& args) {
        Variable self = ThisArray(s);
        String separator = args.empty() ? String(",") : TextArg(args, 0);
        String result;
        const auto& elements = self.As<Array>().indexed;
        for (std::size_t i = 0; i < elements.size(); i++) {
            if (i > 0)
                result += separator;
            if (!elements[i].Is<Undefined>() && !elements[i].Is<Null>())
                result += ToText(elements[i]);
        }
        return s.GetMemory().Create(std::move(result));
    });
    Define(state, methods, "Array.", "slice", [](State& s, Args& args) {
        Variable self = ThisArray(s);
        const auto& elements = self.As<Array>().indexed;
        std::size_t start = ClampIndex(IntegerArg(args, 0, 0), elements.size());
        std::size_t end = ClampIndex(IntegerArg(args, 1, static_cast<std::int64_t>(elements.size())), elements.size());
        std::vector<Variable> result;
        for (std::size_t i = start; i < end; i++)
            result.push_back(Copy(s.GetMemory(), elements[i]));
        return CreateArray(s.GetMemory(), std::move(result));
    });
    Define(state, methods, "Array.", "reverse", [](State& s, Args&) {
        Variable self = ThisArray(s);
        auto& elements = self.As<Array>().indexed;
        std::reverse(elements.begin(), elements.end());
        return self;
    });
    Define(state, methods, "Array.", "sort", [](State& s, Args& args) {
        Variable self = ThisArray(s);
        Variable compare = Arg(s, args, 0);
        // The elements are sorted in a copy, the comparison function can modify the array
        std::vector<Variable> elements = self.As<Array>().indexed;
        if (compare.Is<Callback>() || compare.Is<ClassObject>()) {
            MergeSort(elements, [&](const Variable& a, const Variable& b) {
                Variable r = s.Call(compare, { a, b });
                return r.Is<Number>() && r.As<Number>().ToFloat() < 0;
            });
        }
        else {
            MergeSort(elements, [](const Variable& a, const Variable& b) {
                return std::is_lt(CompareValues(a, b));
            });
        }
        if (self.Is<Array>())
            self.As<Array>().indexed = std::move(elements);
        return self;
    });
    Define(state, methods, "Array.", "keys", [](State& s, Args&) {
        Variable self = ThisArray(s);
        std::vector<Variable> keys;
        for (const auto& [name, value] : self.As<Array>().named) {
            if (!value.Is<Undefined>())
                keys.push_back(s.GetMemory().CreateString(name));
        }
        return CreateArray(s.GetMemory(), std::move(keys));
    });
    // map, filter and forEach call a function with the element and its index
    Define(state, methods, "Array.", "map", [](State& s, Args& args) {
        Variable self = ThisArray(s);
        Variable function = Arg(s, args, 0);
        std::vector<Variable> elements = self.As<Array>().indexed, result;
        for (std::size_t i = 0; i < elements.size(); i++)
            result.push_back(Copy(s.GetMemory(), s.Call(function, { elements[i], s.GetMemory().CreateNumber(static_cast<std::int64_t>(i)) })));
        return CreateArray(s.GetMemory(), std::move(result));
    });
    Define(state, methods, "Array.", "filter", [](State& s, Args& args) {
        Variable self = ThisArray(s);
        Variable function = Arg(s, args, 0);
        std::vector<Variable> elements = self.As<Array>().indexed, result;
        for (std::size_t i = 0; i < elements.size(); i++) {
            if (IsTruthy(s.Call(function, { elements[i], s.GetMemory().CreateNumber(static_cast<std::int64_t>(i)) })))
                result.push_back(Copy(s.GetMemory(), elements[i]));
        }
        return CreateArray(s.GetMemory(), std::move(result));
    });
    Define(state, methods, "Array.", "forEach", [](State& s, Args& args) {
        Variable self = ThisArray(s);
        Variable function = Arg(s, args, 0);
        std::vector<Variable> elements = self.As<Array>().indexed;
        for (std::size_t i = 0; i < elements.size(); i++)
            s.Call(function, { elements[i], s.GetMemory().CreateNumber(static_cast<std::int64_t>(i)) });
        return s.GetMemory().CreateUndefined();
    });
    memory.SetMethods(Type::Array, methods);
}

void LiteScript::Standard::InstallStrings(State& state) {
    Memory& memory = state.GetMemory();
    Variable methods = memory.CreateNamespace();
    Define(state, methods, "String.", "substring", [](State& s, Args& args) {
        String self = ThisString(s);
        std::size_t length = self.GetLength();
        std::size_t start = ClampIndex(std::max<std::int64_t>(IntegerArg(args, 0, 0), 0), length);
        std::size_t end = ClampIndex(std::max<std::int64_t>(IntegerArg(args, 1, static_cast<std::int64_t>(length)), 0), length);
        if (start > end)
            std::swap(start, end);
        return s.GetMemory().Create(String(self.GetData().substr(start, end - start)));
    });
    Define(state, methods, "String.", "indexOf", [](State& s, Args& args) {
        String self = ThisString(s);
        std::size_t from = ClampIndex(std::max<std::int64_t>(IntegerArg(args, 1, 0), 0), self.GetLength());
        std::size_t index = self.GetData().find(TextArg(args, 0).GetData(), from);
        return s.GetMemory().CreateNumber(index == std::u32string::npos ? std::int64_t{ -1 } : static_cast<std::int64_t>(index));
    });
    Define(state, methods, "String.", "contains", [](State& s, Args& args) {
        return s.GetMemory().CreateBoolean(ThisString(s).GetData().find(TextArg(args, 0).GetData()) != std::u32string::npos);
    });
    Define(state, methods, "String.", "startsWith", [](State& s, Args& args) {
        return s.GetMemory().CreateBoolean(ThisString(s).GetData().starts_with(TextArg(args, 0).GetData()));
    });
    Define(state, methods, "String.", "endsWith", [](State& s, Args& args) {
        return s.GetMemory().CreateBoolean(ThisString(s).GetData().ends_with(TextArg(args, 0).GetData()));
    });
    Define(state, methods, "String.", "split", [](State& s, Args& args) {
        const std::u32string text = ThisString(s).GetData();
        std::vector<Variable> parts;
        if (args.empty() || args[0].Is<Undefined>()) {
            parts.push_back(s.GetMemory().Create(String(text)));
        }
        else {
            const std::u32string separator = TextArg(args, 0).GetData();
            if (separator.empty()) {
                for (char32_t c : text)
                    parts.push_back(s.GetMemory().Create(String(c)));
            }
            else {
                std::size_t start = 0;
                while (true) {
                    std::size_t found = text.find(separator, start);
                    parts.push_back(s.GetMemory().Create(String(text.substr(start, found - start))));
                    if (found == std::u32string::npos)
                        break;
                    start = found + separator.size();
                }
            }
        }
        return CreateArray(s.GetMemory(), std::move(parts));
    });
    Define(state, methods, "String.", "replace", [](State& s, Args& args) {
        std::u32string text = ThisString(s).GetData();
        const std::u32string search = TextArg(args, 0).GetData();
        const std::u32string replacement = TextArg(args, 1).GetData();
        if (!search.empty()) {
            std::u32string result;
            std::size_t start = 0;
            while (true) {
                std::size_t found = text.find(search, start);
                result += text.substr(start, found - start);
                if (found == std::u32string::npos)
                    break;
                result += replacement;
                start = found + search.size();
            }
            text = std::move(result);
        }
        return s.GetMemory().Create(String(std::move(text)));
    });
    Define(state, methods, "String.", "trim", [](State& s, Args&) {
        std::u32string text = ThisString(s).GetData();
        std::size_t start = 0, end = text.size();
        while (start < end && IsSpace(text[start]))
            start++;
        while (end > start && IsSpace(text[end - 1]))
            end--;
        return s.GetMemory().Create(String(text.substr(start, end - start)));
    });
    Define(state, methods, "String.", "toUpperCase", [](State& s, Args&) {
        std::u32string text = ThisString(s).GetData();
        for (char32_t& c : text)
            c = ToUpper(c);
        return s.GetMemory().Create(String(std::move(text)));
    });
    Define(state, methods, "String.", "toLowerCase", [](State& s, Args&) {
        std::u32string text = ThisString(s).GetData();
        for (char32_t& c : text)
            c = ToLower(c);
        return s.GetMemory().Create(String(std::move(text)));
    });
    Define(state, methods, "String.", "charCode", [](State& s, Args& args) {
        String self = ThisString(s);
        std::int64_t index = IntegerArg(args, 0, 0);
        if (index < 0 || index >= static_cast<std::int64_t>(self.GetLength()))
            return s.GetMemory().CreateUndefined();
        return s.GetMemory().CreateNumber(static_cast<std::int64_t>(self[static_cast<std::size_t>(index)]));
    });
    memory.SetMethods(Type::String, methods);
    memory.SetMethods(Type::Character, methods);
}
