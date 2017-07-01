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
/////////////////////////////////////////////////////////////////////

#include "object.hpp"

LiteScript::_Type_OBJECT LiteScript::_type_object;

LiteScript::_Type_OBJECT::_Type_OBJECT() : Type("OBJECT") {}

void LiteScript::_Type_OBJECT::CreateObject(Object &obj) {
    obj.Reassign(*this, sizeof(VObject));
    std::allocator<VObject> allocator;
    allocator.construct(&obj.GetData<VObject>(), obj.memory);
}

LiteScript::Variable LiteScript::_Type_OBJECT::Convert(const Variable &object, const Type &type) const {
    if (type == Type::STRING) {
        Variable result = object->memory.Create(Type::STRING);
        result->GetData<String>() = String(this->ToString(object));
        return result;
    }
    else if (type == Type::OBJECT) {
        Variable result = object->memory.Create(Type::OBJECT);
        result->GetData<VObject>() = object->GetData<VObject>();
        return result;
    }
    else {
        return object->memory.Create(Type::NIL);
    }
}

LiteScript::Object & LiteScript::_Type_OBJECT::AssignObject(Object &object) {
    object.Reassign(*this, sizeof(VObject));
    std::allocator<VObject> allocator;
    allocator.construct(&object.GetData<VObject>(), object.memory);
    return object;
}

LiteScript::Variable LiteScript::_Type_OBJECT::OAssign(Variable &obj1, const Variable &obj2) const {
    if (obj2->GetType() == *this) {
        obj1->GetData<VObject>() = obj2->GetData<VObject>();
    }
    else {
        obj2->GetType().AssignObject(*obj1);
        obj2->GetType().OAssign(obj1, obj2);
    }
    return Variable(obj1);
}

LiteScript::Variable LiteScript::_Type_OBJECT::OEqual(const Variable &obj1, const Variable &obj2) const {
    Variable result = obj1->memory.Create(Type::BOOLEAN);
    result->GetData<bool>() = (obj1->ID == obj2->ID);
    return result;
}

LiteScript::Variable LiteScript::_Type_OBJECT::ONotEqual(const Variable &obj1, const Variable &obj2) const {
    Variable result = obj1->memory.Create(Type::BOOLEAN);
    result->GetData<bool>() = (obj1->ID != obj2->ID);
    return result;
}

LiteScript::Variable LiteScript::_Type_OBJECT::OArray(Variable &obj1, const Variable &obj2) const {
    if (obj2->GetType() == Type::NUMBER)
        return obj1->GetData<VObject>()[(unsigned int)(int)obj2->GetData<Number>()];
    else if (obj2->GetType() == Type::STRING)
        return obj1->GetData<VObject>()[((std::string)(obj2->GetData<String>())).c_str()];
    else
        return obj1->memory.Create(Type::UNDEFINED);
}

LiteScript::Variable LiteScript::_Type_OBJECT::OMember(Variable &object, const char *name) const {
    return object->GetData<VObject>()[name];
}

std::string LiteScript::_Type_OBJECT::ToString(const Variable &object) const {
    std::stringstream ss;
    ss << "{";
    const VObject& obj = object->GetData<VObject>();
    for (unsigned int i = 0, sz = obj.UnamedCount(); i < sz; i++) {
        if (obj.ExistUnamed(i)) {
            ss << "[" << i << "]:" << ((std::string) (obj.ConstantGet(i))).c_str();
            if (i < sz - 1)
                ss << ",";
        }
    }
    if (obj.UnamedCount() > 0)
        ss << ",";
    for (unsigned int i = 0, sz = obj.NamedCount(); i < sz; i++) {
        Variable v = obj.GetNamedVariable(i);
        if (v->GetType() != Type::UNDEFINED) {
            ss << "[" << obj.GetNamedKey(i) << "]:" << ((std::string) (v)).c_str();
            if (i < sz - 1)
                ss << ",";
        }
    }
    ss << "}";
    return ss.str();
}
