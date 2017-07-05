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

#include "class_object.hpp"

LiteScript::_Type_CLASS_OBJECT LiteScript::_type_class_object;

LiteScript::_Type_CLASS_OBJECT::_Type_CLASS_OBJECT() : Type("CLASS OBJECT") {}

LiteScript::Object & LiteScript::_Type_CLASS_OBJECT::AssignObject(Object &obj) {
    obj.Reassign(*this, sizeof(ClassObject));
    std::allocator<ClassObject> allocator;
    allocator.construct(&obj.GetData<ClassObject>());
    return obj;
}

std::string LiteScript::_Type_CLASS_OBJECT::ToString(const Variable &object) const {
    std::stringstream ss;
    const ClassObject& co = object->GetData<ClassObject>();
    ss << "{";
    for (unsigned int i = 0, sz = co.GetMemberCount(); i < sz; i++) {
        ss << co.GetMemberName(i) << ((std::string)(co.GetMemberVariable(i))).c_str();
        if (i < sz - 1)
            ss << ",";
    }
    ss << "}";
    return ss.str();
}