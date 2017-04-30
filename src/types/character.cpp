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

#include "character.hpp"

LiteScript::_Type_CHARACTER LiteScript::_type_character;

LiteScript::_Type_CHARACTER::_Type_CHARACTER() : Type("CHARACTER") {}

LiteScript::Object LiteScript::_Type_CHARACTER::CreateObject() {
    return Type::NIL.CreateObject();
}
LiteScript::Object LiteScript::_Type_CHARACTER::Convert(const LiteScript::Object& obj, const LiteScript::Type& type) const {
    if (type == Type::STRING) {
        Object res = Type::STRING.CreateObject();
        res.GetData<String>() = String(obj.GetData<Character>());
        return res;
    }
    else {
        return Type::NIL.CreateObject();
    }
}

LiteScript::Object& LiteScript::_Type_CHARACTER::OAssign(LiteScript::Object& src, const LiteScript::Object& dest) const {
    if (dest.GetType() != Type::STRING) {
        Object tmp = dest.Convert(Type::STRING);
        if (tmp.GetType() != Type::STRING)
            return src;
        src.GetData<Character>() = tmp.GetData<String>();
    }
    else {
        src.GetData<Character>() = dest.GetData<String>();
    }
    return src;
}

LiteScript::Object LiteScript::_Type_CHARACTER::OAdd(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != Type::STRING) {
        Object tmp = obj2.Convert(Type::STRING);
        if (tmp.GetType() != Type::STRING)
            return Type::NIL.CreateObject();
        Object res = Type::STRING.CreateObject();
        res.GetData<String>() = obj1.GetData<Character>() + tmp.GetData<String>();
        return res;
    }
    Object res = Type::STRING.CreateObject();
    res.GetData<String>() = obj1.GetData<Character>() + obj2.GetData<String>();
    return res;
}
LiteScript::Object LiteScript::_Type_CHARACTER::OMultiply(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != Type::NUMBER) {
        Object tmp = obj2.Convert(Type::NUMBER);
        if (tmp.GetType() != Type::NUMBER)
            return Type::NIL.CreateObject();
        Object res = Type::STRING.CreateObject();
        res.GetData<String>() = obj1.GetData<Character>() * (unsigned int)(int)tmp.GetData<Number>();
        return res;
    }
    Object res = Type::STRING.CreateObject();
    res.GetData<String>() = obj1.GetData<Character>() * (unsigned int)(int)obj2.GetData<Number>();
    return res;
}

LiteScript::Object LiteScript::_Type_CHARACTER::OEqual(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != Type::STRING) {
        Object tmp = obj2.Convert(Type::STRING);
        if (tmp.GetType() == Type::STRING)
            res.GetData<bool>() = (obj1.GetData<Character>() == tmp.GetData<String>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<Character>() == obj2.GetData<String>());
    return res;
}
LiteScript::Object LiteScript::_Type_CHARACTER::ONotEqual(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != Type::STRING) {
        Object tmp = obj2.Convert(Type::STRING);
        if (tmp.GetType() == Type::STRING)
            res.GetData<bool>() = (obj1.GetData<Character>() != tmp.GetData<String>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<Character>() != obj2.GetData<String>());
    return res;
}
LiteScript::Object LiteScript::_Type_CHARACTER::OGreater(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != Type::STRING) {
        Object tmp = obj2.Convert(Type::STRING);
        if (tmp.GetType() == Type::STRING)
            res.GetData<bool>() = (obj1.GetData<Character>() > tmp.GetData<String>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<Character>() > obj2.GetData<String>());
    return res;
}
LiteScript::Object LiteScript::_Type_CHARACTER::OLess(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != Type::STRING) {
        Object tmp = obj2.Convert(Type::STRING);
        if (tmp.GetType() == Type::STRING)
            res.GetData<bool>() = (obj1.GetData<Character>() < tmp.GetData<String>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<Character>() < obj2.GetData<String>());
    return res;
}
LiteScript::Object LiteScript::_Type_CHARACTER::OGreaterOrEqual(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != Type::STRING) {
        Object tmp = obj2.Convert(Type::STRING);
        if (tmp.GetType() == Type::STRING)
            res.GetData<bool>() = (obj1.GetData<Character>() >= tmp.GetData<String>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<Character>() >= obj2.GetData<String>());
    return res;
}
LiteScript::Object LiteScript::_Type_CHARACTER::OLessOrEqual(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != Type::STRING) {
        Object tmp = obj2.Convert(Type::STRING);
        if (tmp.GetType() == Type::STRING)
            res.GetData<bool>() = (obj1.GetData<Character>() <= tmp.GetData<String>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<Character>() <= obj2.GetData<String>());
    return res;
}

LiteScript::Object& LiteScript::_Type_CHARACTER::OAddAndAssign(LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != Type::STRING) {
        Object tmp = obj2.Convert(Type::STRING);
        if (tmp.GetType() == Type::STRING) {
            obj1.GetData<Character>() += tmp.GetData<String>();
            return obj1;
        }
        obj1.GetData<Character>() += String("");
        return obj1;
    }
    obj1.GetData<Character>() += obj2.GetData<String>();
    return obj1;
}
LiteScript::Object& LiteScript::_Type_CHARACTER::OMultiplyAndAssign(LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != Type::NUMBER) {
        Object tmp = obj2.Convert(Type::NUMBER);
        if (tmp.GetType() == Type::NUMBER) {
            obj1.GetData<Character>() *= (unsigned int) (int) tmp.GetData<Number>();
            return obj1;
        }
        obj1.GetData<Character>() += String("");
        return obj1;
    }
    obj1.GetData<Character>() *= (unsigned int)(int)obj2.GetData<Number>();
    return obj1;
}

std::string LiteScript::_Type_CHARACTER::ToString(const LiteScript::Object& obj) const {
    return String(obj.GetData<Character>());
}