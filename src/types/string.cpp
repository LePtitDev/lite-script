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

#include <cstdint>
#include <sstream>

#include "../memory/object.hpp"
#include "../memory/variable.hpp"
#include "../memory/memory.hpp"
#include "string.hpp"
#include "undefined.hpp"
#include "character.hpp"
#include "internal.hpp"

LiteScript::_Type_STRING LiteScript::_type_string;

LiteScript::_Type_STRING::_Type_STRING() : Type("STRING") {}

LiteScript::Variable LiteScript::_Type_STRING::Convert(const LiteScript::Variable& obj, const LiteScript::Type& type) const {
    if (type == Type::STRING) {
        Variable res = obj->memory.Create(Type::STRING);
        res->GetData<String>() = obj->GetData<String>();
        return res;
    }
    else {
        return obj->memory.Create(Type::NIL);
    }
}
LiteScript::Object& LiteScript::_Type_STRING::AssignObject(LiteScript::Object& obj) {
    obj.Reassign(*this, sizeof(String));
    std::allocator<String> allocator;
    allocator.construct(&obj.GetData<String>());
    return obj;
}

void LiteScript::_Type_STRING::ODestroy(Object &object) const {
    std::allocator<String> allocator;
    allocator.destroy(&object.GetData<String>());
}

LiteScript::Variable LiteScript::_Type_STRING::OAssign(LiteScript::Variable& src, const LiteScript::Variable& dest) const {
    if (dest->GetType() != *this) {
        dest->GetType().AssignObject(*src);
        src->GetType().OAssign(src, dest);
    }
    else {
        src->GetData<String>() = dest->GetData<String>();
    }
    return Variable(src);
}

LiteScript::Variable LiteScript::_Type_STRING::OAdd(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() != *this)
            return obj1->memory.Create(Type::NIL);
        Variable res = obj1->memory.Create(Type::STRING);
        res->GetData<String>() = obj1->GetData<String>() + tmp->GetData<String>();
        return res;
    }
    Variable res = obj1->memory.Create(Type::STRING);
    res->GetData<String>() = obj1->GetData<String>() + obj2->GetData<String>();
    return res;
}
LiteScript::Variable LiteScript::_Type_STRING::OMultiply(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != Type::NUMBER) {
        Variable tmp = obj2.Convert(Type::NUMBER);
        if (tmp->GetType() != Type::NUMBER)
            return obj1->memory.Create(Type::NIL);
        Variable res = obj1->memory.Create(Type::STRING);
        res->GetData<String>() = obj1->GetData<String>() * (int)(tmp->GetData<Number>());
        return res;
    }
    Variable res = obj1->memory.Create(Type::STRING);
    res->GetData<String>() = obj1->GetData<String>() * (int)(obj2->GetData<Number>());
    return res;
}

LiteScript::Variable LiteScript::_Type_STRING::OEqual(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        if (obj2->GetType() == _type_character)
            res->GetData<bool>() = (obj1->GetData<String>() == String((char32_t)obj2->GetData<Character>()));
        else
            res->GetData<bool>() = false;
    }
    else
        res->GetData<bool>() = (obj1->GetData<String>() == obj2->GetData<String>());
    return res;
}
LiteScript::Variable LiteScript::_Type_STRING::ONotEqual(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        if (obj2->GetType() == _type_character)
            res->GetData<bool>() = (obj1->GetData<String>() != String((char32_t)obj2->GetData<Character>()));
        else
            res->GetData<bool>() = true;
    }
    else
        res->GetData<bool>() = (obj1->GetData<String>() != obj2->GetData<String>());
    return res;
}
LiteScript::Variable LiteScript::_Type_STRING::OGreater(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() == *this)
            res->GetData<bool>() = (obj1->GetData<String>() > tmp->GetData<String>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<String>() > obj2->GetData<String>());
    return res;
}
LiteScript::Variable LiteScript::_Type_STRING::OLess(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() == *this)
            res->GetData<bool>() = (obj1->GetData<String>() < tmp->GetData<String>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<String>() < obj2->GetData<String>());
    return res;
}
LiteScript::Variable LiteScript::_Type_STRING::OGreaterOrEqual(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() == *this)
            res->GetData<bool>() = (obj1->GetData<String>() >= tmp->GetData<String>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<String>() >= obj2->GetData<String>());
    return res;
}
LiteScript::Variable LiteScript::_Type_STRING::OLessOrEqual(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() == *this)
            res->GetData<bool>() = (obj1->GetData<String>() <= tmp->GetData<String>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<String>() <= obj2->GetData<String>());
    return res;
}

LiteScript::Variable LiteScript::_Type_STRING::OAddAndAssign(LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() != *this) {
            obj1 = obj1->memory.Create(Type::NIL);
            return obj1;
        }
        obj1->GetData<String>() += tmp->GetData<String>();
        return obj1;
    }
    obj1->GetData<String>() += obj2->GetData<String>();
    return obj1;
}
LiteScript::Variable LiteScript::_Type_STRING::OMultiplyAndAssign(LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != Type::NUMBER) {
        Variable tmp = obj2.Convert(Type::NUMBER);
        if (tmp->GetType() != Type::NUMBER) {
            obj1 = obj1->memory.Create(Type::NIL);
            return obj1;
        }
        obj1->GetData<String>() *= (int)(tmp->GetData<Number>());
        return obj1;
    }
    obj1->GetData<String>() *= (int)(obj2->GetData<Number>());
    return obj1;
}

LiteScript::Variable LiteScript::_Type_STRING::OArray(LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable tmp = obj2.Convert(Type::NUMBER);
    if (tmp->GetType() != Type::NUMBER)
        return obj1->memory.Create(_type_undefined);
    if ((unsigned int)(int)tmp->GetData<Number>() >= obj1->GetData<String>().GetLength())
        return obj1->memory.Create(_type_undefined);
    Variable res = obj1->GetData<String>().GetChar(obj1->memory, (unsigned int)(int)tmp->GetData<Number>());
    if (res->GetType() == _type_character)
        res->GetData<Character>().obj_string = obj1;
    return res;
}
LiteScript::Variable LiteScript::_Type_STRING::OMember(LiteScript::Variable& obj, const char * name) const {
    Variable result = obj->GetData<String>().GetMember(obj->memory, name);
    if (result->GetType() == Type::CALLBACK)
        result->GetData<Callback>().This = obj;
    return result;
}

std::string LiteScript::_Type_STRING::ToString(const LiteScript::Variable& obj) const {
    std::stringstream ss;
    ss << "\"" << ((std::string)(obj->GetData<String>())).c_str() << "\"";
    return ss.str();
}

void LiteScript::_Type_STRING::Save(std::ostream &stream, Object &object, bool (Memory::*caller)(std::ostream&, unsigned int)) const {
    std::string str = String::ConvertToUTF8(object.GetData<String>().GetData());
    stream << str << (uint8_t)0;
}

void LiteScript::_Type_STRING::Load(std::istream &stream, Object &object, unsigned int (Memory::*caller)(std::istream&)) const {
    object.Reassign(Type::STRING, sizeof(String));
    std::allocator<String> allocator;
    allocator.construct(&object.GetData<String>());
    std::string str;
    unsigned char c;
    while (!stream.eof() && (c = (unsigned char)stream.get()) != 0)
        str += c;
    object.GetData<String>() = String(str);
}