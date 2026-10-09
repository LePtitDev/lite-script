// C interface of LiteScript used by the .NET wrapper (LiteScript.Net)
//
// Conventions:
// - The handles of variables are LiteScript::Variable objects allocated by the library ("new"),
//   a function that returns a variable gives a new handle that must be released with ls_var_delete
//   (nullptr for an empty variable). The variables given as arguments are borrowed.
// - The functions that can fail return a status (LS_OK...), the details of the error are available
//   with ls_error_* until the next failure of the thread.
// - The texts are UTF-8. The texts and the buffers returned by the library are allocated with
//   malloc and must be released with ls_free.
// - The lists (instructions, analysis, frames...) are exchanged as little endian binary buffers.

#include <litescript/litescript.hpp>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#if defined(_WIN32)
#define LS_API extern "C" __declspec(dllexport)
#else
#define LS_API extern "C" __attribute__((visibility("default")))
#endif

using namespace LiteScript;

// A number exchanged with the host
struct LsNumber {
    std::int64_t integer;
    double floating;
    std::int32_t is_integer;
};

namespace {

    ////// STATUS AND ERRORS //////

    enum Status : std::int32_t {
        LS_OK = 0,
        // A ScriptError (value, message and position available)
        LS_SCRIPT_ERROR = 1,
        // A SerializationError
        LS_SERIALIZATION_ERROR = 2,
        // Another exception of the engine
        LS_RUNTIME_ERROR = 3,
        // An invalid argument (wrong type of value, index out of range...)
        LS_INVALID_ARGUMENT = 4,
        // An exception of the host (a hook of the application), kept by the host
        LS_HOST_ERROR = 5
    };

    // An exception raised by a function of the host that must reach the caller of the host unchanged
    // (doesn't derive from std::exception so the native functions don't convert it into a script error)
    struct HostError {
        std::string message;
    };

    // An invalid argument given to the C interface
    struct InvalidArgument : std::invalid_argument {
        using std::invalid_argument::invalid_argument;
    };

    // The last error of the thread
    struct LastError {
        std::string message;
        Variable value;
        SourcePosition position;
    };

    thread_local LastError last_error;

    Status Fail(Status status, std::string message, Variable value = Variable(), SourcePosition position = {}) {
        last_error.message = std::move(message);
        last_error.value = std::move(value);
        last_error.position = position;
        return status;
    }

    // Execute a function and convert its exceptions into a status
    template <typename F>
    Status Guard(F&& function) noexcept {
        try {
            function();
            return LS_OK;
        }
        catch (const ScriptError& e) {
            return Fail(LS_SCRIPT_ERROR, e.what(), e.GetValue(), e.GetPosition());
        }
        catch (const SerializationError& e) {
            return Fail(LS_SERIALIZATION_ERROR, e.what());
        }
        catch (const InvalidArgument& e) {
            return Fail(LS_INVALID_ARGUMENT, e.what());
        }
        catch (const HostError& e) {
            return Fail(LS_HOST_ERROR, e.message);
        }
        catch (const std::exception& e) {
            return Fail(LS_RUNTIME_ERROR, e.what());
        }
        catch (...) {
            return Fail(LS_RUNTIME_ERROR, "unknown error");
        }
    }

    ////// HOST CALLBACKS //////

    // Call of a function of the host: fills "result" (a new handle or nullptr) and returns a status,
    // LS_SCRIPT_ERROR, LS_RUNTIME_ERROR and LS_HOST_ERROR take the error given by ls_host_set_error
    using HostInvoke = std::int32_t (*)(std::intptr_t function, State * state, const Variable * const * args, std::size_t count, Variable ** result);

    // Release of a function of the host (GC handle)
    using HostRelease = void (*)(std::intptr_t function);

    // Call of a debug hook of the host
    using HostDebugHook = std::int32_t (*)(std::intptr_t hook, State * state);

    // Call of an error hook of the host
    using HostErrorHook = std::int32_t (*)(std::intptr_t hook, State * state, const Variable * value, const char * message, std::uint32_t line, std::uint32_t column);

    HostInvoke host_invoke = nullptr;
    HostRelease host_release = nullptr;
    HostDebugHook host_debug_hook = nullptr;
    HostErrorHook host_error_hook = nullptr;

    // The error given by the host before returning a failure
    struct HostErrorInfo {
        std::string message;
        Variable value;
        SourcePosition position;
    };

    thread_local HostErrorInfo host_error;

    // Raise the error given by the host
    [[noreturn]] void RaiseHostError(std::int32_t status) {
        HostErrorInfo error = std::move(host_error);
        host_error = HostErrorInfo();
        switch (status) {
            case LS_SCRIPT_ERROR:
                throw ScriptError(std::move(error.value), error.message, error.position);
            case LS_HOST_ERROR:
                throw HostError{ std::move(error.message) };
            default:
                throw std::runtime_error(error.message);
        }
    }

    // A reference to an object of the host, released with the last copy
    class HostHandle {
        std::intptr_t handle;
    public:
        explicit HostHandle(std::intptr_t h) noexcept : handle(h) {}
        HostHandle(const HostHandle&) = delete;
        HostHandle& operator=(const HostHandle&) = delete;
        ~HostHandle() {
            if (host_release != nullptr)
                host_release(this->handle);
        }
        std::intptr_t Get() const noexcept { return this->handle; }
    };

    NativeFunction::Signature MakeHostFunction(std::intptr_t function) {
        auto handle = std::make_shared<HostHandle>(function);
        return [handle](State& state, std::vector<Variable>& args) -> Variable {
            std::vector<const Variable *> pointers;
            pointers.reserve(args.size());
            for (const Variable& v : args)
                pointers.push_back(&v);
            Variable * result = nullptr;
            std::int32_t status = host_invoke(handle->Get(), &state, pointers.data(), pointers.size(), &result);
            std::unique_ptr<Variable> owned(result);
            if (status != LS_OK)
                RaiseHostError(status);
            return owned ? std::move(*owned) : Variable();
        };
    }

    ////// VALUES //////

    Variable * NewHandle(Variable v) {
        if (!v)
            return nullptr;
        return new Variable(std::move(v));
    }

    Variable Borrow(const Variable * v) {
        return v != nullptr ? *v : Variable();
    }

    template <typename T>
    T& Expect(const Variable * v, const char * type_name) {
        if (v == nullptr || !v->Is<T>())
            throw InvalidArgument(std::string("the value isn't ") + type_name);
        return v->As<T>();
    }

    char * CopyText(std::string_view text) {
        char * buffer = static_cast<char *>(std::malloc(text.size() + 1));
        if (buffer == nullptr)
            throw std::bad_alloc();
        std::memcpy(buffer, text.data(), text.size());
        buffer[text.size()] = '\0';
        return buffer;
    }

