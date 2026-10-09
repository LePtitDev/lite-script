#include "tools.hpp"

#include "../modules.hpp"
#include "jsonvalue.hpp"
#include "transport.hpp"

#include <algorithm>
#include <atomic>
#include <cctype>
#include <condition_variable>
#include <deque>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <optional>
#include <set>
#include <sstream>
#include <streambuf>
#include <thread>

#include <litescript/litescript.hpp>

#ifdef _WIN32
// Conflicts with Script::Result::GetMessage
#undef GetMessage
#endif

namespace {

    using namespace LiteScript;
    using Tools::Json;
    using Analysis = Script::Analysis;
    namespace fs = std::filesystem;

    // Thrown by the hooks to stop the execution (not an std::exception: the native functions don't convert it)
    struct TerminateRequest {};

    // Thrown by Process.exit
    struct ExitRequest {
        int code;
    };

    class DebugAdapter;

    // Send what the script writes in the standard outputs as "output" events
    class OutputEvents : public std::streambuf {
        DebugAdapter& adapter;
        std::string category;
        std::string buffer;
    public:
        OutputEvents(DebugAdapter& a, std::string c) : adapter(a), category(std::move(c)) {}
        void Flush();
    protected:
        int overflow(int c) override {
            if (c != traits_type::eof()) {
                this->buffer += static_cast<char>(c);
                if (c == '\n')
                    this->Flush();
            }
            return c;
        }
        std::streamsize xsputn(const char * s, std::streamsize n) override {
            this->buffer.append(s, static_cast<std::size_t>(n));
            if (this->buffer.find('\n') != std::string::npos || this->buffer.size() > 4096)
                this->Flush();
            return n;
        }
        int sync() override {
            this->Flush();
            return 0;
        }
    };

    // An empty input (the standard input contains the messages of the protocol)
    class EmptyInput : public std::streambuf {};

    // A breakpoint of the program
    struct Breakpoint {
        std::int64_t id = 0;
        std::size_t line = 0;
        std::string condition;
        std::string log_message;
    };

    // A value that can be expanded in the variables view
    struct Handle {
        enum class Kind { Value, Locals, Closure, Globals } kind = Kind::Value;
        Variable value;
        std::size_t frame = 0;
    };

    class DebugAdapter {

        Tools::Transport& transport;
        std::int64_t sequence = 1;

        // The messages read by the reader thread
        std::mutex queue_mutex;
        std::condition_variable queue_signal;
        std::deque<Json> queue;
        std::atomic<bool> pending{ false };

        // The program
        std::optional<Memory> memory;
        std::optional<State> state;
        std::string program_path;
        std::string program_code;
        std::vector<std::string> program_args;
        Analysis analysis;
        std::vector<Instruction> instructions;
        std::size_t program_list = 0;
        std::set<std::size_t> statements;
        std::set<std::string> library_names;
        bool no_debug = false;
        bool stop_on_entry = false;
        bool launched = false;
        bool started = false;
        bool finished = false;

        // The breakpoints by instruction
        std::map<std::size_t, Breakpoint> breakpoints;
        std::int64_t next_breakpoint = 1;
        bool break_on_errors = true;

        // The execution control
        enum class Mode { Run, StepIn, StepOver, StepOut } mode = Mode::Run;
        std::size_t step_depth = 0;
        std::atomic<bool> pause_requested{ false };
        bool paused = false;
        bool terminate = false;
        bool disconnected = false;
        std::string exception_message;
        std::vector<State::FrameInfo> frames;
        std::vector<Handle> handles;

    public:

        OutputEvents stdout_events{ *this, "stdout" };
        OutputEvents stderr_events{ *this, "stderr" };

        explicit DebugAdapter(Tools::Transport& t) : transport(t) {}

        int Run() {
            std::thread reader([this]() {
                while (auto message = this->transport.Read()) {
                    std::lock_guard<std::mutex> lock(this->queue_mutex);
                    this->queue.push_back(std::move(*message));
                    this->pending = true;
                    this->queue_signal.notify_one();
                }
                // End of the input: as a disconnection
                Json end;
                end["type"] = "request";
                end["command"] = "disconnect";
                std::lock_guard<std::mutex> lock(this->queue_mutex);
                this->queue.push_back(std::move(end));
                this->pending = true;
                this->queue_signal.notify_one();
            });
            // The thread is blocked on the input until the end of the program
            reader.detach();

            while (!this->disconnected) {
                this->HandleMessage(this->WaitMessage());
                if (this->started && !this->finished)
                    this->Execute();
            }
            return 0;
        }

        void SendEvent(const std::string& name, Json body = Json::MakeObject()) {
            Json message;
            message["seq"] = this->sequence++;
            message["type"] = "event";
            message["event"] = name;
            message["body"] = std::move(body);
            this->transport.Write(message);
        }

