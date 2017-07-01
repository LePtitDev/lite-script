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
    global_nsp(memory.Create(Type::OBJECT)),
    current_nsp(Variable(global_nsp)),
    CurrentInstructions(instr_index), CurrentLine(line_num),
    NamespaceGlobal(global_nsp), NamespaceCurrent(current_nsp)
{
    this->instr.push_back(std::vector<Instruction>());
}

LiteScript::State::State(Memory &memory, std::vector<Instruction> instrs) :
    memory(memory),
    global_nsp(memory.Create(Type::OBJECT)),
    current_nsp(Variable(global_nsp)),
    CurrentInstructions(instr_index), CurrentLine(line_num),
    NamespaceGlobal(global_nsp), NamespaceCurrent(current_nsp)
{
    this->instr.push_back(instrs);
}

void LiteScript::State::Execute() {

}

void LiteScript::State::ExecuteSingle() {

}

void LiteScript::State::JumpTo(unsigned int intr, unsigned int line) {
    if (intr < this->instr.size() && this->instr[intr].size() < line) {
        this->instr_index = intr;
        this->line_num = line;
    }
}