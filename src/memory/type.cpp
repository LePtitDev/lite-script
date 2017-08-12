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

#include "../litescript.hpp"

#include "../types/undefined.hpp"
#include "../types/null.hpp"
#include "../types/boolean.hpp"
#include "../types/number.hpp"
#include "../types/string.hpp"
#include "../types/callback.hpp"
#include "../types/array.hpp"
#include "../types/class.hpp"
#include "../types/class_object.hpp"
#include "../types/namespace.hpp"

LiteScript::Type& LiteScript::Type::UNDEFINED(LiteScript::_type_undefined);
LiteScript::Type& LiteScript::Type::NIL(LiteScript::_type_nil);
LiteScript::Type& LiteScript::Type::BOOLEAN(LiteScript::_type_boolean);
LiteScript::Type& LiteScript::Type::NUMBER(LiteScript::_type_number);
LiteScript::Type& LiteScript::Type::STRING(LiteScript::_type_string);
LiteScript::Type& LiteScript::Type::CALLBACK(LiteScript::_type_callback);
LiteScript::Type& LiteScript::Type::ARRAY(LiteScript::_type_array);
LiteScript::Type& LiteScript::Type::CLASS(LiteScript::_type_class);
LiteScript::Type& LiteScript::Type::CLASS_OBJECT(LiteScript::_type_class_object);
LiteScript::Type& LiteScript::Type::NAMESPACE(LiteScript::_type_namespace);

unsigned int litescript_type_id_iterator = 0;

LiteScript::Type::Type(const char * name) :
    name(name), id(litescript_type_id_iterator++)
{
    this->name.shrink_to_fit();
}

unsigned int LiteScript::Type::GetID() const {
    return this->id;
}

const char * LiteScript::Type::GetName() const {
    return this->name.c_str();
}

bool LiteScript::Type::operator==(const Type & t) const {
    return (this->id == t.id);
}

bool LiteScript::Type::operator!=(const Type & t) const {
    return (this->id != t.id);
}

LiteScript::Variable LiteScript::Type::Convert(const Variable& obj, const Type & type) const { return obj->memory.Create(Type::NIL); }

void LiteScript::Type::ODestroy(Object& obj) {}

LiteScript::Variable LiteScript::Type::OAssign(LiteScript::Variable& x1, const LiteScript::Variable&) const { return x1; }

LiteScript::Variable LiteScript::Type::OUnaryPlus(const LiteScript::Variable& obj) const { return obj->memory.Create(Type::NIL); }
LiteScript::Variable LiteScript::Type::OUnaryMinus(const LiteScript::Variable& obj) const { return obj->memory.Create(Type::NIL); }
LiteScript::Variable LiteScript::Type::OPreIncrement(LiteScript::Variable& x) const { return x; }
LiteScript::Variable LiteScript::Type::OPostIncrement(LiteScript::Variable& obj) const { return obj->memory.Create(Type::NIL); }
LiteScript::Variable LiteScript::Type::OPreDecrement(LiteScript::Variable& x) const { return x; }
LiteScript::Variable LiteScript::Type::OPostDecrement(LiteScript::Variable& obj) const { return obj->memory.Create(Type::NIL); }

LiteScript::Variable LiteScript::Type::OAdd(const LiteScript::Variable& obj, const LiteScript::Variable&) const { return obj->memory.Create(Type::NIL); }
LiteScript::Variable LiteScript::Type::OSubstract(const LiteScript::Variable& obj, const LiteScript::Variable&) const { return obj->memory.Create(Type::NIL); }
LiteScript::Variable LiteScript::Type::OMultiply(const LiteScript::Variable& obj, const LiteScript::Variable&) const { return obj->memory.Create(Type::NIL); }
LiteScript::Variable LiteScript::Type::ODivide(const LiteScript::Variable& obj, const LiteScript::Variable&) const { return obj->memory.Create(Type::NIL); }
LiteScript::Variable LiteScript::Type::OModulo(const LiteScript::Variable& obj, const LiteScript::Variable&) const { return obj->memory.Create(Type::NIL); }

