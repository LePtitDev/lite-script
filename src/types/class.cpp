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

#include "class.hpp"

LiteScript::_Type_CLASS LiteScript::_type_class;

LiteScript::_Type_CLASS::_Type_CLASS() : Type("CLASS") {}

LiteScript::Object & LiteScript::_Type_CLASS::AssignObject(Object &obj) {
    obj.Reassign(*this, sizeof(Class));
    std::allocator<Class> allocator;
    allocator.construct(&obj.GetData<Class>(), obj.memory);
    return obj;
}

LiteScript::Variable LiteScript::_Type_CLASS::OAssign(Variable &src, const Variable &dest) const {
    dest->GetType().AssignObject(*src);
    src->GetType().OAssign(src, dest);
    return Variable(src);
}

void LiteScript::_Type_CLASS::OCall(Variable &object, std::vector<Variable> &args) const {
    object->GetData<Class>().CreateElement(args);
}

std::string LiteScript::_Type_CLASS::ToString(const Variable &object) const {
    std::stringstream ss;
    ss << "class";
    return ss.str();
    // A COMPLETER
}