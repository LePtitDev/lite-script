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

#include "type.hpp"
#include "object.hpp"
#include "memory.hpp"
#include "basic_memory.hpp"
#include "../script/state.hpp"
#include "../streamer.hpp"

LiteScript::Memory::Memory() :
    first_nfull(0), count(0), Count(count)
{
    for (short i = 0; i < LITESCRIPT_MEMORY_SIZE; i++) {
        this->arr[i] = nullptr;
        this->nfull[i] = i + (short)1;
    }
}

LiteScript::Memory::~Memory() {
    for (unsigned int i = 0; i < LITESCRIPT_MEMORY_SIZE; i++) {
        if (this->arr[i] != nullptr)
            delete (LiteScript::_BasicMemory_1 *)this->arr[i];
    }
}

LiteScript::Variable LiteScript::Memory::Create(Type &type) {
    // Si le premier non plein n'existe pas, on le crée
    if (this->arr[this->first_nfull] == nullptr)
        this->arr[this->first_nfull] = (void *)(new LiteScript::_BasicMemory_1(*this));
    // On crée l'objet
    Variable result = ((LiteScript::_BasicMemory_1 *)(this->arr[this->first_nfull]))->Create(type, ((unsigned int)this->first_nfull) << 16);
    // Si le premier non plein devient plein, on passe au suivant
    if (((LiteScript::_BasicMemory_1 *)(this->arr[this->first_nfull]))->isFull())
        this->first_nfull = this->nfull[this->first_nfull];
    // On incrémente le compteur d'objets
    this->count++;
    // On retourne la variable
    return result;
}

void LiteScript::Memory::Remove(unsigned int id) {
    unsigned int block = id >> 16;
    if (this->arr[block] != nullptr) {
        bool wasFull = ((LiteScript::_BasicMemory_1 *) (this->arr[block]))->isFull();
        unsigned int nb = ((LiteScript::_BasicMemory_1 *) (this->arr[block]))->Count;
        ((LiteScript::_BasicMemory_1 *) (this->arr[block]))->Remove(id & 0xffff);
        // Si le block n'est plus plein, on le place en premier
        if (wasFull && !((LiteScript::_BasicMemory_1 *) (this->arr[block]))->isFull()) {
            this->nfull[block] = this->first_nfull;
            this->first_nfull = (short)block;
        }
        // Si le nombre d'objet à diminué, on décrémente le compteur
        if (nb != ((LiteScript::_BasicMemory_1 *) (this->arr[block]))->Count)
            this->count--;
    }
}

LiteScript::Nullable<LiteScript::Variable> LiteScript::Memory::GetVariable(unsigned int id) {
    unsigned int block = id >> 16;
    if (this->arr[block] != nullptr)
        return Nullable<Variable>(((LiteScript::_BasicMemory_1 *) (this->arr[block]))->GetVariable(id & 0xffff));
    return Nullable<Variable>();
}

void LiteScript::Memory::GarbageCollector(const State& state) {
    std::vector<const State *> tmp;
    tmp.push_back(&state);
    this->GarbageCollector(1, tmp.data());
}

void LiteScript::Memory::GarbageCollector(int scount, const State ** sarray) {
    for (unsigned int i = 0; i < LITESCRIPT_MEMORY_SIZE; i++) {
        if (this->arr[i] != nullptr)
            ((LiteScript::_BasicMemory_1 *)this->arr[i])->FlagsInit();
    }
    for (unsigned int i = 0, sz = (unsigned int)scount; i < sz; i++)
        sarray[i]->GarbageCollector(&Memory::ProtectVariable);
    for (unsigned int i = 0; i < LITESCRIPT_MEMORY_SIZE; i++) {
        if (this->arr[i] != nullptr)
            ((LiteScript::_BasicMemory_1 *)this->arr[i])->FlagsErase();
    }
}

