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

#include "array.hpp"

LiteScript::_Type_ARRAY LiteScript::_type_array;

LiteScript::_Type_ARRAY::_Type_ARRAY() : Type("OBJECT") {}

LiteScript::Variable LiteScript::_Type_ARRAY::Convert(const Variable &object, const Type &type) const {
    if (type == Type::STRING) {
        Variable result = object->memory.Create(Type::STRING);
        result->GetData<String>() = String(this->ToString(object));
        return result;
    }
    else if (type == Type::ARRAY) {
        Variable result = object->memory.Create(Type::ARRAY);
        result->GetData<Array>() = object->GetData<Array>();
        return result;
    }
    else {
        return object->memory.Create(Type::NIL);
    }
}

LiteScript::Object & LiteScript::_Type_ARRAY::AssignObject(Object &object) {
    object.Reassign(*this, sizeof(Array));
    std::allocator<Array> allocator;
    allocator.construct(&object.GetData<Array>(), object.memory);
    return object;
}

LiteScript::Variable LiteScript::_Type_ARRAY::OAssign(Variable &obj1, const Variable &obj2) const {
    if (obj2->GetType() == *this) {
        obj1->GetData<Array>() = obj2->GetData<Array>();
    }
    else {
        obj2->GetType().AssignObject(*obj1);
        obj2->GetType().OAssign(obj1, obj2);
    }
    return Variable(obj1);
}

LiteScript::Variable LiteScript::_Type_ARRAY::OEqual(const Variable &obj1, const Variable &obj2) const {
    Variable result = obj1->memory.Create(Type::BOOLEAN);
    result->GetData<bool>() = (obj1->ID == obj2->ID);
    return result;
}

LiteScript::Variable LiteScript::_Type_ARRAY::ONotEqual(const Variable &obj1, const Variable &obj2) const {
    Variable result = obj1->memory.Create(Type::BOOLEAN);
    result->GetData<bool>() = (obj1->ID != obj2->ID);
    return result;
}

LiteScript::Variable LiteScript::_Type_ARRAY::OArray(Variable &obj1, const Variable &obj2) const {
    if (obj2->GetType() == Type::NUMBER)
        return obj1->GetData<Array>()[(unsigned int)(int)obj2->GetData<Number>()];
    else if (obj2->GetType() == Type::STRING)
        return obj1->GetData<Array>()[((std::string)(obj2->GetData<String>())).c_str()];
    else
        return obj1->memory.Create(Type::UNDEFINED);
}

LiteScript::Variable LiteScript::_Type_ARRAY::OMember(Variable &object, const char *name) const {
    return object->GetData<Array>()[name];
}

std::string LiteScript::_Type_ARRAY::ToString(const Variable &object) const {
    std::stringstream ss;
    ss << "{";
    const Array& obj = object->GetData<Array>();
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
