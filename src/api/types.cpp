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

void LiteScript::NullifyVariable(Variable &v) {
    v->Reassign(Type::NIL, 0);
}

LiteScript::Variable LiteScript::CreateUndefined(Memory &memory) {
    return memory.Create(Type::UNDEFINED);
}

LiteScript::Variable LiteScript::CreateVariable(Memory &memory) {
    return memory.Create(Type::NIL);
}

LiteScript::Variable LiteScript::CreateVariable(Memory &memory, bool value) {
    Variable v = memory.Create(Type::BOOLEAN);
    v->GetData<bool>() = value;
    return v;
}

LiteScript::Variable LiteScript::CreateVariable(Memory &memory, int value) {
    Variable v = memory.Create(Type::NUMBER);
    v->GetData<Number>() = Number(value);
    return v;
}

LiteScript::Variable LiteScript::CreateVariable(Memory &memory, float value) {
    Variable v = memory.Create(Type::NUMBER);
    v->GetData<Number>() = Number(value);
    return v;
}

LiteScript::Variable LiteScript::CreateVariable(Memory &memory, const char *value) {
    Variable v = memory.Create(Type::STRING);
    v->GetData<String>() = String(value);
    return v;
}

LiteScript::Variable LiteScript::CreateVariable(Memory &memory, Variable (* value)(State &, std::vector<Variable> &)) {
    Variable v = memory.Create(Type::CALLBACK);
    v->GetData<Callback>() = Callback(memory, value);
    return v;
}

LiteScript::Variable LiteScript::CreateVariable(Memory &memory, int count, Variable *values) {
    Variable v = memory.Create(Type::ARRAY);
    Array& a = v->GetData<Array>();
    for (unsigned int i = 0, sz = (unsigned int)count; i < sz; i++)
        a[i] = values[i];
    return v;
}

LiteScript::Variable LiteScript::CreateVariable(Memory &memory, int count, const char **keys, Variable *values) {
    Variable v = memory.Create(Type::ARRAY);
    Array& a = v->GetData<Array>();
    for (unsigned int i = 0, sz = (unsigned int)count; i < sz; i++)
        a[keys[i]] = values[i];
    return v;
}

LiteScript::Variable LiteScript::CreateVariable(Memory &memory, Class &value) {
    Variable v = memory.Create(Type::CLASS);
    v->GetData<Class>() = value;
    return v;
}

LiteScript::Variable LiteScript::CreateNamespace(Memory &memory, int count, const char **names, Variable *values) {
    Variable v = memory.Create(Type::NAMESPACE);
    Namespace& n = v->GetData<Namespace>();
    for (unsigned int i = 0, sz = (unsigned int)count; i < sz; i++)
        n.DefineVariable(names[i], values[i]);
    return v;
}