LiteScript::Variable LiteScript::Type::OEqual(const LiteScript::Variable& obj, const LiteScript::Variable&) const { return obj->memory.Create(Type::BOOLEAN); }
LiteScript::Variable LiteScript::Type::ONotEqual(const LiteScript::Variable& obj, const LiteScript::Variable&) const { return obj->memory.Create(Type::BOOLEAN); }
LiteScript::Variable LiteScript::Type::OGreater(const LiteScript::Variable& obj, const LiteScript::Variable&) const { return obj->memory.Create(Type::BOOLEAN); }
LiteScript::Variable LiteScript::Type::OLess(const LiteScript::Variable& obj, const LiteScript::Variable&) const { return obj->memory.Create(Type::BOOLEAN); }
LiteScript::Variable LiteScript::Type::OGreaterOrEqual(const LiteScript::Variable& obj, const LiteScript::Variable&) const { return obj->memory.Create(Type::BOOLEAN); }
LiteScript::Variable LiteScript::Type::OLessOrEqual(const LiteScript::Variable& obj, const LiteScript::Variable&) const { return obj->memory.Create(Type::BOOLEAN); }

LiteScript::Variable LiteScript::Type::OLogicalNot(const LiteScript::Variable& obj) const { return obj->memory.Create(Type::BOOLEAN); }
LiteScript::Variable LiteScript::Type::OLogicalAnd(const LiteScript::Variable& obj, const LiteScript::Variable&) const { return obj->memory.Create(Type::BOOLEAN); }
LiteScript::Variable LiteScript::Type::OLogicalOr(const LiteScript::Variable& obj, const LiteScript::Variable&) const { return obj->memory.Create(Type::BOOLEAN); }

LiteScript::Variable LiteScript::Type::OBitwiseNot(const LiteScript::Variable& obj) const { return obj->memory.Create(Type::NIL); }
LiteScript::Variable LiteScript::Type::OBitwiseAnd(const LiteScript::Variable& obj, const LiteScript::Variable&) const { return obj->memory.Create(Type::NIL); }
LiteScript::Variable LiteScript::Type::OBitwiseOr(const LiteScript::Variable& obj, const LiteScript::Variable&) const { return Variable(obj); }
LiteScript::Variable LiteScript::Type::OBitwiseXor(const LiteScript::Variable& obj, const LiteScript::Variable&) const { return obj->memory.Create(Type::NIL); }
LiteScript::Variable LiteScript::Type::OLeftShift(const LiteScript::Variable& obj, const LiteScript::Variable&) const { return obj->memory.Create(Type::NIL); }
LiteScript::Variable LiteScript::Type::ORightShift(const LiteScript::Variable& obj, const LiteScript::Variable&) const { return obj->memory.Create(Type::NIL); }

LiteScript::Variable LiteScript::Type::OAddAndAssign(LiteScript::Variable& x, const LiteScript::Variable&) const { return x; }
LiteScript::Variable LiteScript::Type::OSubstractAndAssign(LiteScript::Variable& x, const LiteScript::Variable&) const { return x; }
LiteScript::Variable LiteScript::Type::OMultiplyAndAssign(LiteScript::Variable& x, const LiteScript::Variable&) const { return x; }
LiteScript::Variable LiteScript::Type::ODivideAndAssign(LiteScript::Variable& x, const LiteScript::Variable&) const { return x; }

LiteScript::Variable LiteScript::Type::OArray(LiteScript::Variable& x, const LiteScript::Variable&) const { return x->memory.Create(_type_undefined); }
LiteScript::Variable LiteScript::Type::OMember(LiteScript::Variable& x, const char *) const { return x->memory.Create(_type_undefined); }

LiteScript::Variable LiteScript::Type::OCall(LiteScript::Variable& obj, State& state, std::vector<LiteScript::Variable>&) const { return obj->memory.Create(_type_undefined); }

std::string LiteScript::Type::ToString(const LiteScript::Variable&) const { return this->name; }

void LiteScript::Type::GarbageCollector(const Variable &object, void (Memory::*caller)(unsigned int)) const { (object->memory.*caller)(object->ID); }