    Number FromHost(const LsNumber * n) {
        if (n == nullptr)
            return Number();
        return n->is_integer ? Number(n->integer) : Number(n->floating);
    }

    void ToHost(const Number& n, LsNumber * out) {
        if (out == nullptr)
            return;
        out->is_integer = n.IsInteger() ? 1 : 0;
        out->integer = n.ToInteger();
        out->floating = n.ToFloat();
    }

    ////// BINARY BUFFERS //////

    constexpr std::uint64_t NONE = std::numeric_limits<std::uint64_t>::max();

    class Writer {
        std::string data;
    public:
        void U8(std::uint8_t v) { this->data.push_back(static_cast<char>(v)); }
        void U32(std::uint32_t v) { for (int i = 0; i < 4; i++) this->U8(static_cast<std::uint8_t>(v >> (i * 8))); }
        void U64(std::uint64_t v) { for (int i = 0; i < 8; i++) this->U8(static_cast<std::uint8_t>(v >> (i * 8))); }
        void I64(std::int64_t v) { this->U64(static_cast<std::uint64_t>(v)); }
        void F64(double v) { std::uint64_t bits; std::memcpy(&bits, &v, sizeof(bits)); this->U64(bits); }
        void Size(std::size_t v) { this->U64(v == static_cast<std::size_t>(-1) ? NONE : static_cast<std::uint64_t>(v)); }
        void Str(std::string_view s) { this->U32(static_cast<std::uint32_t>(s.size())); this->data.append(s); }
        // A new handle (owned by the reader)
        void Var(const Variable& v) { this->U64(reinterpret_cast<std::uintptr_t>(NewHandle(v))); }

        void Output(std::uint8_t ** buffer, std::size_t * size) const {
            *size = this->data.size();
            *buffer = static_cast<std::uint8_t *>(std::malloc(this->data.empty() ? 1 : this->data.size()));
            if (*buffer == nullptr)
                throw std::bad_alloc();
            if (!this->data.empty())
                std::memcpy(*buffer, this->data.data(), this->data.size());
        }
    };

    class Reader {
        const std::uint8_t * data;
        std::size_t size;
        std::size_t pos = 0;

        void Need(std::size_t count) {
            if (this->size - this->pos < count)
                throw InvalidArgument("invalid buffer");
        }

    public:
        Reader(const std::uint8_t * d, std::size_t s) : data(d), size(d != nullptr ? s : 0) {}

        std::uint8_t U8() { this->Need(1); return this->data[this->pos++]; }
        std::uint32_t U32() { std::uint32_t v = 0; for (int i = 0; i < 4; i++) v |= static_cast<std::uint32_t>(this->U8()) << (i * 8); return v; }
        std::uint64_t U64() { std::uint64_t v = 0; for (int i = 0; i < 8; i++) v |= static_cast<std::uint64_t>(this->U8()) << (i * 8); return v; }
        std::int64_t I64() { return static_cast<std::int64_t>(this->U64()); }
        double F64() { std::uint64_t bits = this->U64(); double v; std::memcpy(&v, &bits, sizeof(v)); return v; }
        std::string Str() {
            std::uint32_t length = this->U32();
            this->Need(length);
            std::string s(reinterpret_cast<const char *>(this->data + this->pos), length);
            this->pos += length;
            return s;
        }
        // A borrowed handle
        Variable Var() { return Borrow(reinterpret_cast<const Variable *>(static_cast<std::uintptr_t>(this->U64()))); }
    };

    enum OperandKind : std::uint8_t {
        OPERAND_NONE,
        OPERAND_BOOLEAN,
        OPERAND_INTEGER,
        OPERAND_FLOAT,
        OPERAND_STRING
    };

    void WriteInstruction(Writer& w, const Instruction& instr) {
        w.U8(static_cast<std::uint8_t>(instr.code));
        if (instr.HasBoolean()) {
            w.U8(OPERAND_BOOLEAN);
            w.U8(instr.GetBoolean() ? 1 : 0);
        }
        else if (instr.HasInteger()) {
            w.U8(OPERAND_INTEGER);
            w.I64(instr.GetInteger());
        }
        else if (instr.HasFloat()) {
            w.U8(OPERAND_FLOAT);
            w.F64(instr.GetFloat());
        }
        else if (instr.HasString()) {
            w.U8(OPERAND_STRING);
            w.Str(instr.GetString());
        }
        else {
            w.U8(OPERAND_NONE);
        }
        w.U32(instr.position.line);
        w.U32(instr.position.column);
    }

    Instruction ReadInstruction(Reader& r) {
        std::uint8_t code = r.U8();
        if (code >= INSTRUCTION_COUNT)
            throw InvalidArgument("invalid instruction code");
        Instruction instr(static_cast<InstrCode>(code));
        switch (r.U8()) {
            case OPERAND_NONE: break;
            case OPERAND_BOOLEAN: instr.operand = r.U8() != 0; break;
            case OPERAND_INTEGER: instr.operand = r.I64(); break;
            case OPERAND_FLOAT: instr.operand = r.F64(); break;
            case OPERAND_STRING: instr.operand = r.Str(); break;
            default: throw InvalidArgument("invalid instruction operand");
        }
        instr.position.line = r.U32();
        instr.position.column = r.U32();
        return instr;
    }

    void WriteInstructions(Writer& w, const std::vector<Instruction>& instructions) {
        w.U32(static_cast<std::uint32_t>(instructions.size()));
        for (const Instruction& instr : instructions)
            WriteInstruction(w, instr);
    }

    std::vector<Instruction> ReadInstructions(Reader& r) {
        std::uint32_t count = r.U32();
        std::vector<Instruction> instructions;
        instructions.reserve(count);
        for (std::uint32_t i = 0; i < count; i++)
            instructions.push_back(ReadInstruction(r));
        return instructions;
    }

    std::vector<Instruction> ReadInstructions(const std::uint8_t * buffer, std::size_t size) {
        Reader r(buffer, size);
        return ReadInstructions(r);
    }

    void WriteNamer(Writer& w, const Namer& namer) {
        w.Var(namer.global);
        w.Var(namer.current);
        w.U32(static_cast<std::uint32_t>(namer.scopes.size()));
        for (const Variable& scope : namer.scopes)
            w.Var(scope);
    }

    Namer ReadNamer(Reader& r) {
        Namer namer;
        namer.global = r.Var();
        namer.current = r.Var();
        std::uint32_t count = r.U32();
        for (std::uint32_t i = 0; i < count; i++)
            namer.scopes.push_back(r.Var());
        return namer;
    }

    void WritePosition(Writer& w, const Script::Analysis::Position& p) {
        w.Size(p.line);
        w.Size(p.column);
        w.Size(p.offset);
    }

