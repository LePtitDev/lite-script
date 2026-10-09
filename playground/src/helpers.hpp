#ifndef LITESCRIPT_PLAYGROUND_HELPERS_HPP
#define LITESCRIPT_PLAYGROUND_HELPERS_HPP

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <litescript/litescript.hpp>

// Helpers shared by the modules of the playground
namespace Playground {

    using namespace LiteScript;
    using Args = std::vector<Variable>;

    /**
     * Return an argument or undefined
     */
    inline Variable Arg(State& state, const Args& args, std::size_t index) {
        return index < args.size() ? args[index] : state.GetMemory().CreateUndefined();
    }

    /**
     * Indicate if an argument is missing (absent, undefined or null)
     */
    inline bool IsMissing(const Args& args, std::size_t index) {
        return index >= args.size() || args[index].Is<Undefined>() || args[index].Is<Null>();
    }

    /**
     * Return the UTF-8 text of an argument (throw if the argument is missing)
     */
    inline std::string TextArg(const Args& args, std::size_t index, std::string_view name) {
        if (IsMissing(args, index))
            throw std::invalid_argument("the argument \"" + std::string(name) + "\" is missing");
        return ToText(args[index]).ToUTF8();
    }

    /**
     * Return the UTF-8 text of an argument or a default value
     */
    inline std::string TextArg(const Args& args, std::size_t index, std::string_view name, std::string_view default_value) {
        return IsMissing(args, index) ? std::string(default_value) : TextArg(args, index, name);
    }

    /**
     * Return an integer argument or a default value
     */
    inline std::int64_t IntegerArg(const Args& args, std::size_t index, std::int64_t default_value) {
        if (index < args.size() && args[index].Is<Number>())
            return args[index].As<Number>().ToInteger();
        return default_value;
    }

    /**
     * Return a boolean argument or a default value
     */
    inline bool BooleanArg(const Args& args, std::size_t index, bool default_value) {
        return IsMissing(args, index) ? default_value : IsTruthy(args[index]);
    }

    /**
     * Create an array of indexed elements
     */
    inline Variable CreateList(Memory& memory, std::vector<Variable> elements) {
        Array array;
        array.indexed = std::move(elements);
        return memory.Create(std::move(array));
    }

    /**
     * Create an array of strings
     */
    inline Variable CreateList(Memory& memory, const std::vector<std::string>& texts) {
        std::vector<Variable> elements;
        elements.reserve(texts.size());
        for (const std::string& text : texts)
            elements.push_back(memory.CreateString(text));
        return CreateList(memory, std::move(elements));
    }

    /**
     * Add a function to a namespace, registered with the name "pg.<prefix><name>"
     */
    inline void Define(State& state, const Variable& nsp, std::string_view prefix, std::string_view name, NativeFunction::Signature function) {
        Variable callback = state.GetMemory().RegisterFunction("pg." + std::string(prefix) + std::string(name), std::move(function));
        nsp.As<Namespace>().variables.Set(name, std::move(callback));
    }

}

#endif //LITESCRIPT_PLAYGROUND_HELPERS_HPP
