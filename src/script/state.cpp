/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/*
    Copyright (C) 2017 LePtitDev
    All rights reserved.

    This software may be modified and distributed under the terms
    of the BSD license.  See the LICENSE file for details.

    Author: Arthur Ferré <leptitdev.com>
*/
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#include "../memory/object.hpp"
#include "state.hpp"
#include "executor.hpp"
#include "../types/internal.hpp"
#include "../streamer.hpp"

LiteScript::State::State(Memory &memory) :
    instr_index(0), line_num(0), memory(memory),
    nsp_global(memory.Create(Type::NAMESPACE)),
    InstructionIndex(instr_index), InstructionLine(line_num)
{
    this->nsp_lifo.push_back(Namer(this->nsp_global));
    this->instr.push_back(std::vector<Instruction>());
    this->args_tmp.push_back(std::vector<Variable>());
    this->args_def.push_back(std::vector<Variable>());
    this->ths.push_back(Nullable<Variable>(this->memory.Create(Type::UNDEFINED)));
    this->rets.push_back(Nullable<Variable>(this->memory.Create(Type::UNDEFINED)));
}

LiteScript::State::State(const State &state) :
    instr_index(state.instr_index), line_num(state.line_num), memory(state.memory),
    nsp_global(Variable(state.nsp_global)),
    InstructionIndex(instr_index), InstructionLine(line_num),
    instr(state.instr), args_tmp(state.args_tmp), args_def(state.args_def), ths(state.ths), rets(state.rets),
    op_lifo(state.op_lifo), call_lifo(state.call_lifo), nsp_lifo(state.nsp_lifo)
{

}

LiteScript::Variable LiteScript::State::Execute() {
    if (this->instr.size() == 0)
        return this->memory.Create(Type::UNDEFINED);
    for (unsigned int sz = this->instr.size(); this->instr_index < sz; this->instr_index++) {
        while (this->line_num < this->instr[this->instr_index].size())
            this->ExecuteSingle();
        this->line_num = 0;
    }
    this->instr_index--;
    this->line_num = this->instr[this->instr_index].size();

    if (this->op_lifo.size() > 0)
        return Variable(this->op_lifo.back());
    else
        return this->memory.Create(Type::UNDEFINED);
}

LiteScript::Variable LiteScript::State::ExecuteSingle() {
    if (this->instr.size() == 0 || this->line_num >= this->instr[this->instr_index].size())
        return this->memory.Create(Type::UNDEFINED);
    StateExecutor::Execute(*this, this->instr[this->instr_index][this->line_num]);

    if (this->op_lifo.size() > 0)
        return Variable(this->op_lifo.back());
    else
        return this->memory.Create(Type::UNDEFINED);
}

LiteScript::Variable LiteScript::State::ExecuteSingle(const Instruction &instr) {
    Instruction in(instr);
    if (instr.code < InstrCode::INSTR_NUMBER) {
        this->line_num--;
        StateExecutor::Execute(*this, in);
    }

    if (this->op_lifo.size() > 0)
        return Variable(this->op_lifo.back());
    else
        return this->memory.Create(Type::UNDEFINED);
}

bool LiteScript::State::Finished() const {
    return (this->instr_index >= this->instr.size() && this->line_num >= this->instr[this->instr_index].size());
}

void LiteScript::State::AddInstruction(const Instruction &in) {
    if (this->instr.size() == 0)
        this->instr.push_back(std::vector<Instruction>());
    if (this->instr_index >= this->instr.size()) {
        this->instr_index = this->instr.size() - 1;
        this->line_num = this->instr[this->instr_index].size();
    }
    this->instr[this->instr_index].push_back(in);
}

void LiteScript::State::AddInstructions(const std::vector<Instruction> &in_list) {
    this->instr.push_back(in_list);
}

const std::vector<LiteScript::Instruction> & LiteScript::State::GetInstruction(unsigned int i) const {
    return this->instr[i];
}

LiteScript::Variable LiteScript::State::GetVariable(const char *name) const {
    return this->nsp_lifo.back().Get(name);
}

void LiteScript::State::DeclareVariable(const char *name, const Variable &v) {
    nsp_global->GetData<Namespace>().DefineVariable(name, v);
}

LiteScript::Namer& LiteScript::State::GetNamer() {
    return this->nsp_lifo.back();
}