    void WriteRange(Writer& w, const Script::Analysis::Range& range) {
        WritePosition(w, range.start);
        WritePosition(w, range.end);
    }

    void WriteAnalysis(Writer& w, const Script::Analysis& analysis) {
        w.U32(static_cast<std::uint32_t>(analysis.scopes.size()));
        for (const auto& scope : analysis.scopes) {
            w.U8(static_cast<std::uint8_t>(scope.kind));
            WriteRange(w, scope.range);
            w.Size(scope.parent);
        }
        w.U32(static_cast<std::uint32_t>(analysis.symbols.size()));
        for (const auto& symbol : analysis.symbols) {
            w.Str(symbol.name);
            w.U8(static_cast<std::uint8_t>(symbol.kind));
            w.U8(symbol.is_static ? 1 : 0);
            WriteRange(w, symbol.range);
            WriteRange(w, symbol.full);
            w.Size(symbol.scope);
            w.Size(symbol.body);
            w.Str(symbol.nspace);
            w.U32(static_cast<std::uint32_t>(symbol.parameters.size()));
            for (const std::string& parameter : symbol.parameters)
                w.Str(parameter);
            w.Size(symbol.first_instruction);
            w.Size(symbol.end_instruction);
        }
        w.U32(static_cast<std::uint32_t>(analysis.references.size()));
        for (const auto& reference : analysis.references) {
            w.Str(reference.name);
            WriteRange(w, reference.range);
            w.Size(reference.scope);
            w.U8(reference.member ? 1 : 0);
            w.Str(reference.nspace);
        }
        w.U32(static_cast<std::uint32_t>(analysis.statements.size()));
        for (const auto& statement : analysis.statements) {
            WritePosition(w, statement.position);
            w.Size(statement.instruction);
        }
    }

    // Buffer of a compilation: error, line, column, instructions
    template <typename R>
    void WriteResult(Writer& w, const R& result, std::size_t column) {
        w.U8(static_cast<std::uint8_t>(result.error));
        w.Size(result.line);
        w.Size(column);
        WriteInstructions(w, result.instructions);
    }

    ////// MEMBER LISTS AND VECTORS //////

    // The lists of named variables of the values
    enum MemberKind : std::int32_t {
        MEMBERS_ARRAY_NAMED,
        MEMBERS_CLASS_STATICS,
        MEMBERS_CLASS_MEMBERS,
        MEMBERS_INSTANCE_MEMBERS,
        MEMBERS_NAMESPACE_VARIABLES
    };

    MemberList& GetMembers(const Variable * v, std::int32_t kind) {
        switch (kind) {
            case MEMBERS_ARRAY_NAMED: return Expect<Array>(v, "an array").named;
            case MEMBERS_CLASS_STATICS: return Expect<Class>(v, "a class").statics;
            case MEMBERS_CLASS_MEMBERS: return Expect<Class>(v, "a class").members;
            case MEMBERS_INSTANCE_MEMBERS: return Expect<ClassObject>(v, "an instance of class").members;
            case MEMBERS_NAMESPACE_VARIABLES: return Expect<Namespace>(v, "a namespace").variables;
            default: throw InvalidArgument("invalid member list");
        }
    }

    // The lists of variables of the values
    enum VectorKind : std::int32_t {
        VECTOR_ARRAY_INDEXED,
        VECTOR_CLASS_INHERITS
    };

    std::vector<Variable>& GetVector(const Variable * v, std::int32_t kind) {
        switch (kind) {
            case VECTOR_ARRAY_INDEXED: return Expect<Array>(v, "an array").indexed;
            case VECTOR_CLASS_INHERITS: return Expect<Class>(v, "a class").inherits;
            default: throw InvalidArgument("invalid list");
        }
    }

    void CheckIndex(std::size_t index, std::size_t count) {
        if (index >= count)
            throw InvalidArgument("index out of range");
    }

    Operator CheckOperator(std::int32_t op) {
        if (op < 0 || static_cast<std::size_t>(op) >= OPERATOR_COUNT)
            throw InvalidArgument("invalid operator");
        return static_cast<Operator>(op);
    }

    Type CheckType(std::int32_t type) {
        if (type < 0 || static_cast<std::size_t>(type) >= TYPE_COUNT)
            throw InvalidArgument("invalid type");
        return static_cast<Type>(type);
    }

    std::vector<Variable> BorrowAll(const Variable * const * args, std::size_t count) {
        std::vector<Variable> values;
        values.reserve(count);
        for (std::size_t i = 0; i < count; i++)
            values.push_back(Borrow(args[i]));
        return values;
    }

}

////// GENERAL //////

LS_API void ls_free(void * pointer) {
    std::free(pointer);
}

LS_API void ls_host_init(HostInvoke invoke, HostRelease release, HostDebugHook debug_hook, HostErrorHook error_hook) {
    host_invoke = invoke;
    host_release = release;
    host_debug_hook = debug_hook;
    host_error_hook = error_hook;
}

// Give the error of a failed host callback (value: borrowed, may be null)
LS_API void ls_host_set_error(const char * message, const Variable * value, std::uint32_t line, std::uint32_t column) {
    host_error.message = message != nullptr ? message : "";
    host_error.value = Borrow(value);
    host_error.position = SourcePosition{ line, column };
}

LS_API const char * ls_error_message() {
    return last_error.message.c_str();
}

LS_API Variable * ls_error_value() {
    return NewHandle(last_error.value);
}

LS_API void ls_error_position(std::uint32_t * line, std::uint32_t * column) {
    *line = last_error.position.line;
    *column = last_error.position.column;
}

LS_API void ls_error_clear() {
    last_error = LastError();
}

LS_API void ls_release_cached_blocks() {
    Object::ReleaseCachedBlocks();
}

LS_API std::int32_t ls_type_name(std::int32_t type, char ** name) {
    return Guard([&] { *name = CopyText(GetTypeName(CheckType(type))); });
}

////// NUMBERS //////

LS_API void ls_number_from_double(double value, LsNumber * out) {
    ToHost(Number(value), out);
}

LS_API std::int32_t ls_number_to_string(const LsNumber * n, char ** text) {
    return Guard([&] { *text = CopyText(FromHost(n).ToString()); });
}

// Unary operations: UnaryMinus, PreIncrement, PreDecrement, BitwiseNot
LS_API std::int32_t ls_number_unary(std::int32_t op, const LsNumber * a, LsNumber * out) {
    return Guard([&] {
        Number n = FromHost(a);
        switch (CheckOperator(op)) {
            case Operator::UnaryPlus: break;
            case Operator::UnaryMinus: n = -n; break;
            case Operator::PreIncrement: ++n; break;
            case Operator::PreDecrement: --n; break;
            case Operator::BitwiseNot: n = ~n; break;
            default: throw InvalidArgument("invalid unary operator");
        }
        ToHost(n, out);
    });
}

