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

#include <cstdint>

#include "../memory/object.hpp"
#include "../memory/variable.hpp"
#include "../memory/memory.hpp"
#include "boolean.hpp"
#include "internal.hpp"

LiteScript::_Type_BOOLEAN LiteScript::_type_boolean;

LiteScript::_Type_BOOLEAN::_Type_BOOLEAN() : Type("BOOLEAN") {}

LiteScript::Variable LiteScript::_Type_BOOLEAN::Convert(const LiteScript::Variable& obj, const LiteScript::Type& type) const {
    if (type == Type::BOOLEAN) {
        Variable res = obj->memory.Create(Type::BOOLEAN);
        res->GetData<bool>() = obj->GetData<bool>();
        return res;
    }
    else if (type == Type::NUMBER) {
        Variable res = obj->memory.Create(Type::NUMBER);
        res->GetData<Number>() = Number(obj->GetData<bool>() ? 1 : 0);
        return res;
    }
    else if (type == Type::STRING) {
        Variable res = obj->memory.Create(Type::STRING);
        res->GetData<String>() = String((std::string)(obj));
        return res;
    }
    else {
        return obj->memory.Create(Type::NIL);
    }
}

LiteScript::Object& LiteScript::_Type_BOOLEAN::AssignObject(LiteScript::Object& obj) {
    obj.Reassign(*this, sizeof(bool));
    obj.GetData<bool>() = false;
    return obj;
}

std::string LiteScript::_Type_BOOLEAN::ToString(const LiteScript::Variable & obj) const {
    return ((obj->GetData<bool>()) ? "true" : "false");
}

LiteScript::Variable LiteScript::_Type_BOOLEAN::OAssign(LiteScript::Variable& src, const LiteScript::Variable& dest) const {
    if (dest->GetType() != *this) {
        dest->GetType().AssignObject(*src);
        src->GetType().OAssign(src, dest);
    }
    else {
        src->GetData<bool>() = dest->GetData<bool>();
    }
    return Variable(src);
}

LiteScript::Variable LiteScript::_Type_BOOLEAN::OEqual(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this)
        res->GetData<bool>() = false;
    else
        res->GetData<bool>() = (obj1->GetData<bool>() == obj2->GetData<bool>());
    return res;
}
LiteScript::Variable LiteScript::_Type_BOOLEAN::ONotEqual(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this)
        res->GetData<bool>() = true;
    else
        res->GetData<bool>() = (obj1->GetData<bool>() != obj2->GetData<bool>());
    return res;
}

LiteScript::Variable LiteScript::_Type_BOOLEAN::OLogicalNot(const LiteScript::Variable& obj) const {
    Variable res = obj->memory.Create(Type::BOOLEAN);
    res->GetData<bool>() = !obj->GetData<bool>();
    return res;
}
LiteScript::Variable LiteScript::_Type_BOOLEAN::OLogicalAnd(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() == *this)
            res->GetData<bool>() = (obj1->GetData<bool>() && tmp->GetData<bool>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<bool>() && obj2->GetData<bool>());
    return res;
}
LiteScript::Variable LiteScript::_Type_BOOLEAN::OLogicalOr(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() == *this)
            res->GetData<bool>() = (obj1->GetData<bool>() || tmp->GetData<bool>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<bool>() || obj2->GetData<bool>());
    return res;
}

void LiteScript::_Type_BOOLEAN::Save(std::ostream &stream, Object &object, bool (Memory::*caller)(std::ostream&, unsigned int)) const {
    stream << (uint8_t)object.GetData<bool>();
}

void LiteScript::_Type_BOOLEAN::Load(std::istream &stream, Object &object, unsigned int (Memory::*caller)(std::istream&)) const {
    object.Reassign(Type::BOOLEAN, sizeof(bool));
    object.GetData<bool>() = (bool)stream.get();
}