void LiteScript::State::UseNamespace(const char *name) {
    std::string nsp(name);
    if (nsp == "global") {
        this->nsp_lifo.back().Use(this->nsp_global);
        return;
    }
    Nullable<Variable> v(this->nsp_global);
    unsigned len = nsp.size() + 1;
    nsp.clear();
    for (unsigned int i = 0; i < len; i++) {
        if (name[i] == '.' || name[i] == '\0') {
            Namespace& tmp = (*v)->GetData<Namespace>();
            int index = tmp.IndexOf(nsp.c_str());
            if (index < 0) {
                v = this->memory.Create(Type::NAMESPACE);
                tmp.DefineVariable(nsp.c_str(), *v);
            }
            else {
                v = tmp.GetVariable((unsigned int)index);
                if ((*v)->GetType() != Type::NAMESPACE) {
                    this->UseNamespace();
                    return;
                }
            }
        }
        else {
            nsp += name[i];
        }
    }
    this->nsp_lifo.back().Use(*v);
}

void LiteScript::State::DefineArgs(const std::vector<Variable> &args) {
    this->args_def.push_back(args);
}

unsigned int LiteScript::State::GetArgsCount() const {
    if (this->args_def.size() > 0)
        return this->args_def.back().size();
    else
        return 0;
}

LiteScript::Variable LiteScript::State::GetArg(unsigned int i) const {
    if (this->args_def.size() > 0 && this->args_def.back().size() > i)
        return Variable(this->args_def.back().at(i));
    else
        return this->memory.Create(Type::UNDEFINED);
}

LiteScript::Variable LiteScript::State::GetThis() {
    return *this->ths.back();
}

void LiteScript::State::SetThis(const Variable &v) {
    this->ths.back() = v;
}

void LiteScript::State::SetReturn(const Variable &v) {
    this->rets.back() = v;
}

void LiteScript::State::AddCallback(unsigned int index, unsigned int line) {
    this->call_lifo.push_back({ this->instr_index, this->line_num });
    this->instr_index = index;
    this->line_num = line;
}

void LiteScript::State::RemoveCallback() {
    if (this->call_lifo.size() == 0)
        return;
    std::array<unsigned int, 2>& cp = this->call_lifo.back();
    this->instr_index = cp[0];
    this->line_num = cp[1];
    this->call_lifo.pop_back();
}

void LiteScript::State::GarbageCollector(bool (Memory::*caller)(unsigned int)) const {
    Variable(this->nsp_global).GarbageCollector(caller);
    for (unsigned int i = 0, sz_i = this->args_tmp.size(); i < sz_i; i++) {
        for (unsigned int j = 0, sz_j = this->args_tmp[i].size(); j < sz_j; j++)
            Variable(this->args_tmp[i][j]).GarbageCollector(caller);
    }
    for (unsigned int i = 0, sz_i = this->args_def.size(); i < sz_i; i++) {
        for (unsigned int j = 0, sz_j = this->args_def[i].size(); j < sz_j; j++)
            Variable(this->args_def[i][j]).GarbageCollector(caller);
    }
    for (unsigned int i = 0, sz_i = this->ths.size(); i < sz_i; i++) {
        if (!this->ths[i].isNull)
            Variable(*this->ths[i]).GarbageCollector(caller);
    }
    for (unsigned int i = 0, sz_i = this->rets.size(); i < sz_i; i++) {
        if (!this->rets[i].isNull)
            Variable(*this->rets[i]).GarbageCollector(caller);
    }
    for (unsigned int i = 0, sz = this->op_lifo.size(); i < sz; i++)
        Variable(this->op_lifo[i]).GarbageCollector(caller);
    for (unsigned int i = 0, sz = this->nsp_lifo.size(); i < sz; i++)
        this->nsp_lifo[i].GarbageCollector(caller);
}

