#ifndef LITESCRIPT_PLAYGROUND_MODULES_HPP
#define LITESCRIPT_PLAYGROUND_MODULES_HPP

#include <optional>
#include <string>
#include <vector>

#include <litescript/litescript.hpp>

// The libraries given to the scripts by the playground (in addition to the standard library)
namespace Playground {

    // Information about the running program
    struct Context {

        // The arguments given to the scripts (after "--")
        std::vector<std::string> args;

        // The path of the script being executed (empty for inline code)
        std::string script;

    };

    /**
     * Install every module
     */
    void Install(LiteScript::State& state, const Context& context);

    /**
     * print(...), assert(condition, message) and the namespace "Console":
     * write, writeLine, error, readLine, readAll
     */
    void InstallConsole(LiteScript::State& state);

    /**
     * The namespaces "File" (read, write, append, exists, remove, copy, move, size, readLines),
     * "Directory" (list, create, remove, exists, current, setCurrent) and "Path" (join, directory,
     * filename, stem, extension, absolute, normalize, exists)
     */
    void InstallFileSystem(LiteScript::State& state);

    /**
     * The namespace "JSON": parse and stringify
     */
    void InstallJson(LiteScript::State& state);

    /**
     * Convert a value into JSON (nullopt for a value that can't be written: undefined, function, class)
     *
     * @param indent The text of an indentation level (empty for a compact output)
     * @throw std::runtime_error if the value contains a cycle
     */
    std::optional<std::string> Stringify(const LiteScript::Variable& value, std::string indent = "");

    /**
     * The namespace "Http": request, get, post, put, delete, available
     */
    void InstallHttp(LiteScript::State& state);

    /**
     * The namespaces "Process" (args, script, platform, env, exit, run) and "Time" (now, clock, sleep)
     */
    void InstallSystem(LiteScript::State& state, const Context& context);

    /**
     * Change the value of Process.script
     */
    void SetCurrentScript(LiteScript::State& state, const std::string& script);

}

#endif //LITESCRIPT_PLAYGROUND_MODULES_HPP
