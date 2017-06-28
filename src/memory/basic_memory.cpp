#include "basic_memory.hpp"

LiteScript::_BasicMemory_1::_BasicMemory_1(Memory &memory) :
    count(0), memory(memory), Count(count)
{

}

LiteScript::_BasicMemory_1::~_BasicMemory_1() {

}

LiteScript::Variable LiteScript::_BasicMemory_1::Create(Type &type) {
    unsigned int it = 0;
    Object obj(this->memory, 0);
    return Variable(obj, it);
}

void LiteScript::_BasicMemory_1::Remove(unsigned int id) {

}

LiteScript::Nullable<LiteScript::Variable> LiteScript::_BasicMemory_1::GetVariable(unsigned int id) {
    return Nullable<Variable>();
}