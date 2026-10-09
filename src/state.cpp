#include <litescript/state.hpp>

#include <algorithm>

namespace {

    // Maximum number of arguments of a call
    constexpr std::int64_t MAX_ARGS = 65536;

    // Maximum index of an array element pushed by an instruction
    constexpr std::int64_t MAX_ARRAY_PUSH = std::int64_t(1) << 24;

    // Return the operator applied by an instruction
    LiteScript::Operator ToOperator(LiteScript::InstrCode code) {
        using LiteScript::InstrCode;
        using LiteScript::Operator;
        switch (code) {
            case InstrCode::OpAssign: return Operator::Assign;
            case InstrCode::OpUnaryPlus: return Operator::UnaryPlus;
            case InstrCode::OpUnaryMinus: return Operator::UnaryMinus;
            case InstrCode::OpPreIncrement: return Operator::PreIncrement;
            case InstrCode::OpPostIncrement: return Operator::PostIncrement;
            case InstrCode::OpPreDecrement: return Operator::PreDecrement;
            case InstrCode::OpPostDecrement: return Operator::PostDecrement;
            case InstrCode::OpAdd: return Operator::Add;
            case InstrCode::OpSubtract: return Operator::Subtract;
            case InstrCode::OpMultiply: return Operator::Multiply;
            case InstrCode::OpDivide: return Operator::Divide;
            case InstrCode::OpModulo: return Operator::Modulo;
            case InstrCode::OpEqual: return Operator::Equal;
            case InstrCode::OpNotEqual: return Operator::NotEqual;
            case InstrCode::OpGreater: return Operator::Greater;
            case InstrCode::OpLess: return Operator::Less;
            case InstrCode::OpGreaterOrEqual: return Operator::GreaterOrEqual;
            case InstrCode::OpLessOrEqual: return Operator::LessOrEqual;
            case InstrCode::OpLogicalNot: return Operator::LogicalNot;
            case InstrCode::OpLogicalAnd: return Operator::LogicalAnd;
            case InstrCode::OpLogicalOr: return Operator::LogicalOr;
            case InstrCode::OpBitwiseNot: return Operator::BitwiseNot;
            case InstrCode::OpBitwiseAnd: return Operator::BitwiseAnd;
            case InstrCode::OpBitwiseOr: return Operator::BitwiseOr;
            case InstrCode::OpBitwiseXor: return Operator::BitwiseXor;
            case InstrCode::OpLeftShift: return Operator::LeftShift;
            case InstrCode::OpRightShift: return Operator::RightShift;
            case InstrCode::OpAddAssign: return Operator::AddAssign;
            case InstrCode::OpSubtractAssign: return Operator::SubtractAssign;
            case InstrCode::OpMultiplyAssign: return Operator::MultiplyAssign;
            case InstrCode::OpDivideAssign: return Operator::DivideAssign;
            default: return Operator::Array;
        }
    }

    // Return the integer operand of an instruction if it's in [0, max[
    std::optional<std::size_t> IndexOperand(const LiteScript::Instruction& instr, std::int64_t max) {
        if (!instr.HasInteger() || instr.GetInteger() < 0 || instr.GetInteger() >= max)
            return std::nullopt;
        return static_cast<std::size_t>(instr.GetInteger());
    }

}

LiteScript::State::State(Memory& mem) :
    memory(mem), global(mem.CreateNamespace())
{
    Frame root;
    root.namer = Namer(this->global);
    root.self = this->memory.CreateUndefined();
    root.result = this->memory.CreateUndefined();
    this->frames.push_back(std::move(root));
}

////// INSTRUCTIONS //////

std::size_t LiteScript::State::AddInstructions(std::vector<Instruction> instructions) {
    this->lists.push_back(std::move(instructions));
    return this->lists.size() - 1;
}

void LiteScript::State::AddInstruction(Instruction instruction) {
    if (this->lists.empty())
        this->lists.emplace_back();
    auto& list = this->lists.back();
    if (this->running > 0 && list.size() == list.capacity()) {
        // The list can contain the instruction being executed, it's kept until the end of the execution
        std::vector<Instruction> grown;
        grown.reserve(list.capacity() * 2 + 16);
        grown.insert(grown.end(), list.begin(), list.end());
        this->retired.push_back(std::move(list));
        list = std::move(grown);
    }
    list.push_back(std::move(instruction));
}

