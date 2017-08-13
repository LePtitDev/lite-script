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
#include "object.hpp"
#include "variable.hpp"
#include "memory.hpp"

LiteScript::Variable::Variable(Object& obj, unsigned int& nb_ref) :
    obj(obj), nb_ref(nb_ref)
{
    this->nb_ref++;
}

LiteScript::Variable::Variable(const Variable &v) :
    obj(v.obj), nb_ref(v.nb_ref)
{
    this->nb_ref++;
}

LiteScript::Variable::~Variable() {
    if (--this->nb_ref == 0)
        this->obj.memory.Remove(this->obj.ID);
}

LiteScript::Object * LiteScript::Variable::operator->() {
    return &this->obj;
}

const LiteScript::Object * LiteScript::Variable::operator->() const {
    return &this->obj;
}

LiteScript::Object & LiteScript::Variable::operator*() {
    return this->obj;
}

const LiteScript::Object & LiteScript::Variable::operator*() const {
    return this->obj;
}

LiteScript::Variable LiteScript::Variable::Convert(const Type & type) const {
    return this->obj.GetType().Convert(*this, type);
}

LiteScript::Variable LiteScript::Variable::operator=(const LiteScript::Variable& obj) {
    return this->obj.GetType().OAssign(*this, obj);
}

LiteScript::Variable LiteScript::Variable::operator+() const {
    return this->obj.GetType().OUnaryPlus(*this);
}
LiteScript::Variable LiteScript::Variable::operator-() const {
    return this->obj.GetType().OUnaryMinus(*this);
}
LiteScript::Variable LiteScript::Variable::operator++() {
    return this->obj.GetType().OPreIncrement(*this);
}
LiteScript::Variable LiteScript::Variable::operator++(int) {
    return this->obj.GetType().OPostIncrement(*this);
}
LiteScript::Variable LiteScript::Variable::operator--() {
    return this->obj.GetType().OPreDecrement(*this);
}
LiteScript::Variable LiteScript::Variable::operator--(int) {
    return this->obj.GetType().OPostDecrement(*this);
}

LiteScript::Variable LiteScript::Variable::operator+(const LiteScript::Variable& obj) const {
    return this->obj.GetType().OAdd(*this, obj);
}
LiteScript::Variable LiteScript::Variable::operator-(const LiteScript::Variable& obj) const {
    return this->obj.GetType().OSubstract(*this, obj);
}
LiteScript::Variable LiteScript::Variable::operator*(const LiteScript::Variable& obj) const {
    return this->obj.GetType().OMultiply(*this, obj);
}
LiteScript::Variable LiteScript::Variable::operator/(const LiteScript::Variable& obj) const {
    return this->obj.GetType().ODivide(*this, obj);
}
LiteScript::Variable LiteScript::Variable::operator%(const LiteScript::Variable& obj) const {
    return this->obj.GetType().OModulo(*this, obj);
}

LiteScript::Variable LiteScript::Variable::operator==(const LiteScript::Variable& obj) const {
    return this->obj.GetType().OEqual(*this, obj);
}
LiteScript::Variable LiteScript::Variable::operator!=(const LiteScript::Variable& obj) const {
    return this->obj.GetType().ONotEqual(*this, obj);
}
LiteScript::Variable LiteScript::Variable::operator>(const LiteScript::Variable& obj) const {
    return this->obj.GetType().OGreater(*this, obj);
}
LiteScript::Variable LiteScript::Variable::operator<(const LiteScript::Variable& obj) const {
    return this->obj.GetType().OLess(*this, obj);
}
LiteScript::Variable LiteScript::Variable::operator>=(const LiteScript::Variable& obj) const {
    return this->obj.GetType().OGreaterOrEqual(*this, obj);
}
LiteScript::Variable LiteScript::Variable::operator<=(const LiteScript::Variable& obj) const {
    return this ->obj.GetType().OLessOrEqual(*this, obj);
}

LiteScript::Variable LiteScript::Variable::operator!() const {
    return this->obj.GetType().OLogicalNot(*this);
}
LiteScript::Variable LiteScript::Variable::operator&&(const LiteScript::Variable& obj) const {
    return this->obj.GetType().OLogicalAnd(*this, obj);
}
LiteScript::Variable LiteScript::Variable::operator||(const LiteScript::Variable& obj) const {
    return this->obj.GetType().OLogicalOr(*this, obj);
}

LiteScript::Variable LiteScript::Variable::operator~() const {
    return this->obj.GetType().OBitwiseNot(*this);
}
LiteScript::Variable LiteScript::Variable::operator&(const LiteScript::Variable& obj) const {
    return this->obj.GetType().OBitwiseAnd(*this, obj);
}
LiteScript::Variable LiteScript::Variable::operator|(const LiteScript::Variable& obj) const {
    return this->obj.GetType().OBitwiseOr(*this, obj);
}
LiteScript::Variable LiteScript::Variable::operator^(const LiteScript::Variable& obj) const {
    return this->obj.GetType().OBitwiseXor(*this, obj);
}
LiteScript::Variable LiteScript::Variable::operator<<(const LiteScript::Variable& obj) const {
    return this->obj.GetType().OLeftShift(*this, obj);
}
LiteScript::Variable LiteScript::Variable::operator>>(const LiteScript::Variable& obj) const {
    return this->obj.GetType().ORightShift(*this, obj);
}

LiteScript::Variable LiteScript::Variable::operator+=(const LiteScript::Variable& obj) {
    return this->obj.GetType().OAddAndAssign(*this, obj);
}
LiteScript::Variable LiteScript::Variable::operator-=(const LiteScript::Variable& obj) {
    return this->obj.GetType().OSubstractAndAssign(*this, obj);
}
LiteScript::Variable LiteScript::Variable::operator*=(const LiteScript::Variable& obj) {
    return this->obj.GetType().OMultiplyAndAssign(*this, obj);
}
LiteScript::Variable LiteScript::Variable::operator/=(const LiteScript::Variable& obj) {
    return this->obj.GetType().ODivideAndAssign(*this, obj);
}

LiteScript::Variable LiteScript::Variable::operator[](const LiteScript::Variable& obj) {
    return this->obj.GetType().OArray(*this, obj);
}
LiteScript::Variable LiteScript::Variable::GetMember(const char * name) {
    return this->obj.GetType().OMember(*this, name);
}

LiteScript::Variable LiteScript::Variable::operator()(State& state, std::vector<LiteScript::Variable>& args) {
    return this->obj.GetType().OCall(*this, state, args);
}

LiteScript::Variable::operator std::string() const {
    return this->obj.GetType().ToString(*this);
}

void LiteScript::Variable::GarbageCollector(void (Memory::*caller)(unsigned int)) {
    this->obj.GetType().GarbageCollector(*this, caller);
}

std::ostream& operator<<(std::ostream& stream, const LiteScript::Variable& obj) {
    stream << (std::string)(obj);
    return stream;
}