        void Output(const std::string& category, const std::string& text) {
            Json body;
            body["category"] = category;
            body["output"] = text;
            this->SendEvent("output", std::move(body));
        }

    private:

        ////// MESSAGES //////

        Json WaitMessage() {
            std::unique_lock<std::mutex> lock(this->queue_mutex);
            this->queue_signal.wait(lock, [this]() { return !this->queue.empty(); });
            Json message = std::move(this->queue.front());
            this->queue.pop_front();
            this->pending = !this->queue.empty();
            return message;
        }

        std::optional<Json> TryMessage() {
            std::lock_guard<std::mutex> lock(this->queue_mutex);
            if (this->queue.empty())
                return std::nullopt;
            Json message = std::move(this->queue.front());
            this->queue.pop_front();
            this->pending = !this->queue.empty();
            return message;
        }

        void Respond(const Json& request, Json body = Json::MakeObject()) {
            Json message;
            message["seq"] = this->sequence++;
            message["type"] = "response";
            message["request_seq"] = request["seq"];
            message["success"] = true;
            message["command"] = request["command"];
            message["body"] = std::move(body);
            this->transport.Write(message);
        }

        void RespondError(const Json& request, const std::string& text) {
            Json message;
            message["seq"] = this->sequence++;
            message["type"] = "response";
            message["request_seq"] = request["seq"];
            message["success"] = false;
            message["command"] = request["command"];
            message["message"] = text;
            this->transport.Write(message);
        }

        void HandleMessage(const Json& message) {
            if (message["type"].GetString() != "request")
                return;
            const std::string& command = message["command"].GetString();
            const Json& args = message["arguments"];
            try {
                if (command == "initialize")
                    this->Respond(message, this->Capabilities());
                else if (command == "launch")
                    this->Launch(message, args);
                else if (command == "setBreakpoints")
                    this->Respond(message, this->SetBreakpoints(args));
                else if (command == "setExceptionBreakpoints") {
                    this->break_on_errors = false;
                    for (std::size_t i = 0; i < args["filters"].Size(); i++)
                        this->break_on_errors |= args["filters"].At(i).GetString() == "uncaught";
                    this->Respond(message);
                }
                else if (command == "configurationDone") {
                    this->Respond(message);
                    this->started = this->launched;
                }
                else if (command == "threads") {
                    Json thread;
                    thread["id"] = 1;
                    thread["name"] = "main";
                    Json body;
                    body["threads"].Push(std::move(thread));
                    this->Respond(message, std::move(body));
                }
                else if (command == "stackTrace")
                    this->Respond(message, this->StackTrace(args));
                else if (command == "scopes")
                    this->Respond(message, this->Scopes(args));
                else if (command == "variables")
                    this->Respond(message, this->Variables(args));
                else if (command == "setVariable")
                    this->Respond(message, this->SetVariable(args));
                else if (command == "evaluate")
                    this->Respond(message, this->EvaluateRequest(args));
                else if (command == "exceptionInfo") {
                    Json body;
                    body["exceptionId"] = "error";
                    body["description"] = this->exception_message;
                    body["breakMode"] = "unhandled";
                    this->Respond(message, std::move(body));
                }
                else if (command == "continue" || command == "next" || command == "stepIn" || command == "stepOut") {
                    this->mode = command == "continue" ? Mode::Run : command == "next" ? Mode::StepOver : command == "stepIn" ? Mode::StepIn : Mode::StepOut;
                    this->step_depth = this->state ? this->state->GetDepth() : 0;
                    this->paused = false;
                    Json body;
                    if (command == "continue")
                        body["allThreadsContinued"] = true;
                    this->Respond(message, std::move(body));
                }
                else if (command == "pause") {
                    this->pause_requested = true;
                    this->Respond(message);
                }
                else if (command == "terminate" || command == "disconnect") {
                    this->terminate = true;
                    this->paused = false;
                    if (command == "disconnect")
                        this->disconnected = true;
                    this->Respond(message);
                    if (command == "terminate" && (!this->started || this->finished))
                        this->SendEvent("terminated");
                }
                else
                    this->RespondError(message, "unsupported request " + command);
            }
            catch (const ScriptError& error) {
                this->RespondError(message, error.what());
            }
            catch (const std::exception& error) {
                this->RespondError(message, error.what());
            }
        }

        Json Capabilities() {
            Json body;
            body["supportsConfigurationDoneRequest"] = true;
            body["supportsEvaluateForHovers"] = true;
            body["supportsConditionalBreakpoints"] = true;
            body["supportsLogPoints"] = true;
            body["supportsSetVariable"] = true;
            body["supportsTerminateRequest"] = true;
            body["supportsExceptionInfoRequest"] = true;
            Json filter;
            filter["filter"] = "uncaught";
            filter["label"] = "Uncaught errors";
            filter["default"] = true;
            body["exceptionBreakpointFilters"].Push(std::move(filter));
            return body;
        }

