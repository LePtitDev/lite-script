#include "helpers.hpp"
#include "modules.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shellapi.h>
// Conflicts with Script::Result::GetMessage
#undef GetMessage
#endif

#ifndef PLAYGROUND_VERSION
#define PLAYGROUND_VERSION "unknown"
#endif

namespace {

    using namespace Playground;
    namespace fs = std::filesystem;

    constexpr const char * USAGE = R"(Usage: litescript-pg [options] [files...] [-- arguments...]

Execute LiteScript files with the standard library and the libraries of the
playground (Console, File, Directory, Path, JSON, Http, Process, Time).

The files can be patterns ("scripts/*.ls", "tests/**/*.ls") or directories
(every ".ls" file of the directory). Without file, an interactive session starts.

Options:
  -e, --eval <code>    Execute a code (can be repeated, executed before the files)
  -s, --shared         Execute the files in the same state (the variables of a file
                       are visible by the next ones), else each file is isolated
  -b, --bail           Stop at the first file that fails
  -i, --interactive    Start an interactive session after the files
  -h, --help           Display this help
  -v, --version        Display the version

The arguments after "--" are given to the scripts in Process.args.
The exit code is 0 if every file has been executed without error.
)";

    // The options of the command line
    struct Options {
        std::vector<std::string> evals;
        std::vector<std::string> patterns;
        std::vector<std::string> args;
        bool shared = false;
        bool bail = false;
        bool interactive = false;
    };

    ////// PATHS //////

    fs::path ToPath(const std::string& utf8) {
        return fs::path(std::u8string(utf8.begin(), utf8.end()));
    }

    std::string FromPath(const fs::path& path) {
        std::u8string text = path.generic_u8string();
        return std::string(text.begin(), text.end());
    }

    bool HasWildcard(std::string_view text) {
        return text.find_first_of("*?") != std::string_view::npos;
    }

    // Compare two characters of file names (case insensitive on Windows)
    bool SameChar(char32_t a, char32_t b) {
#ifdef _WIN32
        auto lower = [](char32_t c) { return c < 128 ? static_cast<char32_t>(std::tolower(static_cast<int>(c))) : c; };
        return lower(a) == lower(b);
#else
        return a == b;
#endif
    }

    // Match a file name with a pattern that contains "*" (any characters) and "?" (one character)
    bool Match(std::u32string_view pattern, std::u32string_view name) {
        std::size_t p = 0, n = 0, star = std::u32string_view::npos, retry = 0;
        while (n < name.size()) {
            if (p < pattern.size() && pattern[p] == U'*') {
                star = p++;
                retry = n;
            }
            else if (p < pattern.size() && (pattern[p] == U'?' || SameChar(pattern[p], name[n]))) {
                p++;
                n++;
            }
            else if (star != std::u32string_view::npos) {
                p = star + 1;
                n = ++retry;
            }
            else {
                return false;
            }
        }
        while (p < pattern.size() && pattern[p] == U'*')
            p++;
        return p == pattern.size();
    }

    // Find the files that match the components of a pattern from a directory ("**" matches any directories)
    void Expand(const fs::path& base, const std::vector<std::string>& parts, std::size_t index, std::vector<fs::path>& found) {
        std::error_code ec;
        if (index == parts.size()) {
            if (fs::is_regular_file(base, ec))
                found.push_back(base);
            return;
        }
        const std::string& part = parts[index];
        if (part == "**") {
            // Zero directory, then each sub-directory
            Expand(base, parts, index + 1, found);
            for (const auto& entry : fs::recursive_directory_iterator(base.empty() ? fs::path(".") : base, ec)) {
                if (entry.is_directory(ec))
                    Expand(base.empty() ? entry.path().lexically_relative(".") : entry.path(), parts, index + 1, found);
            }
            return;
        }
        if (!HasWildcard(part)) {
            Expand(base / ToPath(part), parts, index + 1, found);
            return;
        }
        std::u32string pattern = LiteScript::String::ConvertToUnicode(part);
        for (const auto& entry : fs::directory_iterator(base.empty() ? fs::path(".") : base, ec)) {
            std::u8string name = entry.path().filename().u8string();
            if (Match(pattern, LiteScript::String::ConvertToUnicode(std::string(name.begin(), name.end()))))
                Expand(base / entry.path().filename(), parts, index + 1, found);
        }
    }

    // Give the files designated by a pattern, a directory or a file
    std::vector<fs::path> FindFiles(const std::string& pattern) {
        std::vector<fs::path> found;
        fs::path path = ToPath(pattern);
        std::error_code ec;
        if (!HasWildcard(pattern)) {
            if (fs::is_directory(path, ec)) {
                for (const auto& entry : fs::directory_iterator(path, ec)) {
                    if (entry.is_regular_file(ec) && entry.path().extension() == ".ls")
                        found.push_back(entry.path());
                }
            }
            else {
                found.push_back(path);
            }
        }
        else {
            // The components before the first wildcard form the base directory
            fs::path base = path.root_path();
            std::vector<std::string> parts;
            for (const auto& component : path.relative_path()) {
                std::u8string text = component.u8string();
                parts.emplace_back(text.begin(), text.end());
            }
            std::size_t first = 0;
            while (first < parts.size() && !HasWildcard(parts[first]) && parts[first] != "**")
                base /= ToPath(parts[first++]);
            parts.erase(parts.begin(), parts.begin() + static_cast<std::ptrdiff_t>(first));
            Expand(base, parts, 0, found);
        }
        std::sort(found.begin(), found.end());
        found.erase(std::unique(found.begin(), found.end()), found.end());
        return found;
    }

    std::optional<std::string> ReadFile(const fs::path& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
            return std::nullopt;
        std::ostringstream content;
        content << file.rdbuf();
        std::string text = content.str();
        // UTF-8 byte order mark
        if (text.starts_with("\xEF\xBB\xBF"))
            text.erase(0, 3);
        // A first line starting with "#!" (shebang) is ignored
        if (text.starts_with("#!"))
            text.insert(0, "//");
        return text;
    }

    ////// EXECUTION //////

    // A memory and a state with every library
    struct Environment {
        Memory memory;
        State state{ memory };

        explicit Environment(const Context& context) {
            Standard::Install(this->state);
            Playground::Install(this->state, context);
        }
    };

    // Execute a code and display its errors, return false if there is an error
    bool Run(State& state, const std::string& code, const std::string& name) {
        try {
            Script::Result result = Script::Execute(state, code);
            if (!result) {
                std::cout.flush();
                std::cerr << name << ":" << result.line + 1 << ":" << result.column + 1 << ": syntax error: " << result.GetMessage() << std::endl;
                return false;
            }
            return true;
        }
        catch (const ScriptError& error) {
            std::cout.flush();
            SourcePosition position = error.GetPosition();
            std::cerr << name << ":" << position.line + 1 << ":" << position.column + 1 << ": error: " << error.what() << std::endl;
        }
        catch (const std::exception& error) {
            std::cout.flush();
            std::cerr << name << ": error: " << error.what() << std::endl;
        }
        return false;
    }

    // Read blocks of lines and execute them (an empty line executes the block)
    void Interactive(State& state) {
        std::cout << "LiteScript playground " << PLAYGROUND_VERSION << std::endl;
        std::cout << "Enter code, an empty line executes it, \"exit\" quits." << std::endl;
        while (true) {
            std::string code, line;
            for (unsigned number = 1; true; number++) {
                std::cout << (number == 1 ? "> " : ". ") << std::flush;
                if (!std::getline(std::cin, line))
                    return;
                if (!line.empty() && line.back() == '\r')
                    line.pop_back();
                if (number == 1 && line == "exit")
                    return;
                if (line.empty())
                    break;
                code += line + '\n';
            }
            if (code.empty())
                continue;
            // A single expression is stored in the variable "_" and its value is displayed
            std::string expression = code;
            while (!expression.empty() && (std::isspace(static_cast<unsigned char>(expression.back())) || expression.back() == ';'))
                expression.pop_back();
            std::string assignment = "var _ = (" + expression + ");";
            bool display = !expression.empty() && Script::Compile(assignment);
            if (!Run(state, display ? assignment : code, "<input>") || !display)
                continue;
            Variable value = state.GetVariable("_");
            if (value && !value.Is<LiteScript::Undefined>())
                std::cout << LiteScript::ToString(value) << std::endl;
        }
    }

    // Read the command line (return nullopt to quit with the given code)
    std::optional<Options> ParseArguments(const std::vector<std::string>& argv, int& exit_code) {
        Options options;
        for (std::size_t i = 1; i < argv.size(); i++) {
            const std::string& arg = argv[i];
            if (arg == "--") {
                options.args.assign(argv.begin() + static_cast<std::ptrdiff_t>(i) + 1, argv.end());
                break;
            }
            if (arg == "-h" || arg == "--help") {
                std::cout << USAGE;
                exit_code = 0;
                return std::nullopt;
            }
            if (arg == "-v" || arg == "--version") {
                std::cout << "litescript-pg " << PLAYGROUND_VERSION << std::endl;
                exit_code = 0;
                return std::nullopt;
            }
            if (arg == "-e" || arg == "--eval") {
                if (++i >= argv.size()) {
                    std::cerr << "litescript-pg: the option " << arg << " needs a code" << std::endl;
                    exit_code = 2;
                    return std::nullopt;
                }
                options.evals.push_back(argv[i]);
            }
            else if (arg == "-s" || arg == "--shared") {
                options.shared = true;
            }
            else if (arg == "-b" || arg == "--bail") {
                options.bail = true;
            }
            else if (arg == "-i" || arg == "--interactive") {
                options.interactive = true;
            }
            else if (arg.size() > 1 && arg[0] == '-') {
                std::cerr << "litescript-pg: unknown option " << arg << " (see --help)" << std::endl;
                exit_code = 2;
                return std::nullopt;
            }
            else {
                options.patterns.push_back(arg);
            }
        }
        return options;
    }

    // Return the arguments of the program encoded in UTF-8
    std::vector<std::string> GetArguments(int argc, char * argv[]) {
#ifdef _WIN32
        // The arguments given to main are encoded with the code page of the system
        int count = 0;
        LPWSTR * wide = CommandLineToArgvW(GetCommandLineW(), &count);
        if (wide != nullptr) {
            std::vector<std::string> args;
            for (int i = 0; i < count; i++) {
                int size = WideCharToMultiByte(CP_UTF8, 0, wide[i], -1, nullptr, 0, nullptr, nullptr);
                std::string arg(static_cast<std::size_t>(std::max(size, 1) - 1), '\0');
                WideCharToMultiByte(CP_UTF8, 0, wide[i], -1, arg.data(), size, nullptr, nullptr);
                args.push_back(std::move(arg));
            }
            LocalFree(wide);
            return args;
        }
#endif
        return std::vector<std::string>(argv, argv + argc);
    }

}

