#include <litescript/serialization.hpp>
#include <litescript/state.hpp>

#include <algorithm>
#include <iterator>
#include <unordered_map>

#include "binary_stream.hpp"
#include "trace.hpp"

namespace {

    using namespace LiteScript;
    using Internal::BinaryReader;
    using Internal::BinaryWriter;

    // Header of the saved states
    constexpr char STATE_MAGIC[4] = { 'L', 'S', 'S', 'T' };
    constexpr std::uint32_t STATE_VERSION = 2;

    // Tags of the instruction operands
    enum class OperandTag : std::uint8_t { None, Boolean, Integer, Float, String };

    // Tags of the numbers
    enum class NumberTag : std::uint8_t { Integer, Float };

    // Kinds of callbacks
    enum class CallbackTag : std::uint8_t { Script, Native };

    // Tag of the objects that reference another object
    constexpr std::uint8_t REFERENCE_TAG = static_cast<std::uint8_t>(std::variant_size_v<Data> - 1);

    void WriteInstructions(BinaryWriter& out, const std::vector<Instruction>& instructions) {
        out.Size(instructions.size());
        for (const Instruction& instr : instructions) {
            out.U8(static_cast<std::uint8_t>(instr.code));
            out.U32(instr.position.line);
            out.U32(instr.position.column);
            if (instr.HasBoolean()) {
                out.U8(static_cast<std::uint8_t>(OperandTag::Boolean));
                out.U8(instr.GetBoolean() ? 1 : 0);
            }
            else if (instr.HasInteger()) {
                out.U8(static_cast<std::uint8_t>(OperandTag::Integer));
                out.I64(instr.GetInteger());
            }
            else if (instr.HasFloat()) {
                out.U8(static_cast<std::uint8_t>(OperandTag::Float));
                out.F64(instr.GetFloat());
            }
            else if (instr.HasString()) {
                out.U8(static_cast<std::uint8_t>(OperandTag::String));
                out.Text(instr.GetString());
            }
            else {
                out.U8(static_cast<std::uint8_t>(OperandTag::None));
            }
        }
    }

    std::vector<Instruction> ReadInstructions(BinaryReader& in) {
        std::vector<Instruction> instructions;
        for (std::size_t i = 0, sz = in.Size(); i < sz; i++) {
            std::uint8_t code = in.U8();
            if (code >= INSTRUCTION_COUNT)
                BinaryReader::Fail("invalid instruction code");
            Instruction instr(static_cast<InstrCode>(code));
            instr.position.line = in.U32();
            instr.position.column = in.U32();
            switch (static_cast<OperandTag>(in.U8())) {
                case OperandTag::None: break;
                case OperandTag::Boolean: instr.operand = in.Bool(); break;
                case OperandTag::Integer: instr.operand = in.I64(); break;
                case OperandTag::Float: instr.operand = in.F64(); break;
                case OperandTag::String: instr.operand = in.Text(); break;
                default: BinaryReader::Fail("invalid operand");
            }
            instructions.push_back(std::move(instr));
        }
        return instructions;
    }

    // Write the objects reachable from variables, each object has an index
    class ObjectWriter {

        BinaryWriter& out;

        // The index of each object
        std::unordered_map<const Object *, std::uint32_t> indexes;

        // The objects in the order of their indexes
        std::vector<Variable> objects;

    public:

        explicit ObjectWriter(BinaryWriter& o) : out(o) {}

        // Give an index to an object and to every object reachable from it
        void Collect(const Variable& root) {
            std::vector<Variable> work;
            auto add = [&](const Variable& v) {
                if (v && this->indexes.emplace(v.Get(), static_cast<std::uint32_t>(this->objects.size())).second) {
                    this->objects.push_back(v);
                    work.push_back(v);
                }
            };
            add(root);
            while (!work.empty()) {
                Variable v = std::move(work.back());
                work.pop_back();
                Internal::TraceData(v->data, add);
            }
        }

        void Collect(const Namer& namer) {
            Internal::TraceNamer(namer, [this](const Variable& v) { this->Collect(v); });
        }

        // Write a reference (0 for an empty variable)
        void Reference(const Variable& v) {
            this->out.U32(v ? this->indexes.at(v.Get()) + 1 : 0);
        }