////// EXECUTION //////

LiteScript::Variable LiteScript::State::Execute() {
    while (this->Step());
    return this->Top();
}

LiteScript::Variable LiteScript::State::ExecuteSingle() {
    this->Step();
    return this->Top();
}

LiteScript::Variable LiteScript::State::ExecuteInstruction(const Instruction& instruction) {
    this->Run(instruction);
    return this->Top();
}

bool LiteScript::State::Finished() const noexcept {
    if (this->list_index < this->lists.size() && this->line < this->lists[this->list_index].size())
        return false;
    // A function that reaches the end of its list returns
    for (std::size_t i = this->frames.size() - 1; i > 0; i--) {
        if (this->frames[i].kind == FrameKind::Native)
            break;
        if (this->frames[i].kind != FrameKind::Scope)
            return false;
    }
    for (std::size_t i = this->list_index + 1; i < this->lists.size(); i++) {
        if (!this->lists[i].empty())
            return false;
    }
    return true;
}

bool LiteScript::State::Step() {
    while (this->list_index >= this->lists.size() || this->line >= this->lists[this->list_index].size()) {
        // A function that reaches the end of its list returns
        bool in_function = false;
        for (std::size_t i = this->frames.size() - 1; i > 0; i--) {
            if (this->frames[i].kind == FrameKind::Native)
                break;
            if (this->frames[i].kind != FrameKind::Scope) {
                in_function = true;
                break;
            }
        }
        if (in_function) {
            this->Return();
            continue;
        }
        if (this->list_index + 1 < this->lists.size()) {
            ++this->list_index;
            this->line = 0;
            continue;
        }
        if (!this->lists.empty())
            this->list_index = this->lists.size() - 1;
        return false;
    }
    if (this->debug_hook && this->evaluating == 0) {
        this->debug_hook(*this);
        if (this->list_index >= this->lists.size() || this->line >= this->lists[this->list_index].size())
            return true;
    }
    // The instruction stays valid even if the lists are modified during its execution (see AddInstruction)
    const Instruction& instr = this->lists[this->list_index][this->line++];
    this->Run(instr);
    this->memory.CollectGarbageIfNeeded();
    return true;
}

void LiteScript::State::Run(const Instruction& instr) {
    struct RunningGuard {
        State& state;
        explicit RunningGuard(State& s) : state(s) { ++this->state.running; }
        ~RunningGuard() {
            if (--this->state.running == 0)
                this->state.retired.clear();
        }
    } guard(*this);
    this->executing = instr.position;
    try {
        this->Dispatch(instr);
    }
    catch (const ScriptError& error) {
        if (this->Catch(error))
            return;
        if (this->handler_floors.empty()) {
            if (this->error_hook && this->evaluating == 0)
                this->error_hook(*this, error);
            // The error isn't caught: the current list of the global scope is stopped
            std::size_t root_list = this->list_index;
            for (std::size_t i = 1; i < this->frames.size(); i++) {
                if (this->frames[i].kind != FrameKind::Scope) {
                    root_list = this->frames[i].return_list;
                    break;
                }
            }
            this->frames.resize(1);
            this->stack.clear();
            this->pending_args.clear();
            this->handlers.clear();
            this->list_index = root_list;
            this->line = root_list < this->lists.size() ? this->lists[root_list].size() : 0;
        }
        throw;
    }
}

void LiteScript::State::Raise(const std::string& message) {
    Array error;
    error.named.Set("message", this->memory.CreateString(message));
    error.named.Set("line", this->memory.CreateNumber(static_cast<std::int64_t>(this->executing.line) + 1));
    error.named.Set("column", this->memory.CreateNumber(static_cast<std::int64_t>(this->executing.column) + 1));
    throw ScriptError(this->memory.Create(std::move(error)), message, this->executing);
}

