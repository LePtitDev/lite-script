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

#include "type.hpp"

#include "../types/null.hpp"
#include "../types/boolean.hpp"
#include "../types/number.hpp"
#include "../types/string.hpp"

LiteScript::Type& LiteScript::Type::NIL(LiteScript::_type_nil);
LiteScript::Type& LiteScript::Type::BOOLEAN(LiteScript::_type_boolean);
LiteScript::Type& LiteScript::Type::NUMBER(LiteScript::_type_number);
LiteScript::Type& LiteScript::Type::STRING(LiteScript::_type_string);

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

LiteScript::Object LiteScript::Type::Convert(const Object&, const Type & type) const { return LiteScript::Type::NIL.CreateObject(); }

LiteScript::Object& LiteScript::Type::AssignObject(Object & obj) { obj.Reassign(*this, 0); return obj; }

void LiteScript::Type::ODestroy(Object& obj) {}

LiteScript::Object& LiteScript::Type::OAssign(LiteScript::Object& x1, const LiteScript::Object&) const { return x1; }

LiteScript::Object LiteScript::Type::OUnaryPlus(const LiteScript::Object&) const { return LiteScript::Type::NIL.CreateObject(); }
LiteScript::Object LiteScript::Type::OUnaryMinus(const LiteScript::Object&) const { return LiteScript::Type::NIL.CreateObject(); }
LiteScript::Object& LiteScript::Type::OPreIncrement(LiteScript::Object& x) const { return x; }
LiteScript::Object LiteScript::Type::OPostIncrement(LiteScript::Object&) const { return LiteScript::Type::NIL.CreateObject(); }
LiteScript::Object& LiteScript::Type::OPreDecrement(LiteScript::Object& x) const { return x; }
LiteScript::Object LiteScript::Type::OPostDecrement(LiteScript::Object&) const { return LiteScript::Type::NIL.CreateObject(); }

LiteScript::Object LiteScript::Type::OAdd(const LiteScript::Object&, const LiteScript::Object&) const { return LiteScript::Type::NIL.CreateObject(); }
LiteScript::Object LiteScript::Type::OSubstract(const LiteScript::Object&, const LiteScript::Object&) const { return LiteScript::Type::NIL.CreateObject(); }
LiteScript::Object LiteScript::Type::OMultiply(const LiteScript::Object&, const LiteScript::Object&) const { return LiteScript::Type::NIL.CreateObject(); }
LiteScript::Object LiteScript::Type::ODivide(const LiteScript::Object&, const LiteScript::Object&) const { return LiteScript::Type::NIL.CreateObject(); }
LiteScript::Object LiteScript::Type::OModulo(const LiteScript::Object&, const LiteScript::Object&) const { return LiteScript::Type::NIL.CreateObject(); }

LiteScript::Object LiteScript::Type::OEqual(const LiteScript::Object&, const LiteScript::Object&) const { return LiteScript::Type::BOOLEAN.CreateObject(); }
LiteScript::Object LiteScript::Type::ONotEqual(const LiteScript::Object&, const LiteScript::Object&) const { return LiteScript::Type::BOOLEAN.CreateObject(); }
LiteScript::Object LiteScript::Type::OGreater(const LiteScript::Object&, const LiteScript::Object&) const { return LiteScript::Type::BOOLEAN.CreateObject(); }
LiteScript::Object LiteScript::Type::OLess(const LiteScript::Object&, const LiteScript::Object&) const { return LiteScript::Type::BOOLEAN.CreateObject(); }
LiteScript::Object LiteScript::Type::OGreaterOrEqual(const LiteScript::Object&, const LiteScript::Object&) const { return LiteScript::Type::BOOLEAN.CreateObject(); }
LiteScript::Object LiteScript::Type::OLessOrEqual(const LiteScript::Object&, const LiteScript::Object&) const { return LiteScript::Type::BOOLEAN.CreateObject(); }

LiteScript::Object LiteScript::Type::OLogicalNot(const LiteScript::Object&) const { return LiteScript::Type::BOOLEAN.CreateObject(); }
LiteScript::Object LiteScript::Type::OLogicalAnd(const LiteScript::Object&, const LiteScript::Object&) const { return LiteScript::Type::BOOLEAN.CreateObject(); }
LiteScript::Object LiteScript::Type::OLogicalOr(const LiteScript::Object&, const LiteScript::Object&) const { return LiteScript::Type::BOOLEAN.CreateObject(); }

LiteScript::Object LiteScript::Type::OBitwiseNot(const LiteScript::Object&) const { return LiteScript::Type::NIL.CreateObject(); }
LiteScript::Object LiteScript::Type::OBitwiseAnd(const LiteScript::Object&, const LiteScript::Object&) const { return LiteScript::Type::NIL.CreateObject(); }
LiteScript::Object LiteScript::Type::OBitwiseOr(const LiteScript::Object&, const LiteScript::Object&) const { return LiteScript::Type::NIL.CreateObject(); }
LiteScript::Object LiteScript::Type::OBitwiseXor(const LiteScript::Object&, const LiteScript::Object&) const { return LiteScript::Type::NIL.CreateObject(); }
LiteScript::Object LiteScript::Type::OLeftShift(const LiteScript::Object&, const LiteScript::Object&) const { return LiteScript::Type::NIL.CreateObject(); }
LiteScript::Object LiteScript::Type::ORightShift(const LiteScript::Object&, const LiteScript::Object&) const { return LiteScript::Type::NIL.CreateObject(); }

LiteScript::Object& LiteScript::Type::OAddAndAssign(LiteScript::Object& x, const LiteScript::Object&) const { return x; }
LiteScript::Object& LiteScript::Type::OSubstractAndAssign(LiteScript::Object& x, const LiteScript::Object&) const { return x; }
LiteScript::Object& LiteScript::Type::OMultiplyAndAssign(LiteScript::Object& x, const LiteScript::Object&) const { return x; }
LiteScript::Object& LiteScript::Type::ODivideAndAssign(LiteScript::Object& x, const LiteScript::Object&) const { return x; }

LiteScript::Object& LiteScript::Type::OArray(LiteScript::Object& x, const LiteScript::Object&) const { return Object::UNDEFINED; }
LiteScript::Object& LiteScript::Type::OMember(LiteScript::Object& x, const char *) const { return Object::UNDEFINED; }

LiteScript::Object LiteScript::Type::OCall(LiteScript::Object&, std::vector<std::unique_ptr<LiteScript::Object>>&) const { return LiteScript::Type::NIL.CreateObject(); }

std::string LiteScript::Type::ToString(const LiteScript::Object&) const { return this->name; }