        void WriteNamer(const Namer& namer) {
            this->Reference(namer.global);
            this->Reference(namer.current);
            this->out.Size(namer.scopes.size());
            for (const Variable& v : namer.scopes)
                this->Reference(v);
        }

        void WriteMembers(const MemberList& list) {
            this->out.Size(list.Count());
            for (const auto& [name, value] : list) {
                this->out.Text(name);
                this->Reference(value);
            }
        }

        void WriteObjects() {
            this->out.Size(this->objects.size());
            for (const Variable& v : this->objects) {
                // The tag is the alternative of the content (a reference isn't followed)
                std::size_t tag = v->data.index();
                this->out.U8(static_cast<std::uint8_t>(tag));
                if (const auto * r = std::get_if<LiteScript::Reference>(&v->data)) {
                    this->Reference(r->target);
                    continue;
                }
                switch (static_cast<Type>(tag)) {
                    case Type::Undefined: {
                        const Location& origin = std::get<Undefined>(v->data).origin;
                        this->Reference(origin.container);
                        if (origin.container) {
                            if (const auto * index = std::get_if<std::size_t>(&origin.key)) {
                                this->out.U8(0);
                                this->out.U64(*index);
                            }
                            else {
                                this->out.U8(1);
                                this->out.Text(std::get<std::string>(origin.key));
                            }
                        }
                        break;
                    }
                    case Type::Null:
                        break;
                    case Type::Boolean:
                        this->out.U8(v.As<bool>() ? 1 : 0);
                        break;
                    case Type::Number: {
                        const Number& n = v.As<Number>();
                        if (n.IsInteger()) {
                            this->out.U8(static_cast<std::uint8_t>(NumberTag::Integer));
                            this->out.I64(n.ToInteger());
                        }
                        else {
                            this->out.U8(static_cast<std::uint8_t>(NumberTag::Float));
                            this->out.F64(n.ToFloat());
                        }
                        break;
                    }
                    case Type::String:
                        this->out.Text(v.As<String>().ToUTF8());
                        break;
                    case Type::Character:
                        this->Reference(v.As<Character>().string);
                        this->out.U64(v.As<Character>().index);
                        break;
                    case Type::Callback: {
                        const Callback& c = v.As<Callback>();
                        if (const auto * f = std::get_if<Callback::ScriptFunction>(&c.target)) {
                            this->out.U8(static_cast<std::uint8_t>(CallbackTag::Script));
                            this->out.U64(f->list);
                            this->out.U64(f->line);
                            this->WriteNamer(f->scope);
                        }
                        else {
                            const auto& native = std::get<1>(c.target);
                            if (native == nullptr || native->name.empty())
                                throw SerializationError("LiteScript: an unregistered native function can't be saved");
                            this->out.U8(static_cast<std::uint8_t>(CallbackTag::Native));
                            this->out.Text(native->name);
                        }
                        this->Reference(c.self);
                        break;
                    }
                    case Type::Array: {
                        const Array& a = v.As<Array>();
                        this->out.Size(a.indexed.size());
                        for (const Variable& e : a.indexed)
                            this->Reference(e);
                        this->WriteMembers(a.named);
                        break;
                    }
                    case Type::Class: {
                        const Class& c = v.As<Class>();
                        this->out.Size(c.inherits.size());
                        for (const Variable& e : c.inherits)
                            this->Reference(e);
                        this->WriteMembers(c.statics);
                        this->WriteMembers(c.members);
                        for (const Variable& e : c.operators)
                            this->Reference(e);
                        this->out.Text(c.constructor);
                        break;
                    }
                    case Type::ClassObject:
                        this->Reference(v.As<ClassObject>().base);
                        this->WriteMembers(v.As<ClassObject>().members);
                        break;
                    case Type::Namespace:
                        this->WriteMembers(v.As<Namespace>().variables);
                        break;
                }
            }
        }

    };

    // Read the objects written by ObjectWriter
    class ObjectReader {

        BinaryReader& in;

        Memory& memory;

        // The objects created for each index (objects are created when they are referenced)
        std::unordered_map<std::uint32_t, Variable> objects;

        // The number of objects
        std::uint32_t count = 0;

        // The objects that contain a reference
        std::vector<Variable> references;

    public:

        ObjectReader(BinaryReader& i, Memory& m) : in(i), memory(m) {}