LS_API std::int32_t ls_number_binary(std::int32_t op, const LsNumber * a, const LsNumber * b, LsNumber * out) {
    return Guard([&] {
        Number x = FromHost(a), y = FromHost(b), r;
        switch (CheckOperator(op)) {
            case Operator::Add: r = x + y; break;
            case Operator::Subtract: r = x - y; break;
            case Operator::Multiply: r = x * y; break;
            case Operator::Divide: r = x / y; break;
            case Operator::Modulo: r = x % y; break;
            case Operator::BitwiseAnd: r = x & y; break;
            case Operator::BitwiseOr: r = x | y; break;
            case Operator::BitwiseXor: r = x ^ y; break;
            case Operator::LeftShift: r = x << y; break;
            case Operator::RightShift: r = x >> y; break;
            default: throw InvalidArgument("invalid binary operator");
        }
        ToHost(r, out);
    });
}

// Return 1 if a == b (op Equal) or a < b (op Less), 0 otherwise
LS_API std::int32_t ls_number_compare(std::int32_t op, const LsNumber * a, const LsNumber * b) {
    Number x = FromHost(a), y = FromHost(b);
    if (op == static_cast<std::int32_t>(Operator::Equal))
        return x == y ? 1 : 0;
    return x < y ? 1 : 0;
}

////// MEMORY //////

LS_API Memory * ls_memory_new(std::uint64_t gc_threshold) {
    try {
        return new Memory(static_cast<std::size_t>(gc_threshold));
    }
    catch (...) {
        return nullptr;
    }
}

LS_API void ls_memory_delete(Memory * memory) {
    delete memory;
}

LS_API std::uint64_t ls_memory_count(const Memory * memory) {
    return memory->Count();
}

LS_API std::int32_t ls_memory_collect_garbage(Memory * memory, std::int32_t if_needed, std::uint64_t * count) {
    return Guard([&] { *count = if_needed ? memory->CollectGarbageIfNeeded() : memory->CollectGarbage(); });
}

LS_API void ls_memory_set_gc_threshold(Memory * memory, std::uint64_t threshold) {
    memory->SetGarbageCollectorThreshold(static_cast<std::size_t>(threshold));
}

LS_API std::int32_t ls_memory_create_undefined(Memory * memory, Variable ** out) {
    return Guard([&] { *out = NewHandle(memory->CreateUndefined()); });
}

LS_API std::int32_t ls_memory_create_null(Memory * memory, Variable ** out) {
    return Guard([&] { *out = NewHandle(memory->CreateNull()); });
}

LS_API std::int32_t ls_memory_create_boolean(Memory * memory, std::int32_t value, Variable ** out) {
    return Guard([&] { *out = NewHandle(memory->CreateBoolean(value != 0)); });
}

LS_API std::int32_t ls_memory_create_number(Memory * memory, const LsNumber * value, Variable ** out) {
    return Guard([&] { *out = NewHandle(memory->CreateNumber(FromHost(value))); });
}

LS_API std::int32_t ls_memory_create_string(Memory * memory, const char * utf8, std::size_t length, Variable ** out) {
    return Guard([&] { *out = NewHandle(memory->CreateString(std::string_view(utf8 != nullptr ? utf8 : "", length))); });
}

LS_API std::int32_t ls_memory_create_character(Memory * memory, const Variable * string, std::uint64_t index, Variable ** out) {
    return Guard([&] {
        Character c;
        c.string = Borrow(string);
        c.index = static_cast<std::size_t>(index);
        *out = NewHandle(memory->Create(std::move(c)));
    });
}

LS_API std::int32_t ls_memory_create_namespace(Memory * memory, Variable ** out) {
    return Guard([&] { *out = NewHandle(memory->CreateNamespace()); });
}

LS_API std::int32_t ls_memory_create_class(Memory * memory, Variable ** out) {
    return Guard([&] { *out = NewHandle(memory->Create(Class{})); });
}

LS_API std::int32_t ls_memory_create_instance(Memory * memory, const Variable * base, Variable ** out) {
    return Guard([&] {
        ClassObject instance;
        instance.base = Borrow(base);
        *out = NewHandle(memory->Create(std::move(instance)));
    });
}

// Create an array with copies of the values (names: nullptr for an indexed array)
LS_API std::int32_t ls_memory_create_array(Memory * memory, const Variable * const * values, const char * const * names, std::size_t count, Variable ** out) {
    return Guard([&] {
        Array array;
        for (std::size_t i = 0; i < count; i++) {
            Variable copy = Copy(*memory, Borrow(values[i]));
            if (names != nullptr)
                array.named.Set(names[i] != nullptr ? names[i] : "", std::move(copy));
            else
                array.indexed.push_back(std::move(copy));
        }
        *out = NewHandle(memory->Create(std::move(array)));
    });
}

// Create a callback that calls a function of the host
LS_API std::int32_t ls_memory_create_callback(Memory * memory, std::intptr_t function, Variable ** out) {
    return Guard([&] { *out = NewHandle(memory->CreateCallback(MakeHostFunction(function))); });
}

LS_API std::int32_t ls_memory_create_callback_named(Memory * memory, const char * name, Variable ** out) {
    return Guard([&] { *out = NewHandle(memory->CreateCallback(std::string_view(name))); });
}

LS_API std::int32_t ls_memory_register_function(Memory * memory, const char * name, std::intptr_t function, Variable ** out) {
    return Guard([&] { *out = NewHandle(memory->RegisterFunction(name, MakeHostFunction(function))); });
}

LS_API std::int32_t ls_memory_has_function(const Memory * memory, const char * name) {
    return memory->FindFunction(name) != nullptr ? 1 : 0;
}

LS_API std::int32_t ls_memory_set_methods(Memory * memory, std::int32_t type, const Variable * methods) {
    return Guard([&] { memory->SetMethods(CheckType(type), Borrow(methods)); });
}

LS_API std::int32_t ls_memory_find_method(const Memory * memory, std::int32_t type, const char * name, Variable ** out) {
    return Guard([&] { *out = NewHandle(memory->FindMethod(CheckType(type), name)); });
}

////// VARIABLES //////

LS_API void ls_var_delete(Variable * v) {
    delete v;
}

LS_API Variable * ls_var_duplicate(const Variable * v) {
    try {
        return NewHandle(Borrow(v));
    }
    catch (...) {
        return nullptr;
    }
}

LS_API std::int32_t ls_var_type(const Variable * v) {
    return static_cast<std::int32_t>(v != nullptr ? v->GetType() : Type::Undefined);
}

LS_API std::int32_t ls_var_same(const Variable * a, const Variable * b) {
    return Borrow(a) == Borrow(b) ? 1 : 0;
}

