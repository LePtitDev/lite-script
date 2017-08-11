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

#include "null.hpp"

LiteScript::_Type_NIL LiteScript::_type_nil;

LiteScript::_Type_NIL::_Type_NIL() : Type("NULL") {}

LiteScript::Variable LiteScript::_Type_NIL::Convert(const LiteScript::Variable& obj, const LiteScript::Type& type) const {
    if (type == Type::STRING) {
        Variable res = obj->memory.Create(Type::STRING);
        res->GetData<String>() = String((std::string)(obj));
        return res;
    }
    else if (type == Type::BOOLEAN) {
        Variable v = obj->memory.Create(Type::BOOLEAN);
        v->GetData<bool>() = false;
        return v;
    }
    return obj;
}
LiteScript::Object& LiteScript::_Type_NIL::AssignObject(LiteScript::Object& obj) {
    return obj.Reassign(*this, 0);
}

LiteScript::Variable LiteScript::_Type_NIL::OAssign(Variable &src, const Variable &dest) const {
    if (dest->GetType() != *this) {
        dest->GetType().AssignObject(*src);
        src->GetType().OAssign(src, dest);
    }
    return Variable(src);
}

LiteScript::Variable LiteScript::_Type_NIL::OEqual(const Variable &obj1, const Variable &obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    res->GetData<bool>() = (obj2->GetType() == *this);
    return res;
}

LiteScript::Variable LiteScript::_Type_NIL::ONotEqual(const Variable &obj1, const Variable &obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    res->GetData<bool>() = (obj2->GetType() != *this);
    return res;
}

LiteScript::Variable LiteScript::_Type_NIL::OLogicalNot(const Variable &object) const {
    Variable res = object->memory.Create(Type::BOOLEAN);
    res->GetData<bool>() = true;
    return res;
}

LiteScript::Variable LiteScript::_Type_NIL::OBitwiseOr(const Variable &obj1, const Variable &obj2) const {
    return Variable(obj2);
}

std::string LiteScript::_Type_NIL::ToString(const Variable &) const {
    return "null";
}