void LiteScript::State::Save(std::ostream &stream, const State &state) {
    OStreamer streamer(stream);
    streamer << state.instr_index << state.line_num;
    streamer << state.nsp_global->ID;
    streamer << state.args_tmp.size();
    for (unsigned int i = 0, sz_i = state.args_tmp.size(); i < sz_i; i++) {
        streamer << state.args_tmp[i].size();
        for (unsigned int j = 0, sz_j = state.args_tmp[i].size(); j < sz_j; j++)
            streamer << state.args_tmp[i][j]->ID;
    }
    streamer << state.args_def.size();
    for (unsigned int i = 0, sz_i = state.args_def.size(); i < sz_i; i++) {
        streamer << state.args_def[i].size();
        for (unsigned int j = 0, sz_j = state.args_def[i].size(); j < sz_j; j++)
            streamer << state.args_def[i][j]->ID;
    }
    streamer << state.ths.size();
    for (unsigned int i = 0, sz = state.ths.size(); i < sz; i++) {
        if (state.ths[i].isNull)
            streamer << (unsigned char)0;
        else
            streamer << (unsigned char)1 << (*state.ths[i])->ID;
    }
    streamer << state.rets.size();
    for (unsigned int i = 0, sz = state.rets.size(); i < sz; i++) {
        if (state.rets[i].isNull)
            streamer << (unsigned char)0;
        else
            streamer << (unsigned char)1 << (*state.rets[i])->ID;
    }
    streamer << state.op_lifo.size();
    for (unsigned int i = 0, sz = state.op_lifo.size(); i < sz; i++)
        streamer << state.op_lifo[i]->ID;
    streamer << state.call_lifo.size();
    for (unsigned int i = 0, sz = state.call_lifo.size(); i < sz; i++)
        streamer << state.call_lifo[i][0] << state.call_lifo[i][1];
    streamer << state.nsp_lifo.size();
    for (unsigned int i = 0, sz = state.nsp_lifo.size(); i < sz; i++)
        state.nsp_lifo[i].SaveIDs(stream);
 }

LiteScript::State LiteScript::State::Load(std::istream &stream, Memory &memory) {
    State state(memory);
    state.instr_index = IStreamer::Read<unsigned int>(stream);
    state.line_num = IStreamer::Read<unsigned int>(stream);
    std::allocator<Variable> allocator;
    allocator.destroy(&state.nsp_global);
    allocator.construct(&state.nsp_global, memory.GetVariable(IStreamer::Read<unsigned int>(stream)));
    state.args_tmp.clear();
    for (unsigned int i = 0, sz_i = IStreamer::Read<unsigned int>(stream); i < sz_i; i++) {
        state.args_tmp.push_back(std::vector<Variable>());
        for (unsigned int j = 0, sz_j = IStreamer::Read<unsigned int>(stream); j < sz_j; j++)
            state.args_tmp[i].push_back(memory.GetVariable(IStreamer::Read<unsigned int>(stream)));
    }
    state.args_tmp.clear();
    for (unsigned int i = 0, sz_i = IStreamer::Read<unsigned int>(stream); i < sz_i; i++) {
        state.args_def.push_back(std::vector<Variable>());
        for (unsigned int j = 0, sz_j = IStreamer::Read<unsigned int>(stream); j < sz_j; j++)
            state.args_def[i].push_back(memory.GetVariable(IStreamer::Read<unsigned int>(stream)));
    }
    state.ths.clear();
    for (unsigned int i = 0, sz = IStreamer::Read<unsigned int>(stream); i < sz; i++) {
        if (stream.get() == 0)
            state.ths.push_back(Nullable<Variable>());
        else
            state.ths.push_back(Nullable<Variable>(memory.GetVariable(IStreamer::Read<unsigned int>(stream))));
    }
    state.rets.clear();
    for (unsigned int i = 0, sz = IStreamer::Read<unsigned int>(stream); i < sz; i++) {
        if (stream.get() == 0)
            state.rets.push_back(Nullable<Variable>());
        else
            state.rets.push_back(Nullable<Variable>(memory.GetVariable(IStreamer::Read<unsigned int>(stream))));
    }
    state.op_lifo.clear();
    for (unsigned int i = 0, sz = IStreamer::Read<unsigned int>(stream); i < sz; i++)
        state.op_lifo.push_back(memory.GetVariable(IStreamer::Read<unsigned int>(stream)));
    state.call_lifo.clear();
    for (unsigned int i = 0, sz = IStreamer::Read<unsigned int>(stream); i < sz; i++)
        state.call_lifo.push_back({ IStreamer::Read<unsigned int>(stream), IStreamer::Read<unsigned int>(stream) });
    state.nsp_lifo.clear();
    for (unsigned int i = 0, sz = IStreamer::Read<unsigned int>(stream); i < sz; i++) {
        state.nsp_lifo.push_back(Namer(state.nsp_global));
        state.nsp_lifo[i].LoadIDs(stream);
    }
    return state;
}