        ////// LAUNCH //////

        void Launch(const Json& request, const Json& args) {
            this->program_path = args["program"].GetString();
            this->no_debug = args["noDebug"].GetBool(false);
            this->stop_on_entry = args["stopOnEntry"].GetBool(false);
            for (std::size_t i = 0; i < args["args"].Size(); i++)
                this->program_args.push_back(args["args"].At(i).GetString());
            std::error_code ec;
            if (!args["cwd"].GetString().empty())
                fs::current_path(fs::path(std::u8string(args["cwd"].GetString().begin(), args["cwd"].GetString().end())), ec);
            fs::path path(std::u8string(this->program_path.begin(), this->program_path.end()));
            std::ifstream file(path, std::ios::binary);
            if (this->program_path.empty() || !file.is_open()) {
                this->RespondError(request, "can't open the file \"" + this->program_path + "\"");
                return;
            }
            std::ostringstream content;
            content << file.rdbuf();
            this->program_code = content.str();
            if (this->program_code.starts_with("\xEF\xBB\xBF"))
                this->program_code.erase(0, 3);
            if (this->program_code.starts_with("#!"))
                this->program_code.insert(0, "//");
            Script::Result result = Script::Compile(this->program_code, &this->analysis);
            if (!result) {
                this->RespondError(request, this->program_path + ":" + std::to_string(result.line + 1) + ":" +
                                                std::to_string(result.column + 1) + ": syntax error: " + std::string(result.GetMessage()));
                return;
            }
            this->instructions = std::move(result.instructions);
            for (const auto& statement : this->analysis.statements)
                this->statements.insert(statement.instruction);

            // The environment of the playground
            this->memory.emplace();
            this->state.emplace(*this->memory);
            Standard::Install(*this->state);
            std::string absolute;
            {
                std::u8string text = fs::absolute(path, ec).lexically_normal().generic_u8string();
                absolute.assign(text.begin(), text.end());
            }
            Playground::Install(*this->state, Playground::Context{ this->program_args, absolute });
            Playground::SetCurrentScript(*this->state, absolute);
            // Process.exit stops the debugging session instead of the debug adapter
            Variable process = this->state->GetGlobalNamespace().As<Namespace>().variables.Find("Process");
            if (process.Is<Namespace>()) {
                process.As<Namespace>().variables.Set("exit", this->memory->CreateCallback([](State&, std::vector<Variable>& a) -> Variable {
                    std::cout.flush();
                    std::cerr.flush();
                    throw ExitRequest{ !a.empty() && a[0].Is<Number>() ? static_cast<int>(a[0].As<Number>().ToInteger()) : 0 };
                }));
            }
            for (const auto& [name, value] : this->state->GetGlobalNamespace().As<Namespace>().variables)
                this->library_names.insert(name);
            this->launched = true;
            this->Respond(request);
            // The breakpoints can be set now that the program is compiled
            this->SendEvent("initialized");
        }

        void Execute() {
            State& s = *this->state;
            this->program_list = s.AddInstructions(this->instructions);
            if (!this->no_debug) {
                s.SetDebugHook([this](State& st) { this->OnInstruction(st); });
                s.SetErrorHook([this](State& st, const ScriptError& error) { this->OnError(st, error); });
            }
            int exit_code = 0;
            try {
                s.Execute();
            }
            catch (const ScriptError& error) {
                std::cout.flush();
                SourcePosition position = error.GetPosition();
                std::cerr << this->program_path << ":" << position.line + 1 << ":" << position.column + 1 << ": error: " << error.what() << std::endl;
                exit_code = 1;
            }
            catch (const ExitRequest& request) {
                exit_code = request.code;
            }
            catch (const TerminateRequest&) {
                exit_code = 1;
            }
            catch (const std::exception& error) {
                std::cout.flush();
                std::cerr << this->program_path << ": error: " << error.what() << std::endl;
                exit_code = 1;
            }
            s.SetDebugHook({});
            s.SetErrorHook({});
            std::cout.flush();
            std::cerr.flush();
            this->finished = true;
            this->paused = false;
            Json body;
            body["exitCode"] = exit_code;
            this->SendEvent("exited", std::move(body));
            this->SendEvent("terminated");
        }

        ////// EXECUTION CONTROL //////

