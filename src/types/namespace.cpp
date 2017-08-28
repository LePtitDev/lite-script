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
#include "namespace.hpp"
#include "internal.hpp"
#include "../streamer.hpp"

LiteScript::_Type_NAMESPACE LiteScript::_type_namespace;

LiteScript::_Type_NAMESPACE::_Type_NAMESPACE() : Type("NAMESPACE") {}

LiteScript::Object& LiteScript::_Type_NAMESPACE::AssignObject(Object &obj) {
    obj.Reassign(*this, sizeof(Namespace));
    std::allocator<Namespace> allocator;
    allocator.construct(&obj.GetData<Namespace>(), obj.memory);
    return obj;
}

void LiteScript::_Type_NAMESPACE::ODestroy(Object &object) const {
    std::allocator<Namespace> allocator;
    allocator.destroy(&object.GetData<Namespace>());
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

void LiteScript::_Type_NAMESPACE::Save(std::ostream &stream, Object &object, bool (Memory::*caller)(std::ostream&, unsigned int)) const {
    Namespace& obj = object.GetData<Namespace>();
    OStreamer::Write<unsigned int>(stream, obj.Count());
    for (unsigned int i = 0, sz = obj.Count(); i < sz; i++) {
        stream << obj.GetKey(i) << (uint8_t)0;
        (object.memory.*caller)(stream, obj.GetVariable(i)->ID);
    }
}

void LiteScript::_Type_NAMESPACE::Load(std::istream &stream, Object &object, unsigned int (Memory::*caller)(std::istream&)) const {
    object.Reassign(Type::NAMESPACE, sizeof(Namespace));
    Namespace& obj = object.GetData<Namespace>();
    unsigned int sz = IStreamer::Read<unsigned int>(stream);
    std::string key;
    unsigned char c;
    for (unsigned int i = 0; i < sz; i++) {
        key.clear();
        while (!stream.eof() && (c = (unsigned char)stream.get()) != 0)
            key += c;
        obj.DefineVariable(key.c_str(), *object.memory.GetVariable((object.memory.*caller)(stream)));
    }
}

void LiteScript::_Type_NAMESPACE::GarbageCollector(const Variable &object, bool (Memory::*caller)(unsigned int)) const {
    if ((object->memory.*caller)(object->ID))
        return;
    const Namespace& nsp = object->GetData<Namespace>();
    for (unsigned int i = 0, sz = nsp.Count(); i < sz; i++)
        nsp.GetVariable(i).GarbageCollector(caller);
}