bool LiteScript::State::Catch(const ScriptError& error) {
    while (this->handlers.size() > this->HandlerFloor()) {
        Handler handler = this->handlers.back();
        this->handlers.pop_back();
        // The handlers of functions that returned are ignored
        if (handler.depth > this->frames.size())
            continue;
        this->frames.resize(handler.depth);
        if (this->stack.size() > handler.stack_size)
            this->stack.resize(handler.stack_size);
        if (this->pending_args.size() > handler.args_size)
            this->pending_args.resize(handler.args_size);
        this->stack.push_back(error.GetValue() ? error.GetValue() : this->memory.CreateUndefined());
        this->list_index = handler.list;
        this->line = handler.line;
        return true;
    }
    return false;
}

LiteScript::Variable LiteScript::State::Call(const Variable& callee, std::vector<Variable> args) {
    std::size_t depth = this->frames.size();
    std::size_t saved_list = this->list_index, saved_line = this->line;
    std::size_t saved_stack = this->stack.size(), saved_args = this->pending_args.size();
    SourcePosition saved_position = this->executing;
    // The errors of the call can't be caught by the "try" blocks outside of the call
    std::size_t floor = this->handlers.size();
    this->handler_floors.push_back(floor);
    this->host_result.Reset();
    try {
        this->Invoke(callee, std::move(args), FrameKind::Host);
        while (this->frames.size() > depth && this->Step());
    }
    catch (...) {
        // Restore the state as it was before the call
        this->handler_floors.pop_back();
        if (this->handlers.size() > floor)
            this->handlers.resize(floor);
        this->executing = saved_position;
        this->frames.resize(depth);
        if (this->stack.size() > saved_stack)
            this->stack.resize(saved_stack);
        if (this->pending_args.size() > saved_args)
            this->pending_args.resize(saved_args);
        this->list_index = saved_list;
        this->line = saved_line;
        throw;
    }
    this->handler_floors.pop_back();
    if (this->handlers.size() > floor)
        this->handlers.resize(floor);
    this->executing = saved_position;
    Variable result = std::move(this->host_result);
    this->host_result.Reset();
    return result ? result : this->memory.CreateUndefined();
}

////// DEBUGGING //////

std::vector<LiteScript::State::FrameInfo> LiteScript::State::GetFrames() const {
    std::vector<FrameInfo> res;
    for (std::size_t i = 0; i < this->frames.size(); i++) {
        const Frame& frame = this->frames[i];
        if (frame.kind == FrameKind::Native)
            continue;
        if (i > 0 && frame.kind == FrameKind::Scope && !res.empty()) {
            // A scope belongs to its function: its namer contains the variables of the function
            res.back().namer = frame.namer;
            continue;
        }
        FrameInfo info;
        info.namer = frame.namer;
        info.self = frame.self ? frame.self : this->memory.CreateUndefined();
        info.args = frame.args;
        // The position is the return position of the next call, or the current position
        info.list = this->list_index;
        info.line = this->line;
        for (std::size_t j = i + 1; j < this->frames.size(); j++) {
            if (this->frames[j].kind != FrameKind::Scope) {
                info.list = this->frames[j].return_list;
                info.line = this->frames[j].return_line > 0 ? this->frames[j].return_line - 1 : 0;
                break;
            }
        }
        res.push_back(std::move(info));
    }
    return res;
}