void LiteScript::Memory::Save(std::ostream &stream, const Memory &memory) {
    for (unsigned int i = 0; i < LITESCRIPT_MEMORY_SIZE; i++) {
        if (memory.arr[i] != nullptr)
            ((LiteScript::_BasicMemory_1 *)memory.arr[i])->FlagsInit();
    }
    const std::vector<Type *>& types = Type::GetTypesList();
    OStreamer::Write<unsigned int>(stream, types.size());
    for (unsigned int i = 0, sz = types.size(); i < sz; i++)
        stream << types[i]->GetName() << (uint8_t)0;
    OStreamer::Write<unsigned int>(stream, memory.count);
    for (unsigned int i = 0; i < LITESCRIPT_MEMORY_SIZE; i++) {
        if (memory.arr[i] != nullptr)
            ((LiteScript::_BasicMemory_1 *)memory.arr[i])->Save(stream, &Memory::SaveVariable);
    }
}

LiteScript::Memory LiteScript::Memory::Load(std::istream &stream) {
    Memory memory;
    const std::vector<Type *>& types = Type::GetTypesList();
    unsigned int sz = IStreamer::Read<unsigned int>(stream);
    std::string str;
    unsigned char c;
    for (unsigned int i = 0; i < sz; i++) {
        str.clear();
        while ((c = (unsigned char)stream.get()) != 0)
            str += c;
        for (unsigned int j = 0, sz_j = types.size(); j < sz_j; j++) {
            if (str == types[j]->GetName()) {
                memory.type_list.push_back(types[j]);
                break;
            }
        }
        if (i == memory.type_list.size())
            return memory;
    }
    sz = IStreamer::Read<unsigned int>(stream);
    for (; memory.count < sz; memory.count++)
        memory.LoadVariable(stream);
    return memory;
}

bool LiteScript::Memory::ProtectVariable(unsigned int i) {
    unsigned int block = i >> 16;
    if (this->arr[block] != nullptr)
        return ((LiteScript::_BasicMemory_1 *)(this->arr[block]))->FlagsProtect(i & 0xffff);
    return false;
}

bool LiteScript::Memory::SaveVariable(std::ostream &stream, unsigned int i) {
    OStreamer streamer(stream);
    unsigned int block = i >> 16;
    if (this->arr[block] != nullptr) {
        if (((LiteScript::_BasicMemory_1 *) (this->arr[block]))->FlagsProtect(i & 0xffff)) {
            streamer << (unsigned char)0 << i;
            return true;
        }
        else {
            Variable v = *this->GetVariable(i);
            streamer << (unsigned char)1 << v->ID << v->GetType().GetID();
            v->GetType().Save(stream, *v, &Memory::SaveVariable);
            return false;
        }
    }
    return false;
}

unsigned int LiteScript::Memory::LoadVariable(std::istream &stream) {
    if (stream.get() == 0)
        return IStreamer::Read<unsigned int>(stream);
    else {
        unsigned int i = IStreamer::Read<unsigned int>(stream);
        unsigned int block = i >> 16;
        if (this->arr[block] == nullptr) {
            this->arr[block] = (void *)(new LiteScript::_BasicMemory_1(*this));
            this->nfull[block] = this->first_nfull;
            this->first_nfull = (short)block;
        }
        Object& obj = ((LiteScript::_BasicMemory_1 *)(this->arr[block]))->CreateAt(i);
        this->count++;
        if (((LiteScript::_BasicMemory_1 *)(this->arr[block]))->isFull()) {
            unsigned int j;
            for (j = 0; j < LITESCRIPT_MEMORY_SIZE && this->nfull[j] != block; j++);
            if (j == LITESCRIPT_MEMORY_SIZE)
                this->first_nfull = this->nfull[block];
            else
                this->nfull[j] = this->nfull[block];
        }
        this->type_list[IStreamer::Read<unsigned int>(stream)]->Load(stream, obj, &Memory::LoadVariable);
        return i;
    }
}