        Variable Get(std::uint32_t index) {
            if (index >= this->count)
                BinaryReader::Fail("invalid object reference");
            Variable& v = this->objects[index];
            if (!v)
                v = this->memory.CreateUndefined();
            return v;
        }

        Variable Reference() {
            std::uint32_t ref = this->in.U32();
            return ref == 0 ? Variable() : this->Get(ref - 1);
        }

        // Read a reference that can't be empty
        Variable RequiredReference() {
            Variable v = this->Reference();
            if (!v)
                BinaryReader::Fail("missing object reference");
            return v;
        }

        Namer ReadNamer() {
            Namer namer;
            namer.global = this->RequiredReference();
            namer.current = this->RequiredReference();
            for (std::size_t i = 0, sz = this->in.Size(); i < sz; i++)
                namer.scopes.push_back(this->RequiredReference());
            return namer;
        }

        MemberList ReadMembers() {
            MemberList list;
            for (std::size_t i = 0, sz = this->in.Size(); i < sz; i++) {
                std::string name = this->in.Text();
                list.Set(name, this->RequiredReference());
            }
            return list;
        }

        void ReadObjects() {
            this->count = this->in.U32();
            for (std::uint32_t i = 0; i < this->count; i++) {
                Variable v = this->Get(i);
                std::uint8_t tag = this->in.U8();
                if (tag == REFERENCE_TAG) {
                    v->data = LiteScript::Reference{ this->RequiredReference() };
                    this->references.push_back(v);
                    continue;
                }
                switch (static_cast<Type>(tag)) {
                    case Type::Undefined: {
                        Undefined u;
                        u.origin.container = this->Reference();
                        if (u.origin.container) {
                            switch (this->in.U8()) {
                                case 0: u.origin.key = static_cast<std::size_t>(this->in.U64()); break;
                                case 1: u.origin.key = this->in.Text(); break;
                                default: BinaryReader::Fail("invalid location");
                            }
                        }
                        v->data = std::move(u);
                        break;
                    }
                    case Type::Null:
                        v->data = Null{};
                        break;
                    case Type::Boolean:
                        v->data = this->in.Bool();
                        break;
                    case Type::Number:
                        switch (static_cast<NumberTag>(this->in.U8())) {
                            case NumberTag::Integer: v->data = Number(this->in.I64()); break;
                            case NumberTag::Float: v->data = Number(this->in.F64()); break;
                            default: BinaryReader::Fail("invalid number");
                        }
                        break;
                    case Type::String:
                        v->data = String(this->in.Text());
                        break;
                    case Type::Character: {
                        Character c;
                        c.string = this->RequiredReference();
                        c.index = this->in.U64();
                        v->data = std::move(c);
                        break;
                    }
                    case Type::Callback: {
                        Callback c;
                        switch (static_cast<CallbackTag>(this->in.U8())) {
                            case CallbackTag::Script: {
                                Callback::ScriptFunction f;
                                f.list = this->in.U64();
                                f.line = this->in.U64();
                                f.scope = this->ReadNamer();
                                c.target = std::move(f);
                                break;
                            }
                            case CallbackTag::Native: {
                                std::string name = this->in.Text();
                                auto native = this->memory.FindFunction(name);
                                if (native == nullptr)
                                    throw SerializationError("LiteScript: the native function \"" + name + "\" isn't registered");
                                c.target = std::move(native);
                                break;
                            }
                            default:
                                BinaryReader::Fail("invalid callback");
                        }
                        c.self = this->Reference();
                        v->data = std::move(c);
                        break;
                    }
                    case Type::Array: {
                        Array a;
                        for (std::size_t j = 0, sz = this->in.Size(); j < sz; j++)
                            a.indexed.push_back(this->RequiredReference());
                        a.named = this->ReadMembers();
                        v->data = std::move(a);
                        break;
                    }
                    case Type::Class: {
                        Class c;
                        for (std::size_t j = 0, sz = this->in.Size(); j < sz; j++)
                            c.inherits.push_back(this->RequiredReference());
                        c.statics = this->ReadMembers();
                        c.members = this->ReadMembers();
                        for (Variable& op : c.operators)
                            op = this->Reference();
                        c.constructor = this->in.Text();
                        v->data = std::move(c);
                        break;
                    }
                    case Type::ClassObject: {
                        ClassObject o;
                        o.base = this->RequiredReference();
                        o.members = this->ReadMembers();
                        v->data = std::move(o);
                        break;
                    }
                    case Type::Namespace: {
                        Namespace n;
                        n.variables = this->ReadMembers();
                        v->data = std::move(n);
                        break;
                    }
                    default:
                        BinaryReader::Fail("invalid object type");
                }
            }
            // A reference must lead to a value (no cycle of references)
            for (const Variable& v : this->references) {
                const Object * obj = v.Get();
                for (std::size_t steps = 0; obj->IsReference(); steps++) {
                    if (steps >= this->references.size())
                        BinaryReader::Fail("cycle of references");
                    obj = std::get<LiteScript::Reference>(obj->data).target.Get();
                }
            }
        }

    };

}