LiteScript::Variable LiteScript::State::Evaluate(std::vector<Instruction> instructions, std::size_t max_steps) {
    std::size_t depth = this->frames.size();
    std::size_t saved_list = this->list_index, saved_line = this->line;
    std::size_t saved_stack = this->stack.size(), saved_args = this->pending_args.size();
    std::size_t saved_lists = this->lists.size();
    SourcePosition saved_position = this->executing;
    // The errors of the evaluation can't be caught by the "try" blocks of the script
    std::size_t floor = this->handlers.size();
    this->handler_floors.push_back(floor);
    this->evaluating++;
    auto restore = [&]() {
        this->evaluating--;
        this->handler_floors.pop_back();
        if (this->handlers.size() > floor)
            this->handlers.resize(floor);
        this->executing = saved_position;
        this->frames.resize(depth);
        if (this->stack.size() > saved_stack)
            this->stack.resize(saved_stack);
        if (this->pending_args.size() > saved_args)
            this->pending_args.resize(saved_args);
        // The list of the evaluation is removed if it's still the last one (functions created
        // by the evaluation can't be called after it)
        if (this->lists.size() == saved_lists + 1)
            this->lists.pop_back();
        else if (this->lists.size() > saved_lists)
            this->lists[saved_lists].clear();
        this->list_index = saved_list;
        this->line = saved_line;
    };
    std::size_t list = this->AddInstructions(std::move(instructions));
    this->list_index = list;
    this->line = 0;
    Variable result;
    try {
        std::size_t steps = 0;
        // The evaluation ends when its list is finished in the frame of the evaluation
        while (!(this->frames.size() == depth && this->list_index == list && this->line >= this->lists[list].size())) {
            if (++steps > max_steps)
                throw std::runtime_error("the evaluation takes too many instructions");
            if (this->list_index >= this->lists.size() || this->line >= this->lists[this->list_index].size()) {
                // A function of the evaluation returns at the end of its list
                if (this->frames.size() > depth) {
                    this->Return();
                    continue;
                }
                break;
            }
            const Instruction& instr = this->lists[this->list_index][this->line++];
            // "return" would leave the function being debugged
            if (instr.code == InstrCode::Return && this->frames.size() == depth)
                throw std::runtime_error("\"return\" can't be evaluated");
            this->Run(instr);
        }
        result =this->stack.size() > saved_stack ? this->stack.back() : this->memory.CreateUndefined();
    }
    catch (...) {
        restore();
        throw;
    }
    restore();
    return result;
}

////// VARIABLES //////

LiteScript::Variable LiteScript::State::GetVariable(std::string_view name) const {
    return this->frames.back().namer.Find(name);
}

void LiteScript::State::DeclareVariable(std::string_view name, Variable value) {
    this->global.As<Namespace>().variables.Set(name, std::move(value));
}

void LiteScript::State::UseNamespace(std::string_view path) {
    Namer& namer = this->frames.back().namer;
    if (path == "global") {
        namer.current = this->global;
        return;
    }
    Variable nsp = this->global;
    while (true) {
        std::size_t dot = path.find('.');
        std::string_view name = path.substr(0, dot);
        Namespace& parent = nsp.As<Namespace>();
        Variable child = parent.variables.Find(name);
        if (!child) {
            child = this->memory.CreateNamespace();
            parent.variables.Set(name, child);
        }
        else if (!child.Is<Namespace>()) {
            namer.current = this->global;
            return;
        }
        nsp = std::move(child);
        if (dot == std::string_view::npos)
            break;
        path.remove_prefix(dot + 1);
    }
    namer.current = std::move(nsp);
}

const LiteScript::State::Frame& LiteScript::State::FunctionFrame() const {
    for (std::size_t i = this->frames.size() - 1; i > 0; i--) {
        if (this->frames[i].kind != FrameKind::Scope)
            return this->frames[i];
    }
    return this->frames.front();
}

LiteScript::Variable LiteScript::State::GetThis() const {
    const Frame& frame = this->FunctionFrame();
    return frame.self ? frame.self : this->memory.CreateUndefined();
}

std::size_t LiteScript::State::GetArgsCount() const {
    return this->FunctionFrame().args.size();
}

LiteScript::Variable LiteScript::State::GetArg(std::size_t index) const {
    const auto& args = this->FunctionFrame().args;
    if (index < args.size() && args[index])
        return args[index];
    return this->memory.CreateUndefined();
}

////// INTERNAL EXECUTION //////

LiteScript::Variable LiteScript::State::Top() const {
    if (this->stack.empty())
        return this->memory.CreateUndefined();
    return this->stack.back();
}

LiteScript::Variable LiteScript::State::Pop() {
    Variable v = std::move(this->stack.back());
    this->stack.pop_back();
    // A class leaves the namespaces resolution when it's removed from the stack
    if (v.Is<Class>())
        this->frames.back().namer.Pop(v);
    return v;
}

