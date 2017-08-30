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

#include "namer.hpp"
#include "../memory/type.hpp"
#include "../memory/object.hpp"
#include "../types/internal.hpp"
#include "../streamer.hpp"

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

void LiteScript::Namer::Save(std::ostream &stream, Namer &namer, bool (Memory::*caller)(std::ostream&, unsigned int)) {
    (namer.global->memory.*caller)(stream, namer.global->ID);
    (namer.global->memory.*caller)(stream, (*namer.current)->ID);
    OStreamer::Write<unsigned int>(stream, namer.heap.size());
    for (unsigned int i = 0, sz = namer.heap.size(); i < sz; i++)
        (namer.global->memory.*caller)(stream, namer.heap[i]->ID);
}

LiteScript::Namer LiteScript::Namer::Load(std::istream &stream, Memory& memory, unsigned int (Memory::*caller)(std::istream&)) {
    Namer res(memory.GetVariable((memory.*caller)(stream)));
    res.current = memory.GetVariable((memory.*caller)(stream));
    unsigned int sz = IStreamer::Read<unsigned int>(stream);
    for (unsigned int i = 0; i < sz; i++)
        res.heap.push_back(memory.GetVariable((memory.*caller)(stream)));
    return res;
}

void LiteScript::Namer::SaveIDs(std::ostream &stream) const {
    OStreamer streamer(stream);
    streamer << (*this->current)->ID;
    streamer << this->heap.size();
    for (unsigned int i = 0, sz = this->heap.size(); i < sz; i++)
        streamer << this->heap[i]->ID;
}

void LiteScript::Namer::LoadIDs(std::istream &stream) {
    this->current = this->global->memory.GetVariable(IStreamer::Read<unsigned int>(stream));
    unsigned int sz = IStreamer::Read<unsigned int>(stream);
    for (unsigned int i = 0; i < sz; i++)
        this->heap.push_back(this->global->memory.GetVariable(IStreamer::Read<unsigned int>(stream)));
}

void LiteScript::Namer::GarbageCollector(bool (Memory::*caller)(unsigned int)) const {
    for (unsigned int i = 0, sz = this->heap.size(); i < sz; i++)
        Variable(this->heap[i]).GarbageCollector(caller);
}

LiteScript::Namer& LiteScript::Namer::operator=(const Namer &n) {
    this->current = n.current;
    this->heap.clear();
    this->heap = n.heap;
    return *this;
}