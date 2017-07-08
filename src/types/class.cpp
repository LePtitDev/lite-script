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

LiteScript::Variable LiteScript::_Type_CLASS::OCall(Variable &object, std::vector<Variable> &args) const {
    return object->GetData<Class>().CreateElement(args);
}

std::string LiteScript::_Type_CLASS::ToString(const Variable &object) const {
    std::stringstream ss;
    ss << "{";
    const Class& C = object->GetData<Class>();
    const std::vector<LiteScript::Variable>& inherits = C.GetInherits();
    if (inherits.size() > 0) {
        ss << "inherits:[";
        for (unsigned int i = 0, sz = inherits.size(); i < sz; i++) {
            ss << inherits[i];
            if (i < sz - 1)
                ss << ",";
        }
        ss << "]";
        if (C.GetStaticCount() > 0 || C.GetUnstaticCount() > 0)
            ss << ",";
    }
    if (C.GetStaticCount() > 0) {
        ss << "static:{";
        for (unsigned int i = 0, sz = C.GetStaticCount(); i < sz; i++) {
            ss << C.GetStaticName(i) << ":" << ((std::string)C.GetStaticMember(i)).c_str();
            if (i < sz - 1)
                ss << ",";
        }
        ss << "}";
        if (C.GetUnstaticCount() > 0)
            ss << ",";
    }
    if (C.GetUnstaticCount() > 0) {
        ss << "unstatic:{";
        int construct = C.GetConstructorIndex();
        for (unsigned int i = 0, sz = C.GetUnstaticCount(); i < sz; i++) {
            ss << C.GetUnstaticName(i) << ((int)i == construct ? "(constructor)" : "") << ":" << ((std::string)C.GetUnstaticMember(i)).c_str();
            if (i < sz - 1)
                ss << ",";
        }
        ss << "}";
    }
    ss << "}";
    return ss.str();
}