void LiteScript::State::Deliver(FrameKind kind, Variable result) {
    if (!result)
        result = this->memory.CreateUndefined();
    switch (kind) {
        case FrameKind::Call:
        case FrameKind::Scope:
            this->stack.push_back(std::move(result));
            break;
        case FrameKind::Host:
            this->host_result = std::move(result);
            break;
        default:
            break;
    }
}

void LiteScript::State::Invoke(const Variable& callee, std::vector<Variable> args, FrameKind kind, const Variable& self) {
    // Instances of classes can overload the call operator
    if (callee.Is<ClassObject>()) {
        const Class * c = callee.As<ClassObject>().GetClass();
        Variable op = c != nullptr ? c->GetOperator(Operator::Call) : Variable();
        if (!op.Is<Callback>())
            this->Raise("the instance doesn't overload the call operator");
        this->Invoke(op, std::move(args), kind, callee);
        return;
    }
    if (!callee.Is<Callback>())
        this->Raise("a value of type " + std::string(GetTypeName(callee.GetType())) + " isn't a function");
    if (this->frames.size() >= this->max_depth)
        this->Raise("maximum call depth exceeded");
    const Callback& callback = callee.As<Callback>();
    // "this" and the result are created when they are used (undefined by default)
    Variable this_value = self ? self : callback.self;

    if (callback.IsNative()) {
        // The function is kept alive even if the callback object is modified during the call
        std::shared_ptr<const NativeFunction> native = std::get<1>(callback.target);
        Frame frame;
        frame.kind = FrameKind::Native;
        frame.deliver = kind;
        frame.namer = this->frames.back().namer;
        frame.self = std::move(this_value);
        frame.args = args;
        frame.return_list = this->list_index;
        frame.return_line = this->line;
        frame.stack_base = this->stack.size();
        frame.args_base = this->pending_args.size();
        std::size_t depth = this->frames.size();
        SourcePosition position = this->executing;
        this->frames.push_back(std::move(frame));
        Variable result;
        try {
            if (native->function)
                result = native->function(*this, args);
        }
        catch (const ScriptError&) {
            this->frames.resize(depth);
            this->executing = position;
            throw;
        }
        catch (const std::exception& e) {
            // The exceptions of the native functions become errors of the script
            this->frames.resize(depth);
            this->executing = position;
            this->Raise(e.what());
        }
        catch (...) {
            this->frames.resize(depth);
            this->executing = position;
            throw;
        }
        this->frames.resize(depth);
        this->executing = position;
        this->Deliver(kind, std::move(result));
        return;
    }

    const auto& function = std::get<Callback::ScriptFunction>(callback.target);
    if (function.list >= this->lists.size() || function.line > this->lists[function.list].size())
        this->Raise("the function doesn't exist in the instructions");
    Frame frame;
    frame.kind = kind;
    frame.namer = function.scope;
    frame.namer.Push(this->memory.CreateNamespace());
    frame.self = std::move(this_value);
    frame.args = std::move(args);
    frame.return_list = this->list_index;
    frame.return_line = this->line;
    frame.stack_base = this->stack.size();
    frame.args_base = this->pending_args.size();
    this->list_index = function.list;
    this->line = function.line;
    this->frames.push_back(std::move(frame));
}

void LiteScript::State::Return() {
    // The scopes pushed inside the function are discarded
    while (this->frames.size() > 1 && this->frames.back().kind == FrameKind::Scope)
        this->frames.pop_back();
    if (this->frames.size() == 1 || this->frames.back().kind == FrameKind::Native) {
        // Return outside of a function: the current list of instructions is stopped
        // and the returned value is the only value left on the stack
        this->stack.clear();
        this->pending_args.clear();
        if (this->handlers.size() > this->HandlerFloor())
            this->handlers.resize(this->HandlerFloor());
        Variable result = std::exchange(this->frames.back().result, Variable());
        this->stack.push_back(result ? std::move(result) : this->memory.CreateUndefined());
        if (this->list_index < this->lists.size())
            this->line = this->lists[this->list_index].size();
        return;
    }
    Frame frame = std::move(this->frames.back());
    this->frames.pop_back();
    if (this->stack.size() > frame.stack_base)
        this->stack.erase(this->stack.begin() + static_cast<std::ptrdiff_t>(frame.stack_base), this->stack.end());
    if (this->pending_args.size() > frame.args_base)
        this->pending_args.resize(frame.args_base);
    this->list_index = frame.return_list;
    this->line = frame.return_line;
    // The "try" blocks of the function are left
    while (this->handlers.size() > this->HandlerFloor() && this->handlers.back().depth > this->frames.size())
        this->handlers.pop_back();
    this->Deliver(frame.kind, std::move(frame.result));
}

