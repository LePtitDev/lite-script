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

#include "string.hpp"

LiteScript::_Type_STRING LiteScript::_type_string;

LiteScript::_Type_STRING::_Type_STRING() : Type("STRING") {};

LiteScript::Object LiteScript::_Type_STRING::CreateObject() {
    Object res(*this, sizeof(String));
    std::allocator<String> allocator;
    allocator.construct(&res.GetData<String>());
    return res;
}

LiteScript::Object LiteScript::_Type_STRING::Convert(const LiteScript::Object& obj, const LiteScript::Type& type) const {
    if (type == Type::STRING) {
        Object res = Type::STRING.CreateObject();
        res.GetData<String>() = obj.GetData<String>();
        return res;
    }
    else {
        return Type::NIL.CreateObject();
    }
}
LiteScript::Object& LiteScript::_Type_STRING::AssignObject(LiteScript::Object& obj) {
    obj.Reassign(*this, sizeof(String));
    std::allocator<String> allocator;
    allocator.construct(&obj.GetData<String>());
    return obj;
}

LiteScript::Object& LiteScript::_Type_STRING::OAssign(LiteScript::Object& src, const LiteScript::Object& dest) const {
    if (dest.GetType() != *this) {
        dest.GetType().AssignObject(src);
        src.GetType().OAssign(src, dest);
    }
    else {
        src.GetData<String>() = dest.GetData<String>();
    }
    return src;
}

LiteScript::Object LiteScript::_Type_STRING::OAdd(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() != *this)
            return Type::NIL.CreateObject();
        Object res = Type::STRING.CreateObject();
        res.GetData<String>() = obj1.GetData<String>() + tmp.GetData<String>();
        return res;
    }
    Object res = Type::STRING.CreateObject();
    res.GetData<String>() = obj1.GetData<String>() + obj2.GetData<String>();
    return res;
}
LiteScript::Object LiteScript::_Type_STRING::OMultiply(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != Type::NUMBER) {
        Object tmp = obj2.Convert(Type::NUMBER);
        if (tmp.GetType() != Type::NUMBER)
            return Type::NIL.CreateObject();
        Object res = Type::STRING.CreateObject();
        res.GetData<String>() = obj1.GetData<String>() * (int)(tmp.GetData<Number>());
        return res;
    }
    Object res = Type::STRING.CreateObject();
    res.GetData<String>() = obj1.GetData<String>() * (int)(obj2.GetData<Number>());
    return res;
}

LiteScript::Object LiteScript::_Type_STRING::OEqual(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<String>() == tmp.GetData<String>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<String>() == obj2.GetData<String>());
    return res;
}
LiteScript::Object LiteScript::_Type_STRING::ONotEqual(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<String>() != tmp.GetData<String>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<String>() != obj2.GetData<String>());
    return res;
}
LiteScript::Object LiteScript::_Type_STRING::OGreater(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<String>() > tmp.GetData<String>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<String>() > obj2.GetData<String>());
    return res;
}
LiteScript::Object LiteScript::_Type_STRING::OLess(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<String>() < tmp.GetData<String>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<String>() < obj2.GetData<String>());
    return res;
}
LiteScript::Object LiteScript::_Type_STRING::OGreaterOrEqual(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<String>() >= tmp.GetData<String>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<String>() >= obj2.GetData<String>());
    return res;
}
LiteScript::Object LiteScript::_Type_STRING::OLessOrEqual(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<String>() <= tmp.GetData<String>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<String>() <= obj2.GetData<String>());
    return res;
}

LiteScript::Object& LiteScript::_Type_STRING::OAddAndAssign(LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() != *this) {
            obj1 = Type::NIL.CreateObject();
            return obj1;
        }
        obj1.GetData<String>() += tmp.GetData<String>();
        return obj1;
    }
    obj1.GetData<String>() += obj2.GetData<String>();
    return obj1;
}
LiteScript::Object& LiteScript::_Type_STRING::OMultiplyAndAssign(LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != Type::NUMBER) {
        Object tmp = obj2.Convert(Type::NUMBER);
        if (tmp.GetType() != Type::NUMBER) {
            obj1 = Type::NIL.CreateObject();
            return obj1;
        }
        obj1.GetData<String>() *= (int)(tmp.GetData<Number>());
        return obj1;
    }
    obj1.GetData<String>() *= (int)(obj2.GetData<Number>());
    return obj1;
}

LiteScript::Object& LiteScript::_Type_STRING::OArray(LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object tmp = obj2.Convert(Type::NUMBER);
    if (tmp.GetType() != Type::NUMBER)
        return Object::UNDEFINED;
    if ((unsigned int)(int)tmp.GetData<Number>() >= obj1.GetData<String>().GetLength())
        return Object::UNDEFINED;
    return obj1.GetData<String>().GetChar((unsigned int)(int)tmp.GetData<Number>());
}
LiteScript::Object& LiteScript::_Type_STRING::OMember(LiteScript::Object& obj, const char * name) const {
    Object& result = obj.GetData<String>().GetMember(name);
    // AJOUTER LE THIS POUR UN CALLBACK
    return result;
}

std::string LiteScript::_Type_STRING::ToString(const LiteScript::Object& obj) const {
    return (std::string)(obj.GetData<String>());
}