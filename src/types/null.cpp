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

LiteScript::Object LiteScript::_Type_NIL::CreateObject() {
    return Object(*this, 0);
}

LiteScript::Object LiteScript::_Type_NIL::Convert(const LiteScript::Object& obj, const LiteScript::Type& type) const {
    if (type == Type::STRING) {
        Object res = Type::STRING.CreateObject();
        res.GetData<String>() = String((std::string)(obj));
        return res;
    }
    return obj;
}
LiteScript::Object& LiteScript::_Type_NIL::AssignObject(LiteScript::Object& obj) {
    return obj.Reassign(*this, 0);
}

std::string LiteScript::_Type_NIL::ToString(const Object &) const {
    return "null";
}