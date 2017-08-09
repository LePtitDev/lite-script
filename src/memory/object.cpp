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

std::allocator<char> LiteScript::ObjectAllocator;

LiteScript::Object::Object(Memory& mem, unsigned int id) :
    ID(id), memory(mem), type(&Type::NIL), size(0), data(nullptr)
{

}

LiteScript::Object::Object(const Object & obj) :
    ID(obj.ID), memory(obj.memory), type(&Type::NIL), size(0), data(nullptr)
{
    obj.type->AssignObject(*this);
}

LiteScript::Object::~Object() {
    this->type->ODestroy(*this);
    if (this->size > 0) {
        this->size = 0;
        ObjectAllocator.deallocate((char *)this->data, this->size);
    }
}

void LiteScript::Object::Clear() {
    this->Reassign(Type::NIL, 0);
}

LiteScript::Type & LiteScript::Object::GetType() const {
    return *this->type;
}

LiteScript::Object& LiteScript::Object::Reassign(Type& type, unsigned int size) {
    if (*this->type != Type::NIL) {
        this->type->ODestroy(*this);
        if (this->size > 0) {
            this->size = 0;
            ObjectAllocator.deallocate((char *) this->data, this->size);
            this->data = nullptr;
        }
    }
    this->size = size;
    this->type = &type;
    if (size > 0)
        this->data = ObjectAllocator.allocate(size);
    return *this;
}