        void OnInstruction(State& s) {
            if (this->pending) {
                while (auto message = this->TryMessage())
                    this->HandleMessage(*message);
            }
            if (this->terminate)
                throw TerminateRequest{};
            if (s.GetListIndex() != this->program_list)
                return;
            std::size_t line = s.GetLine();
            if (this->stop_on_entry) {
                this->stop_on_entry = false;
                this->Stop("entry");
                return;
            }
            if (this->statements.count(line) == 0)
                return;
            if (this->pause_requested) {
                this->pause_requested = false;
                this->Stop("pause");
                return;
            }
            auto bp = this->breakpoints.find(line);
            if (bp != this->breakpoints.end() && this->BreakpointHit(bp->second)) {
                this->Stop("breakpoint", "", bp->second.id);
                return;
            }
            std::size_t depth = s.GetDepth();
            if ((this->mode == Mode::StepIn) || (this->mode == Mode::StepOver && depth <= this->step_depth) ||
                (this->mode == Mode::StepOut && depth < this->step_depth))
                this->Stop("step");
        }

        void OnError(State&, const ScriptError& error) {
            if (!this->break_on_errors)
                return;
            this->exception_message = error.what();
            this->Stop("exception", error.what());
        }

        // Evaluate the condition and the message of a breakpoint
        bool BreakpointHit(const Breakpoint& bp) {
            if (!bp.condition.empty()) {
                try {
                    if (!IsTruthy(this->Evaluate(bp.condition)))
                        return false;
                }
                catch (...) {
                    this->Output("console", "The condition of the breakpoint can't be evaluated: " + bp.condition + "\n");
                }
            }
            if (!bp.log_message.empty()) {
                // The expressions between braces are replaced by their values
                std::string text;
                for (std::size_t i = 0; i < bp.log_message.size(); i++) {
                    std::size_t end;
                    if (bp.log_message[i] == '{' && (end = bp.log_message.find('}', i)) != std::string::npos) {
                        try {
                            text += ToText(this->Evaluate(bp.log_message.substr(i + 1, end - i - 1))).ToUTF8();
                        }
                        catch (...) {
                            text += "<error>";
                        }
                        i = end;
                    }
                    else {
                        text += bp.log_message[i];
                    }
                }
                this->Output("console", text + "\n");
                return false;
            }
            return true;
        }

        // Pause the execution until a resume request
        void Stop(const std::string& reason, const std::string& text = "", std::int64_t breakpoint = 0) {
            this->stdout_events.Flush();
            this->stderr_events.Flush();
            this->mode = Mode::Run;
            this->paused = true;
            this->frames = this->state->GetFrames();
            this->handles.clear();
            Json body;
            body["reason"] = reason;
            body["threadId"] = 1;
            body["allThreadsStopped"] = true;
            if (!text.empty())
                body["text"] = text;
            if (breakpoint != 0)
                body["hitBreakpointIds"].Push(Json(breakpoint));
            this->SendEvent("stopped", std::move(body));
            while (this->paused)
                this->HandleMessage(this->WaitMessage());
            this->handles.clear();
            this->frames.clear();
            if (this->terminate)
                throw TerminateRequest{};
        }

        ////// BREAKPOINTS //////

        Json SetBreakpoints(const Json& args) {
            Json body;
            body["breakpoints"] = Json::MakeArray();
            this->breakpoints.clear();
            for (std::size_t i = 0; i < args["breakpoints"].Size(); i++) {
                const Json& requested = args["breakpoints"].At(i);
                std::size_t line = static_cast<std::size_t>(std::max<std::int64_t>(1, requested["line"].GetInt())) - 1;
                // The first statement on the line or on the next lines
                const Analysis::Statement * found = nullptr;
                for (const auto& statement : this->analysis.statements) {
                    if (statement.instruction >= this->instructions.size() || statement.position.line < line)
                        continue;
                    if (found == nullptr || statement.position.line < found->position.line ||
                        (statement.position.line == found->position.line && statement.instruction < found->instruction))
                        found = &statement;
                }
                Json result;
                result["verified"] = found != nullptr;
                if (found != nullptr) {
                    Breakpoint bp;
                    bp.id = this->next_breakpoint++;
                    bp.line = found->position.line;
                    bp.condition = requested["condition"].GetString();
                    bp.log_message = requested["logMessage"].GetString();
                    result["id"] = bp.id;
                    result["line"] = static_cast<std::int64_t>(found->position.line + 1);
                    this->breakpoints[found->instruction] = std::move(bp);
                }
                else {
                    result["message"] = "No code at this line or after it";
                }
                body["breakpoints"].Push(std::move(result));
            }
            return body;
        }

        ////// STACK AND VARIABLES //////

        void RequirePaused() const {
            if (!this->paused)
                throw std::runtime_error("the program isn't paused");
        }