// An identifier of the referenced object (0 for an empty variable)
LS_API std::uint64_t ls_var_object_id(const Variable * v) {
    return v != nullptr ? reinterpret_cast<std::uintptr_t>(v->Get()) : 0;
}

LS_API std::uint64_t ls_var_reference_count(const Variable * v) {
    return v != nullptr && *v ? (*v)->GetReferenceCount() : 0;
}

LS_API std::int32_t ls_var_is_reference(const Variable * v) {
    return v != nullptr && *v && (*v)->IsReference() ? 1 : 0;
}

LS_API Variable * ls_var_target(const Variable * v) {
    try {
        if (v == nullptr || !*v)
            return nullptr;
        return new Variable((*v)->Target());
    }
    catch (...) {
        return nullptr;
    }
}

LS_API std::int32_t ls_var_to_string(const Variable * v, char ** text) {
    return Guard([&] { *text = CopyText(ToString(Borrow(v))); });
}

LS_API std::int32_t ls_var_to_text(const Variable * v, char ** text) {
    return Guard([&] { *text = CopyText(ToText(Borrow(v)).ToUTF8()); });
}

LS_API std::int32_t ls_var_get_boolean(const Variable * v, std::int32_t * value) {
    return Guard([&] { *value = Expect<bool>(v, "a boolean") ? 1 : 0; });
}

LS_API std::int32_t ls_var_get_number(const Variable * v, LsNumber * value) {
    return Guard([&] { ToHost(Expect<Number>(v, "a number"), value); });
}

LS_API std::int32_t ls_var_get_string(const Variable * v, char ** text) {
    return Guard([&] { *text = CopyText(Expect<String>(v, "a string").ToUTF8()); });
}

LS_API std::int32_t ls_var_get_string_length(const Variable * v, std::uint64_t * length) {
    return Guard([&] { *length = Expect<String>(v, "a string").GetLength(); });
}

// Replace the content of the object (0 undefined, 1 null, 2 boolean, 3 number, 4 string)
LS_API std::int32_t ls_var_set(const Variable * v, std::int32_t kind, std::int32_t boolean, const LsNumber * number, const char * utf8, std::size_t length) {
    return Guard([&] {
        if (v == nullptr || !*v)
            throw InvalidArgument("the variable is empty");
        Data& data = (*v)->data;
        switch (kind) {
            case 0: data = Undefined{}; break;
            case 1: data = Null{}; break;
            case 2: data = boolean != 0; break;
            case 3: data = FromHost(number); break;
            case 4: data = String(std::string_view(utf8 != nullptr ? utf8 : "", length)); break;
            default: throw InvalidArgument("invalid kind of value");
        }
    });
}

////// LISTS OF VARIABLES //////

LS_API std::int32_t ls_vector_count(const Variable * v, std::int32_t kind, std::uint64_t * count) {
    return Guard([&] { *count = GetVector(v, kind).size(); });
}

LS_API std::int32_t ls_vector_get(const Variable * v, std::int32_t kind, std::uint64_t index, Variable ** out) {
    return Guard([&] {
        auto& vector = GetVector(v, kind);
        CheckIndex(static_cast<std::size_t>(index), vector.size());
        *out = NewHandle(vector[static_cast<std::size_t>(index)]);
    });
}

LS_API std::int32_t ls_vector_set(const Variable * v, std::int32_t kind, std::uint64_t index, const Variable * value) {
    return Guard([&] {
        auto& vector = GetVector(v, kind);
        CheckIndex(static_cast<std::size_t>(index), vector.size());
        vector[static_cast<std::size_t>(index)] = Borrow(value);
    });
}

LS_API std::int32_t ls_vector_insert(const Variable * v, std::int32_t kind, std::uint64_t index, const Variable * value) {
    return Guard([&] {
        auto& vector = GetVector(v, kind);
        CheckIndex(static_cast<std::size_t>(index), vector.size() + 1);
        vector.insert(vector.begin() + static_cast<std::ptrdiff_t>(index), Borrow(value));
    });
}

LS_API std::int32_t ls_vector_remove(const Variable * v, std::int32_t kind, std::uint64_t index) {
    return Guard([&] {
        auto& vector = GetVector(v, kind);
        CheckIndex(static_cast<std::size_t>(index), vector.size());
        Variable removed = std::move(vector[static_cast<std::size_t>(index)]);
        vector.erase(vector.begin() + static_cast<std::ptrdiff_t>(index));
    });
}

LS_API std::int32_t ls_vector_clear(const Variable * v, std::int32_t kind) {
    return Guard([&] {
        std::vector<Variable> removed = std::move(GetVector(v, kind));
        GetVector(v, kind).clear();
    });
}

LS_API std::int32_t ls_members_count(const Variable * v, std::int32_t kind, std::uint64_t * count) {
    return Guard([&] { *count = GetMembers(v, kind).Count(); });
}

LS_API std::int32_t ls_members_name(const Variable * v, std::int32_t kind, std::uint64_t index, char ** name) {
    return Guard([&] {
        const MemberList& members = GetMembers(v, kind);
        CheckIndex(static_cast<std::size_t>(index), members.Count());
        *name = CopyText(members.GetName(static_cast<std::size_t>(index)));
    });
}

LS_API std::int32_t ls_members_value(const Variable * v, std::int32_t kind, std::uint64_t index, Variable ** out) {
    return Guard([&] {
        const MemberList& members = GetMembers(v, kind);
        CheckIndex(static_cast<std::size_t>(index), members.Count());
        *out = NewHandle(members.GetValue(static_cast<std::size_t>(index)));
    });
}

LS_API std::int32_t ls_members_index_of(const Variable * v, std::int32_t kind, const char * name, std::int64_t * index) {
    return Guard([&] {
        auto found = GetMembers(v, kind).IndexOf(name);
        *index = found ? static_cast<std::int64_t>(*found) : -1;
    });
}

LS_API std::int32_t ls_members_find(const Variable * v, std::int32_t kind, const char * name, Variable ** out) {
    return Guard([&] { *out = NewHandle(GetMembers(v, kind).Find(name)); });
}

LS_API std::int32_t ls_members_set(const Variable * v, std::int32_t kind, const char * name, const Variable * value) {
    return Guard([&] { GetMembers(v, kind).Set(name, Borrow(value)); });
}

////// CLASSES, INSTANCES, CALLBACKS AND CHARACTERS //////

LS_API std::int32_t ls_class_get_operator(const Variable * v, std::int32_t op, Variable ** out) {
    return Guard([&] { *out = NewHandle(Expect<Class>(v, "a class").GetOperator(CheckOperator(op))); });
}

LS_API std::int32_t ls_class_set_operator(const Variable * v, std::int32_t op, const Variable * callback) {
    return Guard([&] { Expect<Class>(v, "a class").operators[static_cast<std::size_t>(CheckOperator(op))] = Borrow(callback); });
}

