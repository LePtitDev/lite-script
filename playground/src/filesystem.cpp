#include "helpers.hpp"
#include "modules.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace {

    using namespace Playground;
    namespace fs = std::filesystem;

    // Convert an UTF-8 text into a path
    fs::path ToPath(const std::string& utf8) {
        return fs::path(std::u8string(utf8.begin(), utf8.end()));
    }

    // Convert a path into an UTF-8 text (with "/" as separator)
    std::string FromPath(const fs::path& path) {
        std::u8string text = path.generic_u8string();
        return std::string(text.begin(), text.end());
    }

    fs::path PathArg(const Args& args, std::size_t index, std::string_view name) {
        return ToPath(TextArg(args, index, name));
    }

    std::string ReadFile(const fs::path& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("can't open the file \"" + FromPath(path) + "\"");
        std::ostringstream content;
        content << file.rdbuf();
        return content.str();
    }

    void WriteFile(const fs::path& path, std::string_view content, bool append) {
        std::ofstream file(path, std::ios::binary | (append ? std::ios::app : std::ios::trunc));
        if (!file.is_open())
            throw std::runtime_error("can't open the file \"" + FromPath(path) + "\"");
        file.write(content.data(), static_cast<std::streamsize>(content.size()));
        if (!file)
            throw std::runtime_error("can't write the file \"" + FromPath(path) + "\"");
    }

    Variable Nothing(State& state) {
        return state.GetMemory().CreateUndefined();
    }

}