        // The name of the function that contains an instruction
        std::string FunctionName(std::size_t list, std::size_t line) const {
            if (list != this->program_list)
                return "(evaluation)";
            const Analysis::Symbol * best = nullptr;
            for (const auto& s : this->analysis.symbols) {
                if (s.first_instruction == Analysis::NONE || line < s.first_instruction || line >= s.end_instruction)
                    continue;
                if (best == nullptr || s.end_instruction - s.first_instruction < best->end_instruction - best->first_instruction)
                    best = &s;
            }
            if (best == nullptr)
                return "(anonymous function)";
            for (const auto& owner : this->analysis.symbols) {
                if (owner.body != Analysis::NONE && owner.body == best->scope && owner.kind == Analysis::SymbolKind::Class)
                    return owner.name + "." + (best->kind == Analysis::SymbolKind::Operator ? "operator " : "") + best->name;
            }
            return best->name;
        }

        SourcePosition PositionOf(std::size_t list, std::size_t line) const {
            const auto& code = this->state->GetInstructions(list);
            if (code.empty())
                return SourcePosition{};
            return code[std::min(line, code.size() - 1)].position;
        }

        Json Source() const {
            Json source;
            fs::path path(std::u8string(this->program_path.begin(), this->program_path.end()));
            std::u8string name = path.filename().u8string();
            source["name"] = std::string(name.begin(), name.end());
            source["path"] = this->program_path;
            return source;
        }

        Json StackTrace(const Json&) {
            this->RequirePaused();
            Json body;
            body["stackFrames"] = Json::MakeArray();
            for (std::size_t k = this->frames.size(); k-- > 0;) {
                const auto& frame = this->frames[k];
                SourcePosition position = this->PositionOf(frame.list, frame.line);
                Json item;
                item["id"] = static_cast<std::int64_t>(k + 1);
                item["name"] = k == 0 ? std::string("(script)") : this->FunctionName(frame.list, frame.line);
                item["line"] = static_cast<std::int64_t>(position.line) + 1;
                item["column"] = static_cast<std::int64_t>(position.column) + 1;
                if (frame.list == this->program_list)
                    item["source"] = this->Source();
                body["stackFrames"].Push(std::move(item));
            }
            body["totalFrames"] = this->frames.size();
            return body;
        }

        std::int64_t AddHandle(Handle handle) {
            this->handles.push_back(std::move(handle));
            return static_cast<std::int64_t>(this->handles.size());
        }

        const State::FrameInfo& FrameOf(const Json& id) const {
            std::int64_t index = id.GetInt(static_cast<std::int64_t>(this->frames.size()));
            if (index < 1 || static_cast<std::size_t>(index) > this->frames.size())
                throw std::runtime_error("invalid frame");
            return this->frames[static_cast<std::size_t>(index - 1)];
        }

        // The namespaces of local variables of a frame (the last one is the innermost)
        static std::vector<Variable> LocalScopes(const State::FrameInfo& frame) {
            std::vector<Variable> res;
            for (const Variable& scope : frame.namer.scopes) {
                if (scope.Is<Namespace>())
                    res.push_back(scope);
            }
            return res;
        }

        Json Scopes(const Json& args) {
            this->RequirePaused();
            std::size_t index = static_cast<std::size_t>(args["frameId"].GetInt(1));
            const auto& frame = this->FrameOf(args["frameId"]);
            Json body;
            body["scopes"] = Json::MakeArray();
            auto add = [&](const std::string& name, std::int64_t reference, bool expensive) {
                Json scope;
                scope["name"] = name;
                scope["variablesReference"] = reference;
                scope["expensive"] = expensive;
                body["scopes"].Push(std::move(scope));
            };
            auto locals = LocalScopes(frame);
            if (index > 1 || !locals.empty())
                add("Locals", this->AddHandle({ Handle::Kind::Locals, Variable(), index - 1 }), false);
            if (locals.size() > 1)
                add("Closure", this->AddHandle({ Handle::Kind::Closure, Variable(), index - 1 }), false);
            if (frame.namer.current && frame.namer.global && !(frame.namer.current == frame.namer.global) &&
                &frame.namer.current.As<Namespace>() != &frame.namer.global.As<Namespace>())
                add("Namespace", this->AddHandle({ Handle::Kind::Value, frame.namer.current, index - 1 }), false);
            add("Globals", this->AddHandle({ Handle::Kind::Globals, Variable(), index - 1 }), false);
            return body;
        }

        // The name of a class (searched in the global namespace and its namespaces)
        std::string ClassName(const Variable& cls) const {
            if (!cls || !cls.Is<Class>())
                return "class";
            const Class * target = &cls.As<Class>();
            std::vector<std::pair<std::string, Variable>> visit{ { "", this->state->GetGlobalNamespace() } };
            for (std::size_t i = 0; i < visit.size() && i < 64; i++) {
                auto [prefix, nsp] = visit[i];
                for (const auto& [name, value] : nsp.As<Namespace>().variables) {
                    if (value.Is<Class>() && &value.As<Class>() == target)
                        return prefix + name;
                    if (value.Is<Namespace>() && this->library_names.count(name) == 0)
                        visit.emplace_back(prefix + name + ".", value);
                    if (value.Is<Class>()) {
                        for (const auto& [inner, member] : value.As<Class>().statics) {
                            if (member.Is<Class>() && &member.As<Class>() == target)
                                return prefix + name + "." + inner;
                        }
                    }
                }
            }
            return "class";
        }

