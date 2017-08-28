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
#include "character.hpp"
#include "internal.hpp"
#include "../streamer.hpp"

LiteScript::_Type_CHARACTER LiteScript::_type_character;

LiteScript::_Type_CHARACTER::_Type_CHARACTER() : Type("CHARACTER") {}

LiteScript::Object& LiteScript::_Type_CHARACTER::AssignObject(Object& obj) {
    obj.Reassign(*this, sizeof(Character));
    return obj;
}

LiteScript::Variable LiteScript::_Type_CHARACTER::Convert(const LiteScript::Variable& obj, const LiteScript::Type& type) const {
    if (type == Type::STRING) {
        Variable res = obj->memory.Create(Type::STRING);
        res->GetData<String>() = String(obj->GetData<Character>());
        return res;
    }
    else {
        return obj->memory.Create(Type::NIL);
    }
}

void LiteScript::_Type_CHARACTER::ODestroy(Object &object) const {
    std::allocator<Character> allocator;
    allocator.destroy(&object.GetData<Character>());
}

LiteScript::Variable LiteScript::_Type_CHARACTER::OAssign(LiteScript::Variable& src, const LiteScript::Variable& dest) const {
    if (dest->GetType() != Type::STRING) {
        Variable tmp = dest.Convert(Type::STRING);
        if (tmp->GetType() != Type::STRING)
            return Variable(src);
        src->GetData<Character>() = tmp->GetData<String>();
    }
    else {
        src->GetData<Character>() = dest->GetData<String>();
    }
    return Variable(src);
}

LiteScript::Variable LiteScript::_Type_CHARACTER::OAdd(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != Type::STRING) {
        Variable tmp = obj2.Convert(Type::STRING);
        if (tmp->GetType() != Type::STRING)
            return obj1->memory.Create(Type::NIL);
        Variable res = obj1->memory.Create(Type::STRING);
        res->GetData<String>() = obj1->GetData<Character>() + tmp->GetData<String>();
        return res;
    }
    Variable res = obj1->memory.Create(Type::STRING);
    res->GetData<String>() = obj1->GetData<Character>() + obj2->GetData<String>();
    return res;
}
LiteScript::Variable LiteScript::_Type_CHARACTER::OMultiply(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != Type::NUMBER) {
        Variable tmp = obj2.Convert(Type::NUMBER);
        if (tmp->GetType() != Type::NUMBER)
            return obj1->memory.Create(Type::NIL);
        Variable res = obj1->memory.Create(Type::STRING);
        res->GetData<String>() = obj1->GetData<Character>() * (unsigned int)(int)tmp->GetData<Number>();
        return res;
    }
    Variable res = obj1->memory.Create(Type::STRING);
    res->GetData<String>() = obj1->GetData<Character>() * (unsigned int)(int)obj2->GetData<Number>();
    return res;
}

LiteScript::Variable LiteScript::_Type_CHARACTER::OEqual(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != Type::STRING) {
        Variable tmp = obj2.Convert(Type::STRING);
        if (tmp->GetType() == Type::STRING)
            res->GetData<bool>() = (obj1->GetData<Character>() == tmp->GetData<String>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<Character>() == obj2->GetData<String>());
    return res;
}
LiteScript::Variable LiteScript::_Type_CHARACTER::ONotEqual(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != Type::STRING) {
        Variable tmp = obj2.Convert(Type::STRING);
        if (tmp->GetType() == Type::STRING)
            res->GetData<bool>() = (obj1->GetData<Character>() != tmp->GetData<String>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<Character>() != obj2->GetData<String>());
    return res;
}
LiteScript::Variable LiteScript::_Type_CHARACTER::OGreater(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != Type::STRING) {
        Variable tmp = obj2.Convert(Type::STRING);
        if (tmp->GetType() == Type::STRING)
            res->GetData<bool>() = (obj1->GetData<Character>() > tmp->GetData<String>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<Character>() > obj2->GetData<String>());
    return res;
}
LiteScript::Variable LiteScript::_Type_CHARACTER::OLess(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != Type::STRING) {
        Variable tmp = obj2.Convert(Type::STRING);
        if (tmp->GetType() == Type::STRING)
            res->GetData<bool>() = (obj1->GetData<Character>() < tmp->GetData<String>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<Character>() < obj2->GetData<String>());
    return res;
}
LiteScript::Variable LiteScript::_Type_CHARACTER::OGreaterOrEqual(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != Type::STRING) {
        Variable tmp = obj2.Convert(Type::STRING);
        if (tmp->GetType() == Type::STRING)
            res->GetData<bool>() = (obj1->GetData<Character>() >= tmp->GetData<String>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<Character>() >= obj2->GetData<String>());
    return res;
}
LiteScript::Variable LiteScript::_Type_CHARACTER::OLessOrEqual(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != Type::STRING) {
        Variable tmp = obj2.Convert(Type::STRING);
        if (tmp->GetType() == Type::STRING)
            res->GetData<bool>() = (obj1->GetData<Character>() <= tmp->GetData<String>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<Character>() <= obj2->GetData<String>());
    return res;
}

LiteScript::Variable LiteScript::_Type_CHARACTER::OAddAndAssign(LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != Type::STRING) {
        Variable tmp = obj2.Convert(Type::STRING);
        if (tmp->GetType() == Type::STRING) {
            obj1->GetData<Character>() += tmp->GetData<String>();
            return obj1;
        }
        obj1->GetData<Character>() += String("");
        return obj1;
    }
    obj1->GetData<Character>() += obj2->GetData<String>();
    return obj1;
}
LiteScript::Variable LiteScript::_Type_CHARACTER::OMultiplyAndAssign(LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != Type::NUMBER) {
        Variable tmp = obj2.Convert(Type::NUMBER);
        if (tmp->GetType() == Type::NUMBER) {
            obj1->GetData<Character>() *= (unsigned int) (int) tmp->GetData<Number>();
            return obj1;
        }
        obj1->GetData<Character>() += String("");
        return obj1;
    }
    obj1->GetData<Character>() *= (unsigned int)(int)obj2->GetData<Number>();
    return obj1;
}

std::string LiteScript::_Type_CHARACTER::ToString(const LiteScript::Variable& obj) const {
    return String(obj->GetData<Character>());
}

void LiteScript::_Type_CHARACTER::Save(std::ostream &stream, Object &object, bool (Memory::*caller)(std::ostream&, unsigned int)) const {
    Character& C = object.GetData<Character>();
    OStreamer::Write<unsigned int>(stream, C.Index);
    (object.memory.*caller)(stream, (*C.obj_string)->ID);
}

void LiteScript::_Type_CHARACTER::Load(std::istream &stream, Object &object, unsigned int (Memory::*caller)(std::istream&)) const {
    unsigned int index = IStreamer::Read<unsigned int>(stream);
    Variable V = *object.memory.GetVariable((object.memory.*caller)(stream));
    object.Reassign(_type_character, sizeof(Character));
    std::allocator<Character> allocator;
    allocator.construct(&object.GetData<Character>(), V->GetData<String>(), index);
    object.GetData<Character>().obj_string = V;
}

void LiteScript::_Type_CHARACTER::GarbageCollector(const Variable &object, bool (Memory::*caller)(unsigned int)) const {
    (object->memory.*caller)(object->ID);
    const Character& C = object->GetData<Character>();
    if (!C.obj_string.isNull)
        (object->memory.*caller)((*C.obj_string)->ID);
}