void Playground::InstallFileSystem(State& state) {
    Memory& memory = state.GetMemory();

    ////// FILE //////

    Variable file = memory.CreateNamespace();
    Define(state, file, "File.", "read", [](State& s, Args& args) {
        return s.GetMemory().CreateString(ReadFile(PathArg(args, 0, "path")));
    });
    Define(state, file, "File.", "readLines", [](State& s, Args& args) {
        std::istringstream content(ReadFile(PathArg(args, 0, "path")));
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(content, line)) {
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            lines.push_back(std::move(line));
        }
        return CreateList(s.GetMemory(), lines);
    });
    Define(state, file, "File.", "write", [](State& s, Args& args) {
        WriteFile(PathArg(args, 0, "path"), TextArg(args, 1, "content", ""), false);
        return Nothing(s);
    });
    Define(state, file, "File.", "append", [](State& s, Args& args) {
        WriteFile(PathArg(args, 0, "path"), TextArg(args, 1, "content", ""), true);
        return Nothing(s);
    });
    Define(state, file, "File.", "exists", [](State& s, Args& args) {
        std::error_code ec;
        return s.GetMemory().CreateBoolean(fs::is_regular_file(PathArg(args, 0, "path"), ec));
    });
    Define(state, file, "File.", "size", [](State& s, Args& args) {
        return s.GetMemory().CreateNumber(static_cast<std::int64_t>(fs::file_size(PathArg(args, 0, "path"))));
    });
    // remove(path) gives false if the file doesn't exist
    Define(state, file, "File.", "remove", [](State& s, Args& args) {
        fs::path path = PathArg(args, 0, "path");
        if (fs::is_directory(path))
            throw std::runtime_error("\"" + FromPath(path) + "\" is a directory");
        return s.GetMemory().CreateBoolean(fs::remove(path));
    });
    // copy(source, destination, overwrite = true)
    Define(state, file, "File.", "copy", [](State& s, Args& args) {
        auto options = BooleanArg(args, 2, true) ? fs::copy_options::overwrite_existing : fs::copy_options::none;
        fs::copy_file(PathArg(args, 0, "source"), PathArg(args, 1, "destination"), options);
        return Nothing(s);
    });
    Define(state, file, "File.", "move", [](State& s, Args& args) {
        fs::rename(PathArg(args, 0, "source"), PathArg(args, 1, "destination"));
        return Nothing(s);
    });
    state.DeclareVariable("File", file);

    ////// DIRECTORY //////

    Variable directory = memory.CreateNamespace();
    // list(path = ".", recursive = false) gives the sorted paths of the entries
    Define(state, directory, "Directory.", "list", [](State& s, Args& args) {
        fs::path root = ToPath(TextArg(args, 0, "path", "."));
        std::vector<std::string> entries;
        if (BooleanArg(args, 1, false)) {
            for (const auto& entry : fs::recursive_directory_iterator(root))
                entries.push_back(FromPath(entry.path()));
        }
        else {
            for (const auto& entry : fs::directory_iterator(root))
                entries.push_back(FromPath(entry.path()));
        }
        std::sort(entries.begin(), entries.end());
        return CreateList(s.GetMemory(), entries);
    });
    // create(path) creates the parent directories too, gives false if the directory already exists
    Define(state, directory, "Directory.", "create", [](State& s, Args& args) {
        return s.GetMemory().CreateBoolean(fs::create_directories(PathArg(args, 0, "path")));
    });
    // remove(path, recursive = false) gives false if the directory doesn't exist
    Define(state, directory, "Directory.", "remove", [](State& s, Args& args) {
        fs::path path = PathArg(args, 0, "path");
        if (!fs::exists(path))
            return s.GetMemory().CreateBoolean(false);
        if (!fs::is_directory(path))
            throw std::runtime_error("\"" + FromPath(path) + "\" isn't a directory");
        if (BooleanArg(args, 1, false))
            fs::remove_all(path);
        else
            fs::remove(path);
        return s.GetMemory().CreateBoolean(true);
    });
    Define(state, directory, "Directory.", "exists", [](State& s, Args& args) {
        std::error_code ec;
        return s.GetMemory().CreateBoolean(fs::is_directory(PathArg(args, 0, "path"), ec));
    });
    Define(state, directory, "Directory.", "current", [](State& s, Args&) {
        return s.GetMemory().CreateString(FromPath(fs::current_path()));
    });
    Define(state, directory, "Directory.", "setCurrent", [](State& s, Args& args) {
        fs::current_path(PathArg(args, 0, "path"));
        return Nothing(s);
    });
    state.DeclareVariable("Directory", directory);

    ////// PATH //////

    Variable path = memory.CreateNamespace();
    Define(state, path, "Path.", "join", [](State& s, Args& args) {
        fs::path result;
        for (std::size_t i = 0; i < args.size(); i++)
            result /= PathArg(args, i, "path");
        return s.GetMemory().CreateString(FromPath(result));
    });
    Define(state, path, "Path.", "directory", [](State& s, Args& args) {
        return s.GetMemory().CreateString(FromPath(PathArg(args, 0, "path").parent_path()));
    });
    Define(state, path, "Path.", "filename", [](State& s, Args& args) {
        return s.GetMemory().CreateString(FromPath(PathArg(args, 0, "path").filename()));
    });
    Define(state, path, "Path.", "stem", [](State& s, Args& args) {
        return s.GetMemory().CreateString(FromPath(PathArg(args, 0, "path").stem()));
    });
    Define(state, path, "Path.", "extension", [](State& s, Args& args) {
        return s.GetMemory().CreateString(FromPath(PathArg(args, 0, "path").extension()));
    });
    Define(state, path, "Path.", "absolute", [](State& s, Args& args) {
        return s.GetMemory().CreateString(FromPath(fs::absolute(PathArg(args, 0, "path")).lexically_normal()));
    });
    Define(state, path, "Path.", "normalize", [](State& s, Args& args) {
        return s.GetMemory().CreateString(FromPath(PathArg(args, 0, "path").lexically_normal()));
    });
    Define(state, path, "Path.", "exists", [](State& s, Args& args) {
        std::error_code ec;
        return s.GetMemory().CreateBoolean(fs::exists(PathArg(args, 0, "path"), ec));
    });
    state.DeclareVariable("Path", path);
}
