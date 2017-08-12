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

LiteScript::Namer::Namer(const Variable &global) : global(global), current(Variable(global)) {}
LiteScript::Namer::Namer(const Namer &n) :
    global(n.global), current(n.current), heap(n.heap) {}

void LiteScript::Namer::Use(const Variable &nsp) {
    this->current = Variable(nsp);
}

void LiteScript::Namer::Push(const Variable &nsp) {
    if (nsp->GetType() != Type::NAMESPACE && nsp->GetType() != Type::CLASS)
        return;
    this->heap.push_back(Variable(nsp));
}

void LiteScript::Namer::Pop(const Variable& nsp) {
    if (this->heap.size() > 0 && nsp->ID == this->heap.back()->ID)
        this->heap.pop_back();
}

LiteScript::Variable LiteScript::Namer::Get(const char *name) const {
    for (int i = this->heap.size() - 1; i >= 0; i--) {
        Variable tmp = Variable(this->heap[i]).GetMember(name);
        if (tmp->GetType() != Type::UNDEFINED)
            return tmp;
    }
    Variable tmp = Variable(*this->current).GetMember(name);
    if (tmp->GetType() != Type::UNDEFINED)
        return tmp;
    return Variable(this->global).GetMember(name);
}

void LiteScript::Namer::Declare(const char *name, const Variable &v) {
    Nullable<Variable> tmp;
    for (int i = this->heap.size() - 1; i >= 0; i--) {
        if (this->heap[i]->GetType() == Type::NAMESPACE) {
            tmp = this->heap[i];
            break;
        }
    }
    if (tmp.isNull)
        tmp = this->current;
    (*tmp)->GetData<Namespace>().DefineVariable(name, v);
}

void LiteScript::Namer::GarbageCollector(void (Memory::*caller)(unsigned int)) const {
    for (unsigned int i = 0, sz = this->heap.size(); i < sz; i++)
        Variable(this->heap[i]).GarbageCollector(caller);
}

LiteScript::Namer& LiteScript::Namer::operator=(const Namer &n) {
    this->current = n.current;
    this->heap.clear();
    this->heap = n.heap;
    return *this;
}