void LiteScript::SaveInstructions(std::ostream& stream, const std::vector<Instruction>& instructions) {
    BinaryWriter out(stream);
    WriteInstructions(out, instructions);
    out.Check();
}

std::vector<LiteScript::Instruction> LiteScript::LoadInstructions(std::istream& stream) {
    BinaryReader in(stream);
    return ReadInstructions(in);
}

void LiteScript::State::Save(std::ostream& stream) const {
    // When the state is saved during a call of a native function, it's saved as if the
    // native function had returned undefined
    std::size_t frame_count = this->frames.size();
    for (std::size_t i = 1; i < this->frames.size(); i++) {
        if (this->frames[i].kind == FrameKind::Native) {
            frame_count = i;
            break;
        }
    }
    std::size_t list = this->list_index, line_num = this->line;
    std::vector<Variable> saved_stack = this->stack;
    std::vector<std::vector<Variable>> saved_args = this->pending_args;
    if (frame_count < this->frames.size()) {
        const Frame& native = this->frames[frame_count];
        list = native.return_list;
        line_num = native.return_line;
        saved_stack.resize(std::min(saved_stack.size(), native.stack_base));
        saved_args.resize(std::min(saved_args.size(), native.args_base));
        if (native.deliver == FrameKind::Call)
            saved_stack.push_back(this->memory.CreateUndefined());
    }

    BinaryWriter out(stream);
    ObjectWriter objects(out);
    objects.Collect(this->global);
    for (const Variable& v : saved_stack)
        objects.Collect(v);
    for (const auto& args : saved_args) {
        for (const Variable& v : args)
            objects.Collect(v);
    }
    for (std::size_t i = 0; i < frame_count; i++) {
        const Frame& frame = this->frames[i];
        objects.Collect(frame.namer);
        objects.Collect(frame.self);
        objects.Collect(frame.result);
        for (const Variable& v : frame.args)
            objects.Collect(v);
    }

    stream.write(STATE_MAGIC, sizeof(STATE_MAGIC));
    out.U32(STATE_VERSION);
    out.Size(this->lists.size());
    for (const auto& instructions : this->lists)
        WriteInstructions(out, instructions);
    objects.WriteObjects();
    out.U64(list);
    out.U64(line_num);
    objects.Reference(this->global);
    out.Size(saved_stack.size());
    for (const Variable& v : saved_stack)
        objects.Reference(v);
    out.Size(saved_args.size());
    for (const auto& args : saved_args) {
        out.Size(args.size());
        for (const Variable& v : args)
            objects.Reference(v);
    }
    // The try blocks of the saved frames
    std::vector<Handler> saved_handlers;
    for (const Handler& handler : this->handlers) {
        if (handler.depth <= frame_count && handler.stack_size <= saved_stack.size() && handler.args_size <= saved_args.size())
            saved_handlers.push_back(handler);
    }
    out.Size(saved_handlers.size());
    for (const Handler& handler : saved_handlers) {
        out.U64(handler.list);
        out.U64(handler.line);
        out.U64(handler.depth);
        out.U64(handler.stack_size);
        out.U64(handler.args_size);
    }
    out.Size(frame_count);
    for (std::size_t i = 0; i < frame_count; i++) {
        const Frame& frame = this->frames[i];
        out.U8(static_cast<std::uint8_t>(frame.kind));
        objects.WriteNamer(frame.namer);
        objects.Reference(frame.self);
        objects.Reference(frame.result);
        out.Size(frame.args.size());
        for (const Variable& v : frame.args)
            objects.Reference(v);
        out.U64(frame.return_list);
        out.U64(frame.return_line);
        out.U64(frame.stack_base);
        out.U64(frame.args_base);
    }
    out.Check();
}