LS_API std::int32_t ls_class_get_constructor_name(const Variable * v, char ** name) {
    return Guard([&] { *name = CopyText(Expect<Class>(v, "a class").constructor); });
}

LS_API std::int32_t ls_class_set_constructor_name(const Variable * v, const char * name) {
    return Guard([&] { Expect<Class>(v, "a class").constructor = name != nullptr ? name : ""; });
}

LS_API std::int32_t ls_class_define_constructor(const Variable * v, const char * name, std::int32_t * defined) {
    return Guard([&] { *defined = Expect<Class>(v, "a class").DefineConstructor(name) ? 1 : 0; });
}

LS_API std::int32_t ls_class_get_constructor(const Variable * v, Variable ** out) {
    return Guard([&] { *out = NewHandle(Expect<Class>(v, "a class").GetConstructor()); });
}

LS_API std::int32_t ls_class_inherit(const Variable * v, const Variable * parent) {
    return Guard([&] { Expect<Class>(v, "a class").Inherit(Borrow(parent)); });
}

LS_API std::int32_t ls_instance_get_base(const Variable * v, Variable ** out) {
    return Guard([&] { *out = NewHandle(Expect<ClassObject>(v, "an instance of class").base); });
}

LS_API std::int32_t ls_instance_set_base(const Variable * v, const Variable * base) {
    return Guard([&] { Expect<ClassObject>(v, "an instance of class").base = Borrow(base); });
}

LS_API std::int32_t ls_callback_is_native(const Variable * v, std::int32_t * native) {
    return Guard([&] { *native = Expect<Callback>(v, "a callback").IsNative() ? 1 : 0; });
}

LS_API std::int32_t ls_callback_get_self(const Variable * v, Variable ** out) {
    return Guard([&] { *out = NewHandle(Expect<Callback>(v, "a callback").self); });
}

LS_API std::int32_t ls_callback_set_self(const Variable * v, const Variable * self) {
    return Guard([&] { Expect<Callback>(v, "a callback").self = Borrow(self); });
}

LS_API std::int32_t ls_callback_same_target(const Variable * a, const Variable * b, std::int32_t * same) {
    return Guard([&] { *same = Expect<Callback>(a, "a callback").SameTarget(Expect<Callback>(b, "a callback")) ? 1 : 0; });
}

// The name of a registered native function (empty for the others)
LS_API std::int32_t ls_callback_native_name(const Variable * v, char ** name) {
    return Guard([&] {
        const Callback& callback = Expect<Callback>(v, "a callback");
        *name = CopyText(callback.IsNative() && std::get<1>(callback.target) ? std::get<1>(callback.target)->name : std::string());
    });
}

// The position of a script function (list and line) and its scope (buffer: namer)
LS_API std::int32_t ls_callback_script_function(const Variable * v, std::uint64_t * list, std::uint64_t * line, std::uint8_t ** buffer, std::size_t * size) {
    return Guard([&] {
        const Callback& callback = Expect<Callback>(v, "a callback");
        if (callback.IsNative())
            throw InvalidArgument("the callback isn't a script function");
        const auto& function = std::get<0>(callback.target);
        *list = function.list;
        *line = function.line;
        Writer w;
        WriteNamer(w, function.scope);
        w.Output(buffer, size);
    });
}

LS_API std::int32_t ls_character_get(const Variable * v, Variable ** string, std::uint64_t * index) {
    return Guard([&] {
        const Character& c = Expect<Character>(v, "a character");
        *string = NewHandle(c.string);
        *index = c.index;
    });
}

LS_API std::int32_t ls_character_set(const Variable * v, const Variable * string, std::uint64_t index) {
    return Guard([&] {
        Character& c = Expect<Character>(v, "a character");
        c.string = Borrow(string);
        c.index = static_cast<std::size_t>(index);
    });
}

////// NAMESPACES RESOLUTION //////

// Find a variable with a namer (buffer: namer)
LS_API std::int32_t ls_namer_find(const std::uint8_t * buffer, std::size_t size, const char * name, Variable ** out) {
    return Guard([&] {
        Reader r(buffer, size);
        *out = NewHandle(ReadNamer(r).Find(name));
    });
}

LS_API std::int32_t ls_namer_declare(const std::uint8_t * buffer, std::size_t size, const char * name, const Variable * value) {
    return Guard([&] {
        Reader r(buffer, size);
        ReadNamer(r).Declare(name, Borrow(value));
    });
}

////// OPERATIONS //////

LS_API std::int32_t ls_is_truthy(const Variable * v) {
    try {
        return IsTruthy(Borrow(v)) ? 1 : 0;
    }
    catch (...) {
        return 0;
    }
}

LS_API std::int32_t ls_copy(Memory * memory, const Variable * v, Variable ** out) {
    return Guard([&] { *out = NewHandle(Copy(*memory, Borrow(v))); });
}

LS_API std::int32_t ls_clone(Memory * memory, const Variable * v, Variable ** out) {
    return Guard([&] { *out = NewHandle(Clone(*memory, Borrow(v))); });
}

LS_API std::int32_t ls_convert(Memory * memory, const Variable * v, std::int32_t type, Variable ** out) {
    return Guard([&] { *out = NewHandle(Convert(*memory, Borrow(v), CheckType(type))); });
}

// Apply an operator: the result is a value (out) or a call of an overloaded operator
// (callback, self and buffer of the arguments: count + handles)
LS_API std::int32_t ls_operate(Memory * memory, std::int32_t op, const Variable * a, const Variable * b, Variable ** out,
                               Variable ** callback, Variable ** self, std::uint8_t ** args, std::size_t * args_size) {
    return Guard([&] {
        Operator oper = CheckOperator(op);
        if (oper == Operator::Call)
            throw InvalidArgument("the call operator can't be applied by Operate");
        OperationResult result = Operate(*memory, oper, Borrow(a), Borrow(b));
        *out = *callback = *self = nullptr;
        *args = nullptr;
        *args_size = 0;
        if (Variable * value = std::get_if<Variable>(&result)) {
            *out = NewHandle(std::move(*value));
            return;
        }
        OverloadCall& call = std::get<OverloadCall>(result);
        Writer w;
        w.U32(static_cast<std::uint32_t>(call.args.size()));
        for (const Variable& arg : call.args)
            w.Var(arg);
        w.Output(args, args_size);
        *callback = NewHandle(std::move(call.callback));
        *self = NewHandle(std::move(call.self));
    });
}

LS_API std::int32_t ls_get_member(Memory * memory, const Variable * v, const char * name, Variable ** out) {
    return Guard([&] { *out = NewHandle(GetMember(*memory, Borrow(v), name)); });
}

LS_API std::int32_t ls_is_binary(std::int32_t op) {
    if (op < 0 || static_cast<std::size_t>(op) >= OPERATOR_COUNT)
        return 0;
    return IsBinary(static_cast<Operator>(op)) ? 1 : 0;
}

