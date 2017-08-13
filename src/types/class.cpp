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

#include <sstream>

#include "../memory/object.hpp"
#include "../memory/variable.hpp"
#include "../memory/memory.hpp"
#include "class.hpp"
#include "internal.hpp"

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

void LiteScript::_Type_CLASS::ODestroy(Object &object) const {
    std::allocator<Class> allocator;
    allocator.destroy(&object.GetData<Class>());
}

LiteScript::Variable LiteScript::_Type_CLASS::OMember(Variable &object_src, const char *member_name) const {
    return object_src->GetData<Class>().GetStaticMember(member_name);
}

std::string LiteScript::_Type_CLASS::ToString(const Variable &object) const {
    std::stringstream ss;
    ss << "{";
    const Class& C = object->GetData<Class>();
    const std::vector<LiteScript::Variable>& inherits = C.GetInherits();
    if (inherits.size() > 0) {
        ss << "inherits:[";
        for (unsigned int i = 0, sz = inherits.size(); i < sz; i++) {
            ss << inherits[i]->ID;
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

void LiteScript::_Type_CLASS::GarbageCollector(const Variable &object, void (Memory::*caller)(unsigned int)) const {
    (object->memory.*caller)(object->ID);
    const Class& C = object->GetData<Class>();
    const std::vector<LiteScript::Variable>& inherits = C.GetInherits();
    for (unsigned int i = 0, sz = inherits.size(); i < sz; i++)
        Variable(inherits[i]).GarbageCollector(caller);
    for (unsigned int i = 0, sz = C.GetStaticCount(); i < sz; i++)
        C.GetStaticMember(i).GarbageCollector(caller);
    for (unsigned int i = 0, sz = C.GetUnstaticCount(); i < sz; i++)
        C.GetUnstaticMember(i).GarbageCollector(caller);
    for (unsigned int i = 0, sz = Class::OperatorType::OP_TYPE_NUMBER; i < sz; i++)
        C.GetOperator(i).GarbageCollector(caller);
}