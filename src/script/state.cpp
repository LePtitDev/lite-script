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
/////////////////////////////////////////////////////////////////////

#include "state.hpp"

#include "executor.hpp"

LiteScript::State::State(Memory &memory) :
    instr_index(0), line_num(0), memory(memory),
    nsp_global(memory.Create(Type::NAMESPACE))
{
    this->nsp_current = this->nsp_global;
    this->instr.push_back(std::vector<Instruction>());
}

LiteScript::State::State(Memory &memory, std::vector<Instruction> instrs) :
    instr_index(0), line_num(0), memory(memory),
    nsp_global(memory.Create(Type::NAMESPACE))
{
    this->nsp_current = this->nsp_global;
    this->instr.push_back(instrs);
}

void LiteScript::State::Execute() {
    for (unsigned int sz = this->instr.size(); this->instr_index < sz; this->instr_index++) {
        unsigned int nb_instr = this->instr[this->instr_index].size();
        while (this->line_num < nb_instr)
            this->ExecuteSingle();
        this->line_num = 0;
    }
    this->instr_index--;
    this->line_num = this->instr[this->instr_index].size();
}

void LiteScript::State::ExecuteSingle() {
    StateExecutor::Execute(*this, this->instr[this->instr_index][this->line_num]);
}

void LiteScript::State::ExecuteSingle(const Instruction &instr) {
    Instruction in(instr);
    if (instr.code < InstrCode::INSTR_NUMBER)
        StateExecutor::Execute(*this, in);
}

LiteScript::Nullable<LiteScript::Variable> LiteScript::State::GetVariable(const char *name) const {
    int index;
    if (this->nsp_list.size() > 0) {
        const Variable& n_last = *this->nsp_list.end();
        index = n_last->GetData<Namespace>().IndexOf(name);
        if (index >= 0)
            return Nullable<Variable>(n_last->GetData<Namespace>().GetVariable((unsigned int)index));
    }

    const Variable& n_global = this->nsp_global,
                    n_curr = *this->nsp_current;

    index = n_curr->GetData<Namespace>().IndexOf(name);
    if (index >= 0)
        return Nullable<Variable>(n_curr->GetData<Namespace>().GetVariable((unsigned int)index));
    if (n_global->ID != n_curr->ID) {
        index = n_global->GetData<Namespace>().IndexOf(name);
        if (index >= 0)
            return Nullable<Variable>(n_global->GetData<Namespace>().GetVariable((unsigned int)index));
    }

    return Nullable<Variable>();
}

LiteScript::Variable LiteScript::State::GetCurrentNamespace() const {
    if (this->nsp_list.size() > 0)
        return Variable(*this->nsp_list.end());
    else
        return Variable(*this->nsp_current);
}