        // Escape a string for the display
        static std::string Quote(const std::string& text, std::size_t limit) {
            std::string res = "\"";
            for (char c : text.substr(0, limit)) {
                switch (c) {
                    case '"': res += "\\\""; break;
                    case '\\': res += "\\\\"; break;
                    case '\n': res += "\\n"; break;
                    case '\r': res += "\\r"; break;
                    case '\t': res += "\\t"; break;
                    default: res += c;
                }
            }
            return res + (text.size() > limit ? "…\"" : "\"");
        }

        // A short description of a value
        std::string Format(const Variable& v, int depth = 2) const {
            if (!v)
                return "undefined";
            switch (v.GetType()) {
                case Type::String:
                case Type::Character:
                    return Quote(ToText(v).ToUTF8(), 200);
                case Type::Callback: {
                    const Callback& callback = v.As<Callback>();
                    if (callback.IsNative())
                        return "function (native)";
                    const auto& function = std::get<Callback::ScriptFunction>(callback.target);
                    for (const auto& s : this->analysis.symbols) {
                        if (function.list == this->program_list && s.first_instruction == function.line)
                            return "function " + s.name + "(" + Join(s.parameters) + ")";
                    }
                    return "function";
                }
                case Type::Array: {
                    const Array& array = v.As<Array>();
                    if (depth <= 0)
                        return array.named.Count() > 0 ? "{…}" : "[…]";
                    std::string res;
                    std::size_t count = 0;
                    auto append = [&](const std::string& item) {
                        if (count++ > 0)
                            res += ", ";
                        res += item;
                    };
                    for (std::size_t i = 0; i < array.indexed.size() && res.size() < 80; i++)
                        append(array.named.Count() > 0 ? "[" + std::to_string(i) + "]: " + this->Format(array.indexed[i], depth - 1) : this->Format(array.indexed[i], depth - 1));
                    for (const auto& [name, value] : array.named) {
                        if (res.size() >= 80)
                            break;
                        append(name + ": " + this->Format(value, depth - 1));
                    }
                    if (count < array.indexed.size() + array.named.Count())
                        res += ", …";
                    return array.named.Count() > 0 ? "{" + res + "}" : "(" + std::to_string(array.indexed.size()) + ") [" + res + "]";
                }
                case Type::Class:
                    return "class " + this->ClassName(v);
                case Type::ClassObject: {
                    const ClassObject& instance = v.As<ClassObject>();
                    std::string res = this->ClassName(instance.base);
                    if (depth <= 0)
                        return res + " {…}";
                    std::string members;
                    for (const auto& [name, value] : instance.members) {
                        if (members.size() >= 80) {
                            members += ", …";
                            break;
                        }
                        members += (members.empty() ? "" : ", ") + name + ": " + this->Format(value, depth - 1);
                    }
                    return res + " {" + members + "}";
                }
                case Type::Namespace:
                    return "namespace";
                default:
                    return v.ToString();
            }
        }

        static std::string Join(const std::vector<std::string>& parts) {
            std::string res;
            for (const auto& p : parts)
                res += (res.empty() ? "" : ", ") + p;
            return res;
        }

        static bool Expandable(const Variable& v) {
            return v && (v.Is<Array>() || v.Is<ClassObject>() || v.Is<Class>() || v.Is<Namespace>());
        }

        Json Describe(const std::string& name, const Variable& value, const std::string& evaluate_name = "") {
            Json item;
            item["name"] = name;
            item["value"] = this->Format(value);
            item["type"] = std::string(GetTypeName(value ? value.GetType() : Type::Undefined));
            item["variablesReference"] = Expandable(value) ? this->AddHandle({ Handle::Kind::Value, value, 0 }) : 0;
            if (!evaluate_name.empty())
                item["evaluateName"] = evaluate_name;
            return item;
        }

