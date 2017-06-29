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

void LiteScript::_Type_NIL::CreateObject(LiteScript::Object& obj) {
    obj.Reassign(*this, 0);
}

LiteScript::Variable LiteScript::_Type_NIL::Convert(const LiteScript::Variable& obj, const LiteScript::Type& type) const {
    if (type == Type::STRING) {
        Variable res = obj->memory.Create(Type::STRING);
        res->GetData<String>() = String((std::string)(obj));
        return res;
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

std::string LiteScript::_Type_NIL::ToString(const Variable &) const {
    return "null";
}