void LiteScript::State::Operation(Operator op) {
    Variable b;
    if (IsBinary(op)) {
        if (this->stack.size() < 2)
            return;
        b = this->Pop();
    }
    else if (this->stack.empty()) {
        return;
    }
    Variable a = this->Pop();
    OperationResult result = Operate(this->memory, op, a, b);
    if (Variable * v = std::get_if<Variable>(&result)) {
        this->stack.push_back(std::move(*v));
    }
    else {
        OverloadCall& call = std::get<OverloadCall>(result);
        this->Invoke(call.callback, std::move(call.args), FrameKind::Call, call.self);
    }
}

void LiteScript::State::Instantiate(const Variable& base, std::vector<Variable> args) {
    if (!base.Is<Class>())
        this->Raise("a value of type " + std::string(GetTypeName(base.GetType())) + " isn't a class");
    ClassObject instance;
    instance.base = Variable(base->Target());
    for (const auto& [name, value] : base.As<Class>().members) {
        if (!value.Is<Callback>())
            instance.members.Set(name, Copy(this->memory, Clone(this->memory, value)));
    }
    Variable obj = this->memory.Create(std::move(instance));
    this->stack.push_back(obj);
    if (Variable constructor = base.As<Class>().GetConstructor())
        this->Invoke(constructor, std::move(args), FrameKind::Constructor, obj);
}

