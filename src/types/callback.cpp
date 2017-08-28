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
#include "callback.hpp"
#include "internal.hpp"
#include "../streamer.hpp"

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
    allocator.construct(&object.GetData<Callback>(), object.memory);
    return object;
}

void LiteScript::_Type_CALLBACK::ODestroy(Object &object) const {
    std::allocator<Callback> allocator;
    allocator.destroy(&object.GetData<Callback>());
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

LiteScript::Variable LiteScript::_Type_CALLBACK::OCall(Variable &object, State& state, std::vector<Variable> &args) const {
    return (object->GetData<Callback>())(state, args);
}

std::string LiteScript::_Type_CALLBACK::ToString(const Variable &object) const {
    std::stringstream ss;
    const Callback& C = object->GetData<Callback>();
    if (C.isInternal())
        ss << "f(internal)";
    else
        ss << "f(I=" << C.I << ",L=" << C.L << ")";
    return ss.str();
}

void LiteScript::_Type_CALLBACK::Save(std::ostream &stream, Object &object, bool (Memory::*caller)(std::ostream&, unsigned int)) const {
    Callback& C = object.GetData<Callback>();
    OStreamer streamer(stream);
    if (C.isInternal())
        streamer << (unsigned char)0 << C.CallbackIndex;
    else
        streamer << (unsigned char)1 << C.I << C.L;
    if (C.This.isNull)
        streamer << (unsigned char)0;
    else {
        streamer << (unsigned char) 1;
        (object.memory.*caller)(stream, (*C.This)->ID);
    }
    if (C.nsp.isNull)
        streamer << (unsigned char)0;
    else {
        streamer << (unsigned char) 1;
        Namer::Save(stream, *C.nsp, caller);
    }
}

void LiteScript::_Type_CALLBACK::Load(std::istream &stream, Object &object, unsigned int (Memory::*caller)(std::istream&)) const {
    object.Reassign(Type::CALLBACK, sizeof(Callback));
    std::allocator<Callback> allocator;
    allocator.construct(&object.GetData<Callback>(), object.memory);
    Callback& C = object.GetData<Callback>();
    if (IStreamer::Read<unsigned char>(stream) == 0)
        C = Callback(object.memory, IStreamer::Read<unsigned int>(stream));
    else
        C = Callback(object.memory, IStreamer::Read<unsigned int>(stream), IStreamer::Read<unsigned int>(stream));
    if (IStreamer::Read<unsigned char>(stream) == 1)
        C.This = *object.memory.GetVariable((object.memory.*caller)(stream));
    if (IStreamer::Read<unsigned char>(stream) == 1)
        C.nsp = Namer::Load(stream, object.memory, caller);
}

void LiteScript::_Type_CALLBACK::GarbageCollector(const Variable &object, bool (Memory::*caller)(unsigned int)) const {
    if ((object->memory.*caller)(object->ID))
        return;
    const Callback& c = object->GetData<Callback>();
    if (!c.This.isNull)
        c.This->GarbageCollector(caller);
    if (!c.nsp.isNull)
        c.nsp->GarbageCollector(caller);
}