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

#include "undefined.hpp"

LiteScript::_Type_UNDEFINED LiteScript::_type_undefined;

LiteScript::_Type_UNDEFINED::_Type_UNDEFINED() : Type("UNDEFINED") {}

void LiteScript::_Type_UNDEFINED::CreateObject(LiteScript::Object& obj) {
    obj.Reassign(_type_undefined, 0);
}

LiteScript::Variable LiteScript::_Type_UNDEFINED::OAssign(Variable &src, const Variable &dest) const {
    if (dest->GetType() != *this) {
        dest->GetType().AssignObject(*src);
        src->GetType().OAssign(src, dest);
    }
    return Variable(src);
}

LiteScript::Variable LiteScript::_Type_UNDEFINED::OEqual(const Variable &obj1, const Variable &obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    res->GetData<bool>() = (obj2->GetType() == *this);
    return res;
}

LiteScript::Variable LiteScript::_Type_UNDEFINED::ONotEqual(const Variable &obj1, const Variable &obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    res->GetData<bool>() = (obj2->GetType() != *this);
    return res;
}

LiteScript::Variable LiteScript::_Type_UNDEFINED::OLogicalNot(const Variable &object) const {
    Variable res = object->memory.Create(Type::BOOLEAN);
    res->GetData<bool>() = true;
    return res;
}

LiteScript::Variable LiteScript::_Type_UNDEFINED::OBitwiseOr(const Variable &obj1, const Variable &obj2) const {
    return Variable(obj2);
}

std::string LiteScript::_Type_UNDEFINED::ToString(const LiteScript::Variable& obj) const {
    return "undefined";
}