void LiteScript::State::Dispatch(const Instruction& instr) {
    switch (instr.code) {
        case InstrCode::Invalid:
            break;

        ////// DEFINITIONS //////

        case InstrCode::DefineVariable: {
            if (this->stack.empty() || !instr.HasString())
                break;
            Variable v = this->Pop();
            this->frames.back().namer.Declare(instr.GetString(), std::move(v));
            break;
        }
        case InstrCode::DefineArg: {
            if (this->stack.empty())
                break;
            Variable v = this->Pop();
            auto index = IndexOperand(instr, MAX_ARGS);
            if (!index || this->pending_args.empty())
                break;
            auto& args = this->pending_args.back();
            while (args.size() <= *index)
                args.push_back(this->memory.CreateUndefined());
            args[*index] = std::move(v);
            break;
        }
        case InstrCode::DefineReturn:
            if (!this->stack.empty())
                this->frames.back().result = this->Pop();
            break;

        ////// VALUE CREATIONS //////

        case InstrCode::ValuePop:
            if (!this->stack.empty())
                this->Pop();
            break;
        case InstrCode::ValueCopy:
            if (!this->stack.empty())
                this->stack.push_back(this->stack.back());
            break;
        case InstrCode::ValueAssign:
            if (!this->stack.empty()) {
                Variable v = this->Pop();
                this->stack.push_back(Copy(this->memory, v));
            }
            break;
        case InstrCode::ValueUndefined:
            this->stack.push_back(this->memory.CreateUndefined());
            break;
        case InstrCode::ValueNull:
            this->stack.push_back(this->memory.CreateNull());
            break;
        case InstrCode::ValueBoolean:
            this->stack.push_back(this->memory.CreateBoolean(instr.HasBoolean() && instr.GetBoolean()));
            break;
        case InstrCode::ValueNumber:
            if (instr.HasFloat())
                this->stack.push_back(this->memory.CreateNumber(Number(instr.GetFloat())));
            else
                this->stack.push_back(this->memory.CreateNumber(Number(instr.HasInteger() ? instr.GetInteger() : 0)));
            break;
        case InstrCode::ValueString:
            this->stack.push_back(this->memory.CreateString(instr.HasString() ? instr.GetString() : std::string()));
            break;
        case InstrCode::ValueCallback: {
            auto target = IndexOperand(instr, INT64_MAX);
            if (!target) {
                this->stack.push_back(this->memory.CreateUndefined());
                break;
            }
            Callback callback;
            callback.target = Callback::ScriptFunction{ this->list_index, *target, this->frames.back().namer };
            this->stack.push_back(this->memory.Create(std::move(callback)));
            break;
        }
        case InstrCode::ValueArray:
            this->stack.push_back(this->memory.Create(Array{}));
            break;
        case InstrCode::ValueClass: {
            Variable c = this->memory.Create(Class{});
            this->frames.back().namer.Push(c);
            this->stack.push_back(std::move(c));
            break;
        }
        case InstrCode::ValueObject: {
            Variable base = this->stack.empty() ? Variable() : this->Pop();
            std::vector<Variable> args;
            if (!this->pending_args.empty()) {
                args = std::move(this->pending_args.back());
                this->pending_args.pop_back();
            }
            this->Instantiate(base, std::move(args));
            break;
        }
        case InstrCode::ValueArg: {
            // The argument is copied, a missing argument stays undefined
            Variable arg = this->GetArg(IndexOperand(instr, INT64_MAX).value_or(SIZE_MAX));
            this->stack.push_back(Copy(this->memory, arg));
            break;
        }
        case InstrCode::ValueArgs: {
            Array array;
            const auto& args = this->FunctionFrame().args;
            for (std::size_t i = IndexOperand(instr, INT64_MAX).value_or(SIZE_MAX); i < args.size(); i++)
                array.indexed.push_back(Copy(this->memory, args[i]));
            this->stack.push_back(this->memory.Create(std::move(array)));
            break;
        }
        case InstrCode::ValueThis:
            this->stack.push_back(this->GetThis());
            break;
        case InstrCode::ValueVariable: {
            Variable v = instr.HasString() ? this->frames.back().namer.Find(instr.GetString()) : Variable();
            this->stack.push_back(v ? std::move(v) : this->memory.CreateUndefined());
            break;
        }

        ////// STACKS MANAGEMENT //////

        case InstrCode::PushNamespace: {
            if (this->frames.size() >= this->max_depth)
                break;
            Frame frame;
            frame.namer = this->frames.back().namer;
            frame.namer.Push(this->memory.CreateNamespace());
            frame.self = this->memory.CreateUndefined();
            frame.result = this->memory.CreateUndefined();
            frame.stack_base = this->stack.size();
            frame.args_base = this->pending_args.size();
            this->frames.push_back(std::move(frame));
            break;
        }
        case InstrCode::PushArgs:
            this->pending_args.emplace_back();
            break;
        case InstrCode::PopNamespace:
            if (this->frames.size() > 1 && this->frames.back().kind == FrameKind::Scope) {
                Variable result = std::move(this->frames.back().result);
                this->frames.pop_back();
                this->Deliver(FrameKind::Scope, std::move(result));
            }
            break;
        case InstrCode::PopArgs:
            // The arguments prepared by the caller of the current function are kept
            if (this->pending_args.size() > this->FunctionFrame().args_base)
                this->pending_args.pop_back();
            break;
        case InstrCode::Return:
            this->Return();
            break;

        ////// OPERATIONS //////

        case InstrCode::OpArray:
            if (this->stack.size() >= 2) {
                const Variable& container = this->stack[this->stack.size() - 2];
                if (container.Is<Undefined>() || container.Is<Null>())
                    this->Raise("can't read an element of " + std::string(GetTypeName(container.GetType())));
            }
            this->Operation(Operator::Array);
            break;
        case InstrCode::OpMember:
            if (!this->stack.empty() && instr.HasString()) {
                if (this->stack.back().Is<Undefined>() || this->stack.back().Is<Null>())
                    this->Raise("can't read the member \"" + instr.GetString() + "\" of " + std::string(GetTypeName(this->stack.back().GetType())));
                Variable v = this->Pop();
                this->stack.push_back(GetMember(this->memory, v, instr.GetString()));
            }
            break;
        case InstrCode::OpCall: {
            if (this->stack.empty())
                break;
            Variable callee = this->Pop();
            std::vector<Variable> args;
            if (!this->pending_args.empty()) {
                args = std::move(this->pending_args.back());
                this->pending_args.pop_back();
            }
            this->Invoke(callee, std::move(args), FrameKind::Call);
            break;
        }

        ////// CONTROL INSTRUCTIONS //////

        case InstrCode::JumpTo:
            if (auto target = IndexOperand(instr, INT64_MAX))
                this->line = *target;
            break;
        case InstrCode::JumpIf:
        case InstrCode::JumpElse: {
            if (this->stack.empty())
                break;
            bool condition = IsTruthy(this->Pop());
            auto target = IndexOperand(instr, INT64_MAX);
            if (target && condition == (instr.code == InstrCode::JumpIf))
                this->line = *target;
            break;
        }

        ////// COMPLEX VALUES COMPLETION //////

        case InstrCode::ArrayPushNumeric: {
            if (this->stack.size() < 2)
                break;
            Variable v = this->Pop();
            auto index = IndexOperand(instr, MAX_ARRAY_PUSH);
            if (!index || !this->stack.back().Is<Array>())
                break;
            auto& elements = this->stack.back().As<Array>().indexed;
            while (elements.size() <= *index)
                elements.push_back(this->memory.CreateUndefined());
            elements[*index] = Copy(this->memory, v);
            break;
        }
        case InstrCode::ArrayPushLiteral: {
            if (this->stack.size() < 2)
                break;
            Variable v = this->Pop();
            if (!instr.HasString() || !this->stack.back().Is<Array>())
                break;
            this->stack.back().As<Array>().named.Set(instr.GetString(), Copy(this->memory, v));
            break;
        }
        case InstrCode::ClassPushStatic:
        case InstrCode::ClassPushUnstatic: {
            if (this->stack.size() < 2)
                break;
            Variable v = this->Pop();
            if (!instr.HasString() || !this->stack.back().Is<Class>())
                break;
            Class& c = this->stack.back().As<Class>();
            (instr.code == InstrCode::ClassPushStatic ? c.statics : c.members).Set(instr.GetString(), std::move(v));
            break;
        }
        case InstrCode::ClassPushOperator: {
            if (this->stack.size() < 2)
                break;
            Variable v = this->Pop();
            auto index = IndexOperand(instr, static_cast<std::int64_t>(OPERATOR_COUNT));
            if (index && this->stack.back().Is<Class>())
                this->stack.back().As<Class>().operators[*index] = std::move(v);
            break;
        }
        case InstrCode::ClassInherit: {
            if (this->stack.size() < 2)
                break;
            Variable parent = this->Pop();
            if (this->stack.back().Is<Class>())
                this->stack.back().As<Class>().Inherit(parent);
            break;
        }
        case InstrCode::ClassConstructor:
            if (!this->stack.empty() && instr.HasString() && this->stack.back().Is<Class>())
                this->stack.back().As<Class>().DefineConstructor(instr.GetString());
            break;

        ////// NAMESPACES //////

        case InstrCode::NamespaceUse:
            if (instr.HasString())
                this->UseNamespace(instr.GetString());
            break;

        ////// ERRORS //////

        case InstrCode::TryBegin:
            if (auto target = IndexOperand(instr, INT64_MAX))
                this->handlers.push_back({ this->list_index, *target, this->frames.size(), this->stack.size(), this->pending_args.size() });
            break;
        case InstrCode::TryEnd:
            if (this->handlers.size() > this->HandlerFloor() && this->handlers.back().depth == this->frames.size())
                this->handlers.pop_back();
            break;
        case InstrCode::Throw: {
            Variable value = this->stack.empty() ? this->memory.CreateUndefined() : this->Pop();
            // The message of an error is its member "message" if it exists
            Variable message;
            if (value.Is<Array>())
                message = value.As<Array>().named.Find("message");
            else if (value.Is<ClassObject>())
                message = value.As<ClassObject>().members.Find("message");
            throw ScriptError(value, ToText(message ? message : value).ToUTF8(), this->executing);
        }

        default:
            this->Operation(ToOperator(instr.code));
            break;
    }
}
