#include "basic_memory.hpp"

////// BASIC_MEMORY_0 //////

LiteScript::_BasicMemory_0::_BasicMemory_0(Memory &mem) :
    count(0), first_free(0), memory(mem), Count(count)
{
    for (short i = 0; i < LITESCRIPT_MEMORY_0_SIZE; i++)
        this->free[i] = i + (short)1;
}

LiteScript::_BasicMemory_0::~_BasicMemory_0() {
    for (unsigned int i = 0; i < LITESCRIPT_MEMORY_0_SIZE; i++) {
        if (this->free[i] == -1)
            this->allocator.destroy(&(*this)[i]);
    }
}

LiteScript::Variable LiteScript::_BasicMemory_0::Create(Type &type, unsigned int id) {
    unsigned int index = (unsigned int)this->first_free;
    this->allocator.construct(&((*this)[index]), this->memory, id | index);
    if (type != Type::NIL)
        type.AssignObject((*this)[index]);
    this->ref_cpt[index] = 0;
    this->count++;
    this->first_free = this->free[index];
    this->free[index] = -1;
    return Variable((*this)[index], this->ref_cpt[index]);
}

void LiteScript::_BasicMemory_0::Remove(unsigned int id) {
    if (this->free[id] != -1)
        return;
    this->free[id] = this->first_free;
    this->first_free = (short)id;
    this->allocator.destroy(&(*this)[id]);
    this->count--;
}

LiteScript::Nullable<LiteScript::Variable> LiteScript::_BasicMemory_0::GetVariable(unsigned int id) {
    if (this->free[id] == -1)
        return Nullable<Variable>(Variable((*this)[id], this->ref_cpt[id]));
    else
        return Nullable<Variable>();
}

////// BASIC_MEMORY_1 //////

LiteScript::_BasicMemory_1::_BasicMemory_1(Memory &memory) :
    count(0), memory(memory), first_nfull(0), Count(count)
{
    for (short i = 0; i < LITESCRIPT_MEMORY_1_SIZE; i++) {
        this->arr[i] = nullptr;
        this->nfull[i] = i + (short)1;
    }
}

LiteScript::_BasicMemory_1::~_BasicMemory_1() {
    for (unsigned int i = 0; i < LITESCRIPT_MEMORY_1_SIZE; i++) {
        if (this->arr[i] != nullptr)
            delete this->arr[i];
    }
}

LiteScript::Variable LiteScript::_BasicMemory_1::Create(Type &type, unsigned int id) {
    // Si le premier non plein n'existe pas, on le crée
    if (this->arr[this->first_nfull] == nullptr)
        this->arr[this->first_nfull] = new LiteScript::_BasicMemory_0(this->memory);
    // On crée l'objet
    Variable result = this->arr[this->first_nfull]->Create(type, id | (((unsigned int)this->first_nfull) << 8));
    // Si le premier non plein devient plein, on passe au suivant
    if (this->arr[this->first_nfull]->isFull())
        this->first_nfull = this->nfull[this->first_nfull];
    // On incrémente le compteur d'objets
    this->count++;
    // On retourne la variable
    return result;
}

void LiteScript::_BasicMemory_1::Remove(unsigned int id) {
    unsigned int block = id >> 8;
    if (this->arr[block] != nullptr) {
        bool wasFull = this->arr[block]->isFull();
        unsigned int nb = this->arr[block]->Count;
        this->arr[block]->Remove(id & 0xff);
        // Si le block n'est plus plein, on le place en premier
        if (wasFull && !this->arr[block]->isFull()) {
            this->nfull[block] = this->first_nfull;
            this->first_nfull = (short)block;
        }
        // Si le nombre d'objet à diminué, on décrémente le compteur
        if (nb != this->arr[block]->Count)
            this->count--;
    }
}

LiteScript::Nullable<LiteScript::Variable> LiteScript::_BasicMemory_1::GetVariable(unsigned int id) {
    unsigned int block = id >> 8;
    if (this->arr[block] != nullptr)
        return Nullable<Variable>(this->arr[block]->GetVariable(id & 0xff));
    return Nullable<Variable>();
}