void LiteScript::State::Load(std::istream& stream) {
    BinaryReader in(stream);
    char magic[sizeof(STATE_MAGIC)];
    for (char& c : magic)
        c = static_cast<char>(in.U8());
    if (!std::equal(std::begin(magic), std::end(magic), std::begin(STATE_MAGIC)))
        BinaryReader::Fail("not a saved state");
    if (in.U32() != STATE_VERSION)
        BinaryReader::Fail("unsupported version");

    std::vector<std::vector<Instruction>> new_lists;
    for (std::size_t i = 0, sz = in.Size(); i < sz; i++)
        new_lists.push_back(ReadInstructions(in));

    ObjectReader objects(in, this->memory);
    objects.ReadObjects();

    std::uint64_t new_list = in.U64(), new_line = in.U64();
    if (new_list > new_lists.size() || (new_list < new_lists.size() && new_line > new_lists[new_list].size()))
        BinaryReader::Fail("invalid position");
    Variable new_global = objects.RequiredReference();
    if (!new_global.Is<Namespace>())
        BinaryReader::Fail("the global namespace isn't a namespace");

    std::vector<Variable> new_stack;
    for (std::size_t i = 0, sz = in.Size(); i < sz; i++)
        new_stack.push_back(objects.RequiredReference());

    std::vector<std::vector<Variable>> new_args;
    for (std::size_t i = 0, sz = in.Size(); i < sz; i++) {
        std::vector<Variable> args;
        for (std::size_t j = 0, sz_j = in.Size(); j < sz_j; j++)
            args.push_back(objects.RequiredReference());
        new_args.push_back(std::move(args));
    }

    std::vector<Handler> new_handlers;
    for (std::size_t i = 0, sz = in.Size(); i < sz; i++) {
        Handler handler;
        handler.list = static_cast<std::size_t>(in.U64());
        handler.line = static_cast<std::size_t>(in.U64());
        handler.depth = static_cast<std::size_t>(in.U64());
        handler.stack_size = static_cast<std::size_t>(in.U64());
        handler.args_size = static_cast<std::size_t>(in.U64());
        new_handlers.push_back(handler);
    }

    std::vector<Frame> new_frames;
    for (std::size_t i = 0, sz = in.Size(); i < sz; i++) {
        Frame frame;
        std::uint8_t kind = in.U8();
        // The first frame is the global scope, native frames are never saved
        if (kind > static_cast<std::uint8_t>(FrameKind::Host) || (i == 0 && kind != static_cast<std::uint8_t>(FrameKind::Scope)))
            BinaryReader::Fail("invalid frame");
        frame.kind = static_cast<FrameKind>(kind);
        frame.namer = objects.ReadNamer();
        frame.self = objects.Reference();
        frame.result = objects.Reference();
        for (std::size_t j = 0, sz_j = in.Size(); j < sz_j; j++)
            frame.args.push_back(objects.RequiredReference());
        frame.return_list = in.U64();
        frame.return_line = in.U64();
        frame.stack_base = in.U64();
        frame.args_base = in.U64();
        new_frames.push_back(std::move(frame));
    }
    if (new_frames.empty())
        BinaryReader::Fail("missing global frame");
    for (const Handler& handler : new_handlers) {
        if (handler.depth == 0 || handler.depth > new_frames.size() || handler.list >= new_lists.size())
            BinaryReader::Fail("invalid try block");
    }

    // Everything is valid, the content of the state is replaced
    // The instruction being executed (if the state is loaded by a native function) stays valid
    if (this->running > 0) {
        for (auto& list : this->lists)
            this->retired.push_back(std::move(list));
    }
    this->lists = std::move(new_lists);
    this->list_index = static_cast<std::size_t>(new_list);
    this->line = static_cast<std::size_t>(new_line);
    this->global = std::move(new_global);
    this->stack = std::move(new_stack);
    this->pending_args = std::move(new_args);
    this->frames = std::move(new_frames);
    this->handlers = std::move(new_handlers);
    this->handler_floors.clear();
    this->host_result.Reset();
}
