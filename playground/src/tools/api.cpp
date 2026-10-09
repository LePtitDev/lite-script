#include "api.hpp"

namespace {

    using Tools::ApiEntry;
    using Kind = ApiEntry::Kind;

    const std::vector<ApiEntry> API = {
        // Console
        { Kind::Function, "print", "print(values...)", "Writes the values separated by spaces and a line break." },
        { Kind::Function, "assert", "assert(condition, message)", "Throws the error `assertion failed: message` if the condition is false." },
        { Kind::Namespace, "Console", "Console", "Console input and output." },
        { Kind::Function, "Console.write", "write(values...)", "Writes the values separated by spaces, without line break." },
        { Kind::Function, "Console.writeLine", "writeLine(values...)", "Writes the values separated by spaces and a line break." },
        { Kind::Function, "Console.error", "error(values...)", "Writes the values on the error output." },
        { Kind::Function, "Console.readLine", "readLine(prompt)", "Reads a line of the standard input, `null` at the end of the input." },
        { Kind::Function, "Console.readAll", "readAll()", "Reads the whole standard input." },

        // Files
        { Kind::Namespace, "File", "File", "Files (paths relative to the current directory, `/` as separator)." },
        { Kind::Function, "File.read", "read(path)", "Content of a text file." },
        { Kind::Function, "File.readLines", "readLines(path)", "Array of the lines of a text file." },
        { Kind::Function, "File.write", "write(path, text)", "Replaces the content of a file." },
        { Kind::Function, "File.append", "append(path, text)", "Adds text at the end of a file." },
        { Kind::Function, "File.exists", "exists(path)", "Indicates if a file exists." },
        { Kind::Function, "File.size", "size(path)", "Size of a file in bytes." },
        { Kind::Function, "File.copy", "copy(source, destination, overwrite)", "Copies a file (replaces the destination by default)." },
        { Kind::Function, "File.move", "move(source, destination)", "Moves or renames a file." },
        { Kind::Function, "File.remove", "remove(path)", "Deletes a file, `false` if it doesn't exist." },
        { Kind::Namespace, "Directory", "Directory", "Directories." },
        { Kind::Function, "Directory.list", "list(path, recursive)", "Sorted array of the paths of the entries (`\".\"` by default)." },
        { Kind::Function, "Directory.create", "create(path)", "Creates a directory and its parents, `false` if it already exists." },
        { Kind::Function, "Directory.remove", "remove(path, recursive)", "Deletes a directory (with its content if `recursive` is true), `false` if it doesn't exist." },
        { Kind::Function, "Directory.exists", "exists(path)", "Indicates if a directory exists." },
        { Kind::Function, "Directory.current", "current()", "The current directory." },
        { Kind::Function, "Directory.setCurrent", "setCurrent(path)", "Changes the current directory." },
        { Kind::Namespace, "Path", "Path", "Paths (`/` as separator)." },
        { Kind::Function, "Path.join", "join(parts...)", "Joins parts of a path." },
        { Kind::Function, "Path.directory", "directory(path)", "`\"a/b\"` for `\"a/b/c.lsc\"`." },
        { Kind::Function, "Path.filename", "filename(path)", "`\"c.lsc\"` for `\"a/b/c.lsc\"`." },
        { Kind::Function, "Path.stem", "stem(path)", "`\"c\"` for `\"a/b/c.lsc\"`." },
        { Kind::Function, "Path.extension", "extension(path)", "`\".lsc\"` for `\"a/b/c.lsc\"`." },
        { Kind::Function, "Path.absolute", "absolute(path)", "Absolute path." },
        { Kind::Function, "Path.normalize", "normalize(path)", "Path without `.` and `..`." },
        { Kind::Function, "Path.exists", "exists(path)", "Indicates if a file or a directory exists." },

        // JSON
        { Kind::Namespace, "JSON", "JSON", "Conversions between values and JSON." },
        { Kind::Function, "JSON.parse", "parse(text)", "Converts a JSON text into values (objects become arrays with named elements). An invalid text throws an error." },
        { Kind::Function, "JSON.stringify", "stringify(value, indent)", "JSON text of a value, `indent` is a number of spaces or a text (compact by default)." },

        // HTTP
        { Kind::Namespace, "Http", "Http", "HTTP(S) requests. The result is `{ status, ok, headers, body }`." },
        { Kind::Function, "Http.get", "get(url, headers)", "Sends a GET request." },
        { Kind::Function, "Http.delete", "delete(url, headers)", "Sends a DELETE request." },
        { Kind::Function, "Http.post", "post(url, body, headers)", "Sends a POST request (a body that isn't a string is sent in JSON)." },
        { Kind::Function, "Http.put", "put(url, body, headers)", "Sends a PUT request (a body that isn't a string is sent in JSON)." },
        { Kind::Function, "Http.patch", "patch(url, body, headers)", "Sends a PATCH request (a body that isn't a string is sent in JSON)." },
        { Kind::Function, "Http.request", "request(options)", "Sends a request described by `{ url, method, headers, body, timeout, followRedirects }`." },
        { Kind::Value, "Http.available", "available", "`false` if the playground has been built without HTTP client." },

        // Process and time
        { Kind::Namespace, "Process", "Process", "The running program." },
        { Kind::Value, "Process.args", "args", "Array of the arguments given after `--`." },
        { Kind::Value, "Process.script", "script", "Absolute path of the script being executed." },
        { Kind::Value, "Process.platform", "platform", "`\"windows\"`, `\"linux\"`, `\"macos\"` or `\"unknown\"`." },
        { Kind::Function, "Process.env", "env(name)", "Value of an environment variable, `null` if it isn't defined." },
        { Kind::Function, "Process.run", "run(command)", "Executes a command with the shell, gives `{ code, output }`." },
        { Kind::Function, "Process.exit", "exit(code)", "Stops the program." },
        { Kind::Namespace, "Time", "Time", "Dates and durations." },
        { Kind::Function, "Time.now", "now()", "Milliseconds since 1970-01-01 UTC." },
        { Kind::Function, "Time.clock", "clock()", "Milliseconds (with decimals) of a monotonic clock, to measure durations." },
        { Kind::Function, "Time.sleep", "sleep(ms)", "Waits." },
        { Kind::Function, "Time.iso", "iso(ms)", "Date in the ISO 8601 format, the current date by default." },

        // Math
        { Kind::Namespace, "Math", "Math", "Mathematical functions (they give `null` if an argument isn't a number)." },
        { Kind::Function, "Math.abs", "abs(x)", "Absolute value." },
        { Kind::Function, "Math.sign", "sign(x)", "Sign: -1, 0 or 1." },
        { Kind::Function, "Math.floor", "floor(x)", "Rounds down." },
        { Kind::Function, "Math.ceil", "ceil(x)", "Rounds up." },
        { Kind::Function, "Math.round", "round(x)", "Rounds to the nearest integer." },
        { Kind::Function, "Math.trunc", "trunc(x)", "Integer part." },
        { Kind::Function, "Math.sqrt", "sqrt(x)", "Square root." },
        { Kind::Function, "Math.pow", "pow(x, y)", "Power." },
        { Kind::Function, "Math.exp", "exp(x)", "Exponential." },
        { Kind::Function, "Math.log", "log(x)", "Natural logarithm." },
        { Kind::Function, "Math.sin", "sin(x)", "Sine (radians)." },
        { Kind::Function, "Math.cos", "cos(x)", "Cosine (radians)." },
        { Kind::Function, "Math.tan", "tan(x)", "Tangent (radians)." },
        { Kind::Function, "Math.asin", "asin(x)", "Arc sine (radians)." },
        { Kind::Function, "Math.acos", "acos(x)", "Arc cosine (radians)." },
        { Kind::Function, "Math.atan", "atan(x)", "Arc tangent (radians)." },
        { Kind::Function, "Math.atan2", "atan2(y, x)", "Angle of the point (x, y) (radians)." },
        { Kind::Function, "Math.min", "min(values...)", "Minimum." },
        { Kind::Function, "Math.max", "max(values...)", "Maximum." },
        { Kind::Function, "Math.random", "random(a, b)", "`random()`: number in [0, 1[ · `random(n)`: integer in [0, n[ · `random(a, b)`: integer in [a, b]." },
        { Kind::Function, "Math.seed", "seed(n)", "Seeds the random generator (to reproduce draws)." },
        { Kind::Value, "Math.PI", "PI", "3.14159…" },
        { Kind::Value, "Math.E", "E", "2.71828…" },

        // Conversions
        { Kind::Function, "typeOf", "typeOf(value)", "Type name: `\"undefined\"`, `\"null\"`, `\"boolean\"`, `\"number\"`, `\"string\"`, `\"callback\"`, `\"array\"`, `\"class\"`, `\"class object\"`, `\"namespace\"`." },
        { Kind::Function, "toString", "toString(value)", "Text of a value." },
        { Kind::Function, "parseNumber", "parseNumber(text)", "Number written in a string, `null` if the string isn't a number." },
        { Kind::Function, "parseInt", "parseInt(text, base)", "Integer written in a string in a base from 2 to 36 (10 by default)." },
        { Kind::Function, "isNaN", "isNaN(value)", "True if the value isn't a number or is `NaN`." },
        { Kind::Function, "char", "char(code)", "Character of a Unicode code." },

        // Arrays
        { Kind::ArrayMethod, "push", "push(values...)", "Appends elements at the end, returns the new size." },
        { Kind::ArrayMethod, "pop", "pop()", "Removes and returns the last element." },
        { Kind::ArrayMethod, "insert", "insert(index, value)", "Inserts an element at a position." },
        { Kind::ArrayMethod, "remove", "remove(index)", "Removes and returns the element at a position (negative: from the end)." },
        { Kind::ArrayMethod, "clear", "clear()", "Removes all indexed elements." },
        { Kind::ArrayMethod, "indexOf", "indexOf(value)", "Position of an element (-1 if absent). For a string: `indexOf(text, from)`." },
        { Kind::ArrayMethod, "contains", "contains(value)", "Presence of an element. For a string: presence of a text." },
        { Kind::ArrayMethod, "join", "join(separator)", "Text of the elements joined by a separator (`,` by default)." },
        { Kind::ArrayMethod, "slice", "slice(start, end)", "New array with part of the elements (negative positions from the end)." },
        { Kind::ArrayMethod, "reverse", "reverse()", "Reverses the order of the elements." },
        { Kind::ArrayMethod, "sort", "sort(compare)", "Sorts the elements (stable), `compare(a, b)` returns a negative number if `a` goes before `b`." },
        { Kind::ArrayMethod, "keys", "keys()", "Array of the names of the named elements." },
        { Kind::ArrayMethod, "map", "map(f)", "New array of the results of `f(element, index)`." },
        { Kind::ArrayMethod, "filter", "filter(f)", "New array of the elements for which `f(element, index)` is true." },
        { Kind::ArrayMethod, "forEach", "forEach(f)", "Calls `f(element, index)` for each element." },
        { Kind::ArrayMethod, "length", "length", "Number of indexed elements (characters for a string)." },

        // Strings
        { Kind::StringMethod, "substring", "substring(start, end)", "Part of the string." },
        { Kind::StringMethod, "startsWith", "startsWith(text)", "Indicates if the string starts with a text." },
        { Kind::StringMethod, "endsWith", "endsWith(text)", "Indicates if the string ends with a text." },
        { Kind::StringMethod, "split", "split(separator)", "Array of the parts (empty separator: one element per character)." },
        { Kind::StringMethod, "replace", "replace(a, b)", "Replaces all the occurrences of `a` with `b`." },
        { Kind::StringMethod, "trim", "trim()", "Removes the spaces at the start and at the end." },
        { Kind::StringMethod, "toUpperCase", "toUpperCase()", "Upper case (Latin letters)." },
        { Kind::StringMethod, "toLowerCase", "toLowerCase()", "Lower case (Latin letters)." },
        { Kind::StringMethod, "charCode", "charCode(index)", "Unicode code of the character at a position." },
    };

}

const std::vector<std::string_view> Tools::KEYWORDS = {
    "var", "function", "class", "static", "operator", "new", "this", "return", "if", "else", "for", "while",
    "do", "switch", "case", "default", "break", "continue", "try", "catch", "throw", "namespace", "true",
    "false", "null", "undefined",
};

const std::vector<Tools::ApiEntry>& Tools::GetApi() {
    return API;
}

const Tools::ApiEntry * Tools::FindApi(std::string_view name) {
    for (const ApiEntry& entry : API) {
        if (entry.name == name && entry.kind != Kind::ArrayMethod && entry.kind != Kind::StringMethod)
            return &entry;
    }
    return nullptr;
}
