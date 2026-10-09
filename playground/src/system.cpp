// std::getenv is used to read the environment variables
#define _CRT_SECURE_NO_WARNINGS

#include "helpers.hpp"
#include "modules.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <thread>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#else
#include <sys/wait.h>
#endif

namespace {

    using namespace Playground;

    const char * PLATFORM =
#if defined(_WIN32)
        "windows";
#elif defined(__APPLE__)
        "macos";
#elif defined(__linux__)
        "linux";
#else
        "unknown";
#endif

    // Execute a command with the shell, return the exit code and the standard output
    std::pair<int, std::string> RunCommand(const std::string& command) {
        std::cout.flush();
        std::FILE * pipe = popen(command.c_str(), "r");
        if (pipe == nullptr)
            throw std::runtime_error("can't execute the command \"" + command + "\"");
        std::string output;
        char buffer[4096];
        std::size_t read;
        while ((read = std::fread(buffer, 1, sizeof(buffer), pipe)) > 0)
            output.append(buffer, read);
        int status = pclose(pipe);
#ifndef _WIN32
        if (WIFEXITED(status))
            status = WEXITSTATUS(status);
        else
            status = -1;
#endif
        return { status, std::move(output) };
    }

}

void Playground::InstallSystem(State& state, const Context& context) {
    Memory& memory = state.GetMemory();

    ////// PROCESS //////

    Variable process = memory.CreateNamespace();
    process.As<Namespace>().variables.Set("args", CreateList(memory, context.args));
    process.As<Namespace>().variables.Set("script", memory.CreateString(context.script));
    process.As<Namespace>().variables.Set("platform", memory.CreateString(PLATFORM));
    // env(name) gives null if the variable isn't defined
    Define(state, process, "Process.", "env", [](State& s, Args& args) {
        const char * value = std::getenv(TextArg(args, 0, "name").c_str());
        return value != nullptr ? s.GetMemory().CreateString(value) : s.GetMemory().CreateNull();
    });
    Define(state, process, "Process.", "exit", [](State&, Args& args) -> Variable {
        std::cout.flush();
        std::cerr.flush();
        std::exit(static_cast<int>(IntegerArg(args, 0, 0)));
    });
    // run(command) executes a command with the shell and gives { code, output }
    Define(state, process, "Process.", "run", [](State& s, Args& args) {
        auto [code, output] = RunCommand(TextArg(args, 0, "command"));
        Memory& m = s.GetMemory();
        return m.CreateArray({ { "code", m.CreateNumber(static_cast<std::int64_t>(code)) }, { "output", m.CreateString(output) } });
    });
    state.DeclareVariable("Process", process);

    ////// TIME //////

    Variable time = memory.CreateNamespace();
    // now() gives the number of milliseconds since 1970-01-01 UTC
    Define(state, time, "Time.", "now", [](State& s, Args&) {
        auto now = std::chrono::system_clock::now().time_since_epoch();
        return s.GetMemory().CreateNumber(static_cast<std::int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(now).count()));
    });
    // clock() gives a number of milliseconds (with decimals) to measure durations
    Define(state, time, "Time.", "clock", [](State& s, Args&) {
        auto now = std::chrono::steady_clock::now().time_since_epoch();
        return s.GetMemory().CreateNumber(Number(std::chrono::duration<double, std::milli>(now).count()));
    });
    Define(state, time, "Time.", "sleep", [](State& s, Args& args) {
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(std::max<std::int64_t>(IntegerArg(args, 0, 0), 0)));
        return s.GetMemory().CreateUndefined();
    });
    // iso(ms = now) gives the UTC date in the ISO 8601 format ("2024-05-17T08:30:00.000Z")
    Define(state, time, "Time.", "iso", [](State& s, Args& args) {
        std::int64_t ms = IntegerArg(args, 0, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
        std::int64_t seconds = ms >= 0 ? ms / 1000 : (ms - 999) / 1000;
        auto t = static_cast<std::time_t>(seconds);
        std::tm date{};
#ifdef _WIN32
        if (gmtime_s(&date, &t) != 0)
            return s.GetMemory().CreateNull();
#else
        if (gmtime_r(&t, &date) == nullptr)
            return s.GetMemory().CreateNull();
#endif
        char buffer[64];
        std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", date.tm_year + 1900, date.tm_mon + 1,
            date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec, static_cast<int>(ms - seconds * 1000));
        return s.GetMemory().CreateString(buffer);
    });
    state.DeclareVariable("Time", time);
}

void Playground::SetCurrentScript(State& state, const std::string& script) {
    Variable process = state.GetGlobalNamespace().As<Namespace>().variables.Find("Process");
    if (process.Is<Namespace>())
        process.As<Namespace>().variables.Set("script", state.GetMemory().CreateString(script));
}
