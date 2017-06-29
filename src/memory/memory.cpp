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

#include "memory.hpp"

#include "basic_memory.hpp"

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

void LiteScript::Memory::GarbageCollector() {

}