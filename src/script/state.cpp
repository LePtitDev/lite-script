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

LiteScript::State::State(Memory &memory) :
    instr_index(0), line_num(0), memory(memory),
    nsp_current(memory.Create(Type::NAMESPACE))
{
    this->instr.push_back(std::vector<Instruction>());
    this->nsp_major.push_back(this->nsp_current);
}

LiteScript::State::State(Memory &memory, std::vector<Instruction> instrs) :
    instr_index(0), line_num(0), memory(memory),
    nsp_current(memory.Create(Type::NAMESPACE))
{
    this->instr.push_back(instrs);
    this->nsp_major.push_back(this->nsp_current);
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

    this->line_num++;
}