LS_API std::int32_t ls_values_equal(const Variable * a, const Variable * b, std::int32_t * equal) {
    return Guard([&] { *equal = ValuesEqual(Borrow(a), Borrow(b)) ? 1 : 0; });
}

// Result: -1 less, 0 equivalent, 1 greater, 2 unordered
LS_API std::int32_t ls_compare_values(const Variable * a, const Variable * b, std::int32_t * order) {
    return Guard([&] {
        std::partial_ordering result = CompareValues(Borrow(a), Borrow(b));
        if (result == std::partial_ordering::less)
            *order = -1;
        else if (result == std::partial_ordering::equivalent)
            *order = 0;
        else if (result == std::partial_ordering::greater)
            *order = 1;
        else
            *order = 2;
    });
}

////// STATES //////

LS_API State * ls_state_new(Memory * memory) {
    try {
        return new State(*memory);
    }
    catch (...) {
        return nullptr;
    }
}

LS_API void ls_state_delete(State * state) {
    delete state;
}

LS_API std::int32_t ls_state_add_instructions(State * state, const std::uint8_t * buffer, std::size_t size, std::uint64_t * index) {
    return Guard([&] { *index = state->AddInstructions(ReadInstructions(buffer, size)); });
}

LS_API std::int32_t ls_state_add_instruction(State * state, const std::uint8_t * buffer, std::size_t size) {
    return Guard([&] {
        Reader r(buffer, size);
        state->AddInstruction(ReadInstruction(r));
    });
}

LS_API std::uint64_t ls_state_list_count(const State * state) {
    return state->GetInstructionListCount();
}

LS_API std::int32_t ls_state_get_instructions(const State * state, std::uint64_t index, std::uint8_t ** buffer, std::size_t * size) {
    return Guard([&] {
        CheckIndex(static_cast<std::size_t>(index), state->GetInstructionListCount());
        Writer w;
        WriteInstructions(w, state->GetInstructions(static_cast<std::size_t>(index)));
        w.Output(buffer, size);
    });
}

LS_API std::uint64_t ls_state_list_index(const State * state) {
    return state->GetListIndex();
}

LS_API std::uint64_t ls_state_line(const State * state) {
    return state->GetLine();
}

LS_API std::int32_t ls_state_execute(State * state, Variable ** out) {
    return Guard([&] { *out = NewHandle(state->Execute()); });
}

LS_API std::int32_t ls_state_execute_single(State * state, Variable ** out) {
    return Guard([&] { *out = NewHandle(state->ExecuteSingle()); });
}

LS_API std::int32_t ls_state_execute_instruction(State * state, const std::uint8_t * buffer, std::size_t size, Variable ** out) {
    return Guard([&] {
        Reader r(buffer, size);
        Instruction instruction = ReadInstruction(r);
        *out = NewHandle(state->ExecuteInstruction(instruction));
    });
}

LS_API std::int32_t ls_state_finished(const State * state) {
    return state->Finished() ? 1 : 0;
}

LS_API std::int32_t ls_state_call(State * state, const Variable * callee, const Variable * const * args, std::size_t count, Variable ** out) {
    return Guard([&] { *out = NewHandle(state->Call(Borrow(callee), BorrowAll(args, count))); });
}

LS_API void ls_state_set_max_call_depth(State * state, std::uint64_t depth) {
    state->SetMaxCallDepth(static_cast<std::size_t>(depth));
}

LS_API std::int32_t ls_state_get_variable(const State * state, const char * name, Variable ** out) {
    return Guard([&] { *out = NewHandle(state->GetVariable(name)); });
}

LS_API std::int32_t ls_state_declare_variable(State * state, const char * name, const Variable * value) {
    return Guard([&] { state->DeclareVariable(name, Borrow(value)); });
}

LS_API Variable * ls_state_global_namespace(const State * state) {
    try {
        return NewHandle(state->GetGlobalNamespace());
    }
    catch (...) {
        return nullptr;
    }
}

LS_API std::int32_t ls_state_use_namespace(State * state, const char * path) {
    return Guard([&] { state->UseNamespace(path); });
}

LS_API std::int32_t ls_state_get_this(const State * state, Variable ** out) {
    return Guard([&] { *out = NewHandle(state->GetThis()); });
}

LS_API std::uint64_t ls_state_args_count(const State * state) {
    return state->GetArgsCount();
}

LS_API std::int32_t ls_state_get_arg(const State * state, std::uint64_t index, Variable ** out) {
    return Guard([&] { *out = NewHandle(state->GetArg(static_cast<std::size_t>(index))); });
}

// The stack (buffer: count + handles)
LS_API std::int32_t ls_state_get_stack(const State * state, std::uint8_t ** buffer, std::size_t * size) {
    return Guard([&] {
        Writer w;
        const auto& stack = state->GetStack();
        w.U32(static_cast<std::uint32_t>(stack.size()));
        for (const Variable& v : stack)
            w.Var(v);
        w.Output(buffer, size);
    });
}

LS_API std::uint64_t ls_state_depth(const State * state) {
    return state->GetDepth();
}

// The frames (buffer: count + (list, line, namer, self, count + args))
LS_API std::int32_t ls_state_get_frames(const State * state, std::uint8_t ** buffer, std::size_t * size) {
    return Guard([&] {
        Writer w;
        auto frames = state->GetFrames();
        w.U32(static_cast<std::uint32_t>(frames.size()));
        for (const auto& frame : frames) {
            w.U64(frame.list);
            w.U64(frame.line);
            WriteNamer(w, frame.namer);
            w.Var(frame.self);
            w.U32(static_cast<std::uint32_t>(frame.args.size()));
            for (const Variable& arg : frame.args)
                w.Var(arg);
        }
        w.Output(buffer, size);
    });
}

// Define the debug hook (hook: a handle of the host, 0 to remove it)
LS_API std::int32_t ls_state_set_debug_hook(State * state, std::intptr_t hook) {
    return Guard([&] {
        if (hook == 0) {
            state->SetDebugHook(nullptr);
            return;
        }
        auto handle = std::make_shared<HostHandle>(hook);
        state->SetDebugHook([handle](State& s) {
            std::int32_t status = host_debug_hook(handle->Get(), &s);
            if (status != LS_OK)
                RaiseHostError(status);
        });
    });
}

// Define the error hook (hook: a handle of the host, 0 to remove it)
LS_API std::int32_t ls_state_set_error_hook(State * state, std::intptr_t hook) {
    return Guard([&] {
        if (hook == 0) {
            state->SetErrorHook(nullptr);
            return;
        }
        auto handle = std::make_shared<HostHandle>(hook);
        state->SetErrorHook([handle](State& s, const ScriptError& error) {
            const Variable& value = error.GetValue();
            std::int32_t status = host_error_hook(handle->Get(), &s, value ? &value : nullptr, error.what(), error.GetPosition().line, error.GetPosition().column);
            if (status != LS_OK)
                RaiseHostError(status);
        });
    });
}

