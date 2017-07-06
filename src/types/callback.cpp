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

#include "callback.hpp"

LiteScript::_Type_CALLBACK LiteScript::_type_callback;

LiteScript::_Type_CALLBACK::_Type_CALLBACK() : Type("CALLBACK") {}

LiteScript::Variable LiteScript::_Type_CALLBACK::Convert(const Variable &object, const Type &type) const {
    if (type == Type::BOOLEAN) {
        Variable result = object->memory.Create(Type::BOOLEAN);
        result->GetData<bool>() = true;
        return result;
    }
    else if (type == Type::NUMBER) {
        Variable result = object->memory.Create(Type::NUMBER);
        result->GetData<Number>() = Number((int)object->ID);
        return result;
    }
    else if (type == Type::STRING) {
        Variable result = object->memory.Create(Type::STRING);
        std::stringstream ss;
        ss << object->ID;
        result->GetData<String>() = String(ss.str());
        return result;
    }
    else {
        return object->memory.Create(Type::NIL);
    }
}

LiteScript::Object & LiteScript::_Type_CALLBACK::AssignObject(Object &object) {
    object.Reassign(*this, sizeof(Callback));
    std::allocator<Callback> allocator;
    allocator.construct(&object.GetData<Callback>());
}

LiteScript::Variable LiteScript::_Type_CALLBACK::OAssign(Variable &obj1, const Variable &obj2) const {
    if (obj2->GetType() != *this) {
        obj2->GetType().AssignObject(*obj1);
        obj1->GetType().OAssign(obj1, obj2);
    }
    else {
        obj1->GetData<Callback>() = obj2->GetData<Callback>();
    }
    return Variable(obj1);
}

LiteScript::Variable LiteScript::_Type_CALLBACK::OEqual(const Variable &obj1, const Variable &obj2) const {
    Variable result = obj1->memory.Create(Type::BOOLEAN);
    result->GetData<bool>() = (obj1->GetType() == obj2->GetType() && obj1->GetData<Callback>() == obj2->GetData<Callback>());
    return result;
}

LiteScript::Variable LiteScript::_Type_CALLBACK::ONotEqual(const Variable &obj1, const Variable &obj2) const {
    Variable result = obj1->memory.Create(Type::BOOLEAN);
    result->GetData<bool>() = (obj1->GetType() == obj2->GetType() && obj1->GetData<Callback>() != obj2->GetData<Callback>());
    return result;
}

LiteScript::Variable LiteScript::_Type_CALLBACK::OLogicalNot(const Variable &object) const {
    Variable result = object->memory.Create(Type::BOOLEAN);
    result->GetData<bool>() = false;
    return result;
}

LiteScript::Variable LiteScript::_Type_CALLBACK::OBitwiseOr(const Variable &obj1, const Variable &obj2) const {
    return Variable(obj1);
}

LiteScript::Variable LiteScript::_Type_CALLBACK::OCall(Variable &object, std::vector<Variable> &args) const {
    return (object->GetData<Callback>())(args);
}

std::string LiteScript::_Type_CALLBACK::ToString(const Variable &object) const {
    std::stringstream ss;
    ss << object->ID;
    return ss.str();
}