        Json Variables(const Json& args) {
            this->RequirePaused();
            std::int64_t reference = args["variablesReference"].GetInt();
            if (reference < 1 || static_cast<std::size_t>(reference) > this->handles.size())
                throw std::runtime_error("invalid reference");
            Handle handle = this->handles[static_cast<std::size_t>(reference - 1)];
            Json body;
            Json& list = body["variables"] = Json::MakeArray();
            constexpr std::size_t LIMIT = 1000;
            auto members = [&](const MemberList& values, const std::string& prefix, bool hide_libraries) {
                for (const auto& [name, value] : values) {
                    if (list.Size() >= LIMIT)
                        break;
                    if (!hide_libraries || this->library_names.count(name) == 0)
                        list.Push(this->Describe(name, value, prefix.empty() ? name : prefix + "." + name));
                }
            };
            switch (handle.kind) {
                case Handle::Kind::Locals: {
                    const auto& frame = this->frames[handle.frame];
                    if (frame.self && !frame.self.Is<Undefined>())
                        list.Push(this->Describe("this", frame.self, "this"));
                    auto locals = LocalScopes(frame);
                    if (!locals.empty())
                        members(locals.back().As<Namespace>().variables, "", false);
                    break;
                }
                case Handle::Kind::Closure: {
                    auto locals = LocalScopes(this->frames[handle.frame]);
                    std::set<std::string> shown;
                    for (const auto& [name, value] : locals.back().As<Namespace>().variables)
                        shown.insert(name);
                    for (std::size_t i = locals.size() - 1; i-- > 0;) {
                        for (const auto& [name, value] : locals[i].As<Namespace>().variables) {
                            if (shown.insert(name).second)
                                list.Push(this->Describe(name, value, name));
                        }
                    }
                    break;
                }
                case Handle::Kind::Globals:
                    members(this->state->GetGlobalNamespace().As<Namespace>().variables, "", true);
                    break;
                case Handle::Kind::Value: {
                    const Variable& v = handle.value;
                    if (v.Is<Array>()) {
                        const Array& array = v.As<Array>();
                        for (std::size_t i = 0; i < array.indexed.size() && list.Size() < LIMIT; i++)
                            list.Push(this->Describe("[" + std::to_string(i) + "]", array.indexed[i]));
                        members(array.named, "", false);
                    }
                    else if (v.Is<ClassObject>()) {
                        const ClassObject& instance = v.As<ClassObject>();
                        members(instance.members, "", false);
                        if (instance.base)
                            list.Push(this->Describe("[[class]]", instance.base));
                    }
                    else if (v.Is<Class>()) {
                        const Class& cls = v.As<Class>();
                        members(cls.statics, "", false);
                        for (const auto& [name, value] : cls.members) {
                            if (list.Size() < LIMIT)
                                list.Push(this->Describe(name + " (instance)", value));
                        }
                        for (std::size_t i = 0; i < cls.inherits.size(); i++)
                            list.Push(this->Describe("[[inherits " + std::to_string(i) + "]]", cls.inherits[i]));
                    }
                    else if (v.Is<Namespace>()) {
                        members(v.As<Namespace>().variables, "", false);
                    }
                    break;
                }
            }
            return body;
        }

        Json SetVariable(const Json& args) {
            this->RequirePaused();
            std::int64_t reference = args["variablesReference"].GetInt();
            if (reference < 1 || static_cast<std::size_t>(reference) > this->handles.size())
                throw std::runtime_error("invalid reference");
            Handle handle = this->handles[static_cast<std::size_t>(reference - 1)];
            const std::string& name = args["name"].GetString();
            Variable value = Copy(*this->memory, this->Evaluate(args["value"].GetString()));
            MemberList * members = nullptr;
            switch (handle.kind) {
                case Handle::Kind::Locals: {
                    auto locals = LocalScopes(this->frames[handle.frame]);
                    if (!locals.empty())
                        members = &locals.back().As<Namespace>().variables;
                    break;
                }
                case Handle::Kind::Globals:
                    members = &this->state->GetGlobalNamespace().As<Namespace>().variables;
                    break;
                case Handle::Kind::Value:
                    if (handle.value.Is<Array>()) {
                        Array& array = handle.value.As<Array>();
                        if (name.size() > 2 && name.front() == '[' && name.back() == ']') {
                            std::size_t i = std::stoul(name.substr(1, name.size() - 2));
                            if (i < array.indexed.size())
                                array.indexed[i] = value;
                        }
                        else {
                            members = &array.named;
                        }
                    }
                    else if (handle.value.Is<ClassObject>())
                        members = &handle.value.As<ClassObject>().members;
                    else if (handle.value.Is<Class>())
                        members = &handle.value.As<Class>().statics;
                    else if (handle.value.Is<Namespace>())
                        members = &handle.value.As<Namespace>().variables;
                    break;
                default:
                    throw std::runtime_error("this variable can't be modified");
            }
            if (members != nullptr) {
                if (!members->IndexOf(name))
                    throw std::runtime_error("the variable doesn't exist");
                members->Set(name, value);
            }
            Json body;
            body["value"] = this->Format(value);
            body["variablesReference"] = Expandable(value) ? this->AddHandle({ Handle::Kind::Value, value, 0 }) : 0;
            return body;
        }

        ////// EVALUATION //////

        // Evaluate an expression (or instructions for the console) at the current position
        Variable Evaluate(const std::string& expression, bool allow_statements = false) {
            if (!this->state || !this->started)
                throw std::runtime_error("the program isn't started");
            Script::Result compiled = Script::Compile("(" + expression + "\n);");
            if (compiled) {
                // Without the final "value.pop", the value stays on the stack
                compiled.instructions.pop_back();
            }
            else {
                if (!allow_statements)
                    throw std::runtime_error(std::string(compiled.GetMessage()));
                compiled = Script::Compile(expression);
                if (!compiled)
                    throw std::runtime_error(std::string(compiled.GetMessage()) + " (column " + std::to_string(compiled.column + 1) + ")");
            }
            return this->state->Evaluate(std::move(compiled.instructions));
        }

        // Read a path ("a.b[0]") in the variables of a frame (the outer frames can't execute code)
        std::optional<Variable> ReadPath(const State::FrameInfo& frame, const std::string& path) {
            std::size_t i = 0;
            auto name = [&]() {
                std::size_t start = i;
                while (i < path.size() && (std::isalnum(static_cast<unsigned char>(path[i])) || path[i] == '_' || path[i] == '$'))
                    i++;
                return path.substr(start, i - start);
            };
            std::string first = name();
            if (first.empty())
                return std::nullopt;
            Variable v = first == "this" ? frame.self : frame.namer.Find(first);
            if (!v)
                return std::nullopt;
            while (i < path.size()) {
                if (path[i] == '.') {
                    i++;
                    std::string member = name();
                    if (member.empty())
                        return std::nullopt;
                    v = GetMember(*this->memory, v, member);
                }
                else if (path[i] == '[') {
                    std::size_t end = path.find(']', i);
                    if (end == std::string::npos || !v.Is<Array>())
                        return std::nullopt;
                    std::string key = path.substr(i + 1, end - i - 1);
                    i = end + 1;
                    const Array& array = v.As<Array>();
                    if (!key.empty() && std::all_of(key.begin(), key.end(), [](char c) { return c >= '0' && c <= '9'; })) {
                        std::size_t index = std::stoul(key);
                        v = index < array.indexed.size() ? array.indexed[index] : this->memory->CreateUndefined();
                    }
                    else if (key.size() >= 2 && (key.front() == '"' || key.front() == '\'') && key.back() == key.front()) {
                        Variable found = array.named.Find(key.substr(1, key.size() - 2));
                        v = found ? found : this->memory->CreateUndefined();
                    }
                    else {
                        return std::nullopt;
                    }
                }
                else {
                    return std::nullopt;
                }
            }
            return v;
        }

        Json EvaluateRequest(const Json& args) {
            const std::string& expression = args["expression"].GetString();
            const std::string& context = args["context"].GetString();
            Variable value;
            bool outer = this->paused && args.Has("frameId") && static_cast<std::size_t>(args["frameId"].GetInt()) != this->frames.size();
            if (outer || context == "hover") {
                // Hovers and outer frames: only the variables (no execution)
                auto found = this->paused ? this->ReadPath(this->FrameOf(args["frameId"]), expression) : std::nullopt;
                if (!found) {
                    if (outer || context == "hover")
                        throw std::runtime_error(outer ? "only the variables can be read in this frame" : "not available");
                }
                value = *found;
            }
            else {
                value = this->Evaluate(expression, context == "repl");
            }
            Json body;
            body["result"] = this->Format(value);
            body["type"] = std::string(GetTypeName(value ? value.GetType() : Type::Undefined));
            body["variablesReference"] = Expandable(value) && this->paused ? this->AddHandle({ Handle::Kind::Value, value, 0 }) : 0;
            return body;
        }

    };

    void OutputEvents::Flush() {
        if (this->buffer.empty())
            return;
        std::string text = std::move(this->buffer);
        this->buffer.clear();
        this->adapter.Output(this->category, text);
    }

}

int Tools::RunDebugAdapter() {
    SetBinaryStandardStreams();
    Transport transport(std::cin.rdbuf(), std::cout.rdbuf());
    DebugAdapter adapter(transport);
    // The outputs of the scripts become events, the standard input is reserved to the protocol
    EmptyInput empty;
    std::streambuf * saved_out = std::cout.rdbuf(&adapter.stdout_events);
    std::streambuf * saved_err = std::cerr.rdbuf(&adapter.stderr_events);
    std::streambuf * saved_in = std::cin.rdbuf(&empty);
    int code = adapter.Run();
    adapter.stdout_events.Flush();
    adapter.stderr_events.Flush();
    std::cout.rdbuf(saved_out);
    std::cerr.rdbuf(saved_err);
    std::cin.rdbuf(saved_in);
    return code;
}
