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

#include "string.hpp"

#include "undefined.hpp"

LiteScript::_Type_STRING LiteScript::_type_string;

LiteScript::_Type_STRING::_Type_STRING() : Type("STRING") {}

LiteScript::Variable LiteScript::_Type_STRING::Convert(const LiteScript::Variable& obj, const LiteScript::Type& type) const {
    if (type == Type::STRING) {
        Variable res = obj->memory.Create(Type::STRING);
        res->GetData<String>() = obj->GetData<String>();
        return res;
    }
    else {
        return obj->memory.Create(Type::NIL);
    }
}
LiteScript::Object& LiteScript::_Type_STRING::AssignObject(LiteScript::Object& obj) {
    obj.Reassign(*this, sizeof(String));
    std::allocator<String> allocator;
    allocator.construct(&obj.GetData<String>());
    return obj;
}

LiteScript::Variable LiteScript::_Type_STRING::OAssign(LiteScript::Variable& src, const LiteScript::Variable& dest) const {
    if (dest->GetType() != *this) {
        dest->GetType().AssignObject(*src);
        src->GetType().OAssign(src, dest);
    }
    else {
        src->GetData<String>() = dest->GetData<String>();
    }
    return Variable(src);
}

LiteScript::Variable LiteScript::_Type_STRING::OAdd(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() != *this)
            return obj1->memory.Create(Type::NIL);
        Variable res = obj1->memory.Create(Type::STRING);
        res->GetData<String>() = obj1->GetData<String>() + tmp->GetData<String>();
        return res;
    }
    Variable res = obj1->memory.Create(Type::STRING);
    res->GetData<String>() = obj1->GetData<String>() + obj2->GetData<String>();
    return res;
}
LiteScript::Variable LiteScript::_Type_STRING::OMultiply(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != Type::NUMBER) {
        Variable tmp = obj2.Convert(Type::NUMBER);
        if (tmp->GetType() != Type::NUMBER)
            return obj1->memory.Create(Type::NIL);
        Variable res = obj1->memory.Create(Type::STRING);
        res->GetData<String>() = obj1->GetData<String>() * (int)(tmp->GetData<Number>());
        return res;
    }
    Variable res = obj1->memory.Create(Type::STRING);
    res->GetData<String>() = obj1->GetData<String>() * (int)(obj2->GetData<Number>());
    return res;
}

LiteScript::Variable LiteScript::_Type_STRING::OEqual(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() == *this)
            res->GetData<bool>() = (obj1->GetData<String>() == tmp->GetData<String>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<String>() == obj2->GetData<String>());
    return res;
}
LiteScript::Variable LiteScript::_Type_STRING::ONotEqual(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() == *this)
            res->GetData<bool>() = (obj1->GetData<String>() != tmp->GetData<String>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<String>() != obj2->GetData<String>());
    return res;
}
LiteScript::Variable LiteScript::_Type_STRING::OGreater(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() == *this)
            res->GetData<bool>() = (obj1->GetData<String>() > tmp->GetData<String>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<String>() > obj2->GetData<String>());
    return res;
}
LiteScript::Variable LiteScript::_Type_STRING::OLess(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() == *this)
            res->GetData<bool>() = (obj1->GetData<String>() < tmp->GetData<String>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<String>() < obj2->GetData<String>());
    return res;
}
LiteScript::Variable LiteScript::_Type_STRING::OGreaterOrEqual(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() == *this)
            res->GetData<bool>() = (obj1->GetData<String>() >= tmp->GetData<String>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<String>() >= obj2->GetData<String>());
    return res;
}
LiteScript::Variable LiteScript::_Type_STRING::OLessOrEqual(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() == *this)
            res->GetData<bool>() = (obj1->GetData<String>() <= tmp->GetData<String>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<String>() <= obj2->GetData<String>());
    return res;
}

LiteScript::Variable LiteScript::_Type_STRING::OAddAndAssign(LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() != *this) {
            obj1 = obj1->memory.Create(Type::NIL);
            return obj1;
        }
        obj1->GetData<String>() += tmp->GetData<String>();
        return obj1;
    }
    obj1->GetData<String>() += obj2->GetData<String>();
    return obj1;
}
LiteScript::Variable LiteScript::_Type_STRING::OMultiplyAndAssign(LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != Type::NUMBER) {
        Variable tmp = obj2.Convert(Type::NUMBER);
        if (tmp->GetType() != Type::NUMBER) {
            obj1 = obj1->memory.Create(Type::NIL);
            return obj1;
        }
        obj1->GetData<String>() *= (int)(tmp->GetData<Number>());
        return obj1;
    }
    obj1->GetData<String>() *= (int)(obj2->GetData<Number>());
    return obj1;
}

LiteScript::Variable LiteScript::_Type_STRING::OArray(LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable tmp = obj2.Convert(Type::NUMBER);
    if (tmp->GetType() != Type::NUMBER)
        return obj1->memory.Create(_type_undefined);
    if ((unsigned int)(int)tmp->GetData<Number>() >= obj1->GetData<String>().GetLength())
        return obj1->memory.Create(_type_undefined);
    return obj1->GetData<String>().GetChar(obj1->memory, (unsigned int)(int)tmp->GetData<Number>());
}
LiteScript::Variable LiteScript::_Type_STRING::OMember(LiteScript::Variable& obj, const char * name) const {
    Variable result = obj->GetData<String>().GetMember(obj->memory, name);
    // AJOUTER LE THIS POUR UN CALLBACK
    return result;
}

std::string LiteScript::_Type_STRING::ToString(const LiteScript::Variable& obj) const {
    return (std::string)(obj->GetData<String>());
}