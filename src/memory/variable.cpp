#include "variable.hpp"

LiteScript::Variable::Variable(Memory& memory, unsigned int id, Object& obj, unsigned int& nb_ref) :
    memory(memory), id(id), obj(obj), nb_ref(nb_ref)
{
    nb_ref++;
}

LiteScript::Variable::~Variable() {
    if (--this->nb_ref == 0)
        this->memory.RemoveObject(this->id);
}