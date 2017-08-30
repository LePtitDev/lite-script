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

unsigned int LiteScript::AddCallback(Variable (*value)(State &, std::vector<Variable> &)) {
    Callback::List.push_back(value);
    return Callback::List.size() - 1;
}

LiteScript::Variable LiteScript::CreateCallback(Memory &memory, unsigned int id) {
    Variable res = memory.Create(Type::CALLBACK);
    res->GetData<Callback>() = Callback(memory, id);
    return res;
}

void LiteScript::DeclareVariable(State &state, const char * name, const Variable &v) {
    state.DeclareVariable(name, v);
}