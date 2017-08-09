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

#include "../litescript.hpp"

#include "executor.hpp"

LiteScript::State::State(Memory &memory) :
    instr_index(0), line_num(0), memory(memory),
    nsp_global(memory.Create(Type::NAMESPACE)),
    InstructionIndex(instr_index), InstructionLine(line_num)
{
    this->nsp_current = this->nsp_global;
    this->instr.push_back(std::vector<Instruction>());
    this->args.push_back(std::vector<Variable>());
    this->ths.push_back(Nullable<Variable>(this->memory.Create(Type::UNDEFINED)));
    this->rets.push_back(Nullable<Variable>(this->memory.Create(Type::UNDEFINED)));
}

LiteScript::State::State(const State &state) :
    instr_index(state.instr_index), line_num(state.line_num), memory(state.memory),
    nsp_global(Variable(state.nsp_global)), nsp_current(Nullable<Variable>(state.nsp_current)),
    InstructionIndex(instr_index), InstructionLine(line_num),
    instr(state.instr), nsp_list(state.nsp_list), args(state.args), ths(state.ths), rets(state.rets),
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

LiteScript::Variable LiteScript::State::GetVariable(const char *name) const {
    int index;
    if (this->nsp_list.size() > 0) {
        const Variable& n_last = this->nsp_list.back();
        index = n_last->GetData<Namespace>().IndexOf(name);
        if (index >= 0)
            return n_last->GetData<Namespace>().GetVariable((unsigned int)index);
    }

    const Variable& n_global = this->nsp_global,
                    n_curr = *this->nsp_current;

    index = n_curr->GetData<Namespace>().IndexOf(name);
    if (index >= 0)
        return n_curr->GetData<Namespace>().GetVariable((unsigned int)index);
    if (n_global->ID != n_curr->ID) {
        index = n_global->GetData<Namespace>().IndexOf(name);
        if (index >= 0)
            return n_global->GetData<Namespace>().GetVariable((unsigned int)index);
    }

    return this->memory.Create(Type::UNDEFINED);
}

LiteScript::Variable LiteScript::State::GetCurrentNamespace() const {
    if (this->nsp_list.size() > 0)
        return Variable(this->nsp_list.back());
    else
        return Variable(*this->nsp_current);
}

void LiteScript::State::UseNamespace(const char *name) {
    std::string nsp(name);
    if (nsp == "global") {
        this->nsp_current = this->nsp_global;
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
    this->nsp_current = *v;
}

void LiteScript::State::UseNamespace(const Variable& n) {
    if (n->GetType() != Type::NAMESPACE)
        return;
    this->nsp_current = n;
}

unsigned int LiteScript::State::GetArgsCount() const {
    if (this->args.size() > 0)
        return this->args.back().size();
    else
        return 0;
}

LiteScript::Variable LiteScript::State::GetArg(unsigned int i) const {
    if (this->args.size() > 0 && this->args.back().size() > i)
        return Variable(this->args.back().at(i));
    else
        return this->memory.Create(Type::UNDEFINED);
}

LiteScript::Nullable<LiteScript::Variable>& LiteScript::State::GetThis() {
    return this->ths.back();
}

LiteScript::Nullable<LiteScript::Variable> & LiteScript::State::GetReturn() {
    return this->rets.back();
}

void LiteScript::State::PushCall(unsigned int index, unsigned int line) {
    this->call_lifo.push_back({ this->instr_index, this->line_num });
    this->instr_index = index;
    this->line_num = line;
}

void LiteScript::State::PopCall() {
    if (this->call_lifo.size() == 0)
        return;
    std::array<unsigned int, 2>& cp = this->call_lifo.back();
    this->instr_index = cp[0];
    this->line_num = cp[1];
    this->call_lifo.pop_back();
}

void LiteScript::State::JumpTo(unsigned int index, unsigned int line) {
    this->instr_index = index;
    this->line_num = line;
}

const std::vector<LiteScript::Instruction> & LiteScript::State::GetInstruction(unsigned int i) const {
    return this->instr[i];
}