void Playground::Install(State& state, const Context& context) {
    InstallConsole(state);
    InstallFileSystem(state);
    InstallJson(state);
    InstallHttp(state);
    InstallSystem(state, context);
}

int main(int argc, char * argv[]) {
#ifdef _WIN32
    // The scripts write UTF-8 texts
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    std::ios::sync_with_stdio(false);

    int exit_code = 0;
    auto options = ParseArguments(GetArguments(argc, argv), exit_code);
    if (!options)
        return exit_code;

    // The files to execute
    std::vector<fs::path> files;
    for (const std::string& pattern : options->patterns) {
        std::vector<fs::path> found = FindFiles(pattern);
        if (found.empty()) {
            std::cerr << "litescript-pg: no file matches \"" << pattern << "\"" << std::endl;
            return 2;
        }
        files.insert(files.end(), found.begin(), found.end());
    }

    Context context;
    context.args = options->args;
    std::optional<Environment> shared;
    if (options->shared || files.size() + options->evals.size() <= 1)
        shared.emplace(context);

    std::size_t failures = 0;
    auto execute = [&](const std::string& code, const std::string& name, const std::string& script) {
        std::optional<Environment> isolated;
        Environment * env = shared ? &*shared : &isolated.emplace(Context{ context.args, script });
        SetCurrentScript(env->state, script);
        if (!Run(env->state, code, name))
            failures++;
        return failures == 0 || !options->bail;
    };

    bool running = true;
    for (std::size_t i = 0; i < options->evals.size() && running; i++)
        running = execute(options->evals[i], "<eval>", "");
    for (std::size_t i = 0; i < files.size() && running; i++) {
        std::string name = FromPath(files[i]);
        auto code = ReadFile(files[i]);
        if (!code) {
            std::cerr << "litescript-pg: can't open the file \"" << name << "\"" << std::endl;
            failures++;
            running = !options->bail;
            continue;
        }
        running = execute(*code, name, FromPath(fs::absolute(files[i]).lexically_normal()));
    }

    if (options->interactive || (files.empty() && options->evals.empty())) {
        if (!shared)
            shared.emplace(context);
        SetCurrentScript(shared->state, "");
        Interactive(shared->state);
    }

    std::cout.flush();
    if (files.size() > 1 && failures > 0)
        std::cerr << failures << " of " << files.size() + options->evals.size() << " scripts failed" << std::endl;
    return failures == 0 ? 0 : 1;
}
