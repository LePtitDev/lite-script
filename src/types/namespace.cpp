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

#include "namespace.hpp"

LiteScript::_Type_NAMESPACE LiteScript::_type_namespace;

LiteScript::_Type_NAMESPACE::_Type_NAMESPACE() : Type("NAMESPACE") {}

LiteScript::Object& LiteScript::_Type_NAMESPACE::AssignObject(Object &obj) {
    obj.Reassign(*this, sizeof(Namespace));
    std::allocator<Namespace> allocator;
    allocator.construct(&obj.GetData<Namespace>(), obj.memory);
    return obj;
}

LiteScript::Variable LiteScript::_Type_NAMESPACE::OMember(Variable &object, const char *name) const {
    return object->GetData<Namespace>()[name];
}

std::string LiteScript::_Type_NAMESPACE::ToString(const Variable &object) const {
    std::stringstream ss;
    ss << "{";
    const Namespace& nsp = object->GetData<Namespace>();
    for (unsigned int i = 0, sz = nsp.Count(); i < sz; i++) {
        ss << nsp.GetKey(i) << ":" << ((std::string)(nsp.GetVariable(i))).c_str();
        if (i < sz - 1)
            ss << ",";
    }
    ss << "}";
    return ss.str();
}