LS_API std::int32_t ls_state_evaluate(State * state, const std::uint8_t * buffer, std::size_t size, std::uint64_t max_steps, Variable ** out) {
    return Guard([&] { *out = NewHandle(state->Evaluate(ReadInstructions(buffer, size), static_cast<std::size_t>(max_steps))); });
}

LS_API std::int32_t ls_state_save(const State * state, std::uint8_t ** buffer, std::size_t * size) {
    return Guard([&] {
        std::ostringstream stream(std::ios::binary);
        state->Save(stream);
        std::string data = stream.str();
        *size = data.size();
        *buffer = static_cast<std::uint8_t *>(std::malloc(data.empty() ? 1 : data.size()));
        if (*buffer == nullptr)
            throw std::bad_alloc();
        std::memcpy(*buffer, data.data(), data.size());
    });
}

LS_API std::int32_t ls_state_load(State * state, const std::uint8_t * buffer, std::size_t size) {
    return Guard([&] {
        std::istringstream stream(std::string(reinterpret_cast<const char *>(buffer), buffer != nullptr ? size : 0), std::ios::binary);
        state->Load(stream);
    });
}

////// SCRIPT //////

// Compile a script (buffer: error, line, column, instructions, has analysis, analysis)
LS_API std::int32_t ls_script_compile(const char * code, std::size_t length, std::int32_t analyze, std::uint8_t ** buffer, std::size_t * size) {
    return Guard([&] {
        Script::Analysis analysis;
        Script::Result result = Script::Compile(std::string_view(code != nullptr ? code : "", length), analyze ? &analysis : nullptr);
        Writer w;
        WriteResult(w, result, result.column);
        w.U8(analyze ? 1 : 0);
        if (analyze)
            WriteAnalysis(w, analysis);
        w.Output(buffer, size);
    });
}

// Compile and execute a script (buffer: error, line, column, instructions)
LS_API std::int32_t ls_script_execute(State * state, const char * code, std::size_t length, std::uint8_t ** buffer, std::size_t * size, Variable ** out) {
    return Guard([&] {
        Script::Result result = Script::Execute(*state, std::string_view(code != nullptr ? code : "", length));
        Writer w;
        WriteResult(w, result, result.column);
        w.Output(buffer, size);
        *out = NewHandle(result.value);
    });
}

LS_API std::int32_t ls_script_error_message(std::int32_t error, char ** message) {
    return Guard([&] { *message = CopyText(Script::GetErrorMessage(static_cast<Script::Error>(error))); });
}

////// ASSEMBLY //////

// Compile an assembly code (buffer: error, line, column, instructions)
LS_API std::int32_t ls_assembly_compile(const char * code, std::size_t length, std::uint8_t ** buffer, std::size_t * size) {
    return Guard([&] {
        Assembly::Result result = Assembly::Compile(std::string_view(code != nullptr ? code : "", length));
        Writer w;
        WriteResult(w, result, 0);
        w.Output(buffer, size);
    });
}

// Compile a line of assembly code (buffer: one instruction)
LS_API std::int32_t ls_assembly_compile_instruction(const char * line, std::size_t length, std::int32_t * error, std::uint8_t ** buffer, std::size_t * size) {
    return Guard([&] {
        Assembly::Error err = Assembly::Error::None;
        Instruction instruction = Assembly::CompileInstruction(std::string_view(line != nullptr ? line : "", length), err);
        *error = static_cast<std::int32_t>(err);
        Writer w;
        WriteInstruction(w, instruction);
        w.Output(buffer, size);
    });
}

LS_API std::int32_t ls_assembly_disassemble(const std::uint8_t * buffer, std::size_t size, char ** code) {
    return Guard([&] { *code = CopyText(Assembly::Disassemble(ReadInstructions(buffer, size))); });
}

LS_API std::int32_t ls_assembly_disassemble_instruction(const std::uint8_t * buffer, std::size_t size, char ** code) {
    return Guard([&] {
        Reader r(buffer, size);
        *code = CopyText(Assembly::DisassembleInstruction(ReadInstruction(r)));
    });
}

// Compile and execute an assembly code (after: append to the last list)
LS_API std::int32_t ls_assembly_execute(State * state, const char * code, std::size_t length, std::int32_t after, std::uint8_t ** buffer, std::size_t * size, Variable ** out) {
    return Guard([&] {
        std::string_view text(code != nullptr ? code : "", length);
        Assembly::Result result = after ? Assembly::ExecuteAfter(*state, text) : Assembly::Execute(*state, text);
        Writer w;
        WriteResult(w, result, 0);
        w.Output(buffer, size);
        *out = NewHandle(result.value);
    });
}

LS_API std::int32_t ls_assembly_error_message(std::int32_t error, char ** message) {
    return Guard([&] { *message = CopyText(Assembly::GetErrorMessage(static_cast<Assembly::Error>(error))); });
}

////// SERIALIZATION //////

LS_API std::int32_t ls_save_instructions(const std::uint8_t * buffer, std::size_t size, std::uint8_t ** out, std::size_t * out_size) {
    return Guard([&] {
        std::ostringstream stream(std::ios::binary);
        SaveInstructions(stream, ReadInstructions(buffer, size));
        std::string data = stream.str();
        *out_size = data.size();
        *out = static_cast<std::uint8_t *>(std::malloc(data.empty() ? 1 : data.size()));
        if (*out == nullptr)
            throw std::bad_alloc();
        std::memcpy(*out, data.data(), data.size());
    });
}

LS_API std::int32_t ls_load_instructions(const std::uint8_t * buffer, std::size_t size, std::uint8_t ** out, std::size_t * out_size) {
    return Guard([&] {
        std::istringstream stream(std::string(reinterpret_cast<const char *>(buffer), buffer != nullptr ? size : 0), std::ios::binary);
        Writer w;
        WriteInstructions(w, LoadInstructions(stream));
        w.Output(out, out_size);
    });
}

////// STANDARD LIBRARY //////

// Install modules (0 all, 1 Math, 2 conversions, 3 arrays, 4 strings)
LS_API std::int32_t ls_standard_install(State * state, std::int32_t module) {
    return Guard([&] {
        switch (module) {
            case 0: Standard::Install(*state); break;
            case 1: Standard::InstallMath(*state); break;
            case 2: Standard::InstallConversions(*state); break;
            case 3: Standard::InstallArrays(*state); break;
            case 4: Standard::InstallStrings(*state); break;
            default: throw InvalidArgument("invalid module");
        }
    });
}
