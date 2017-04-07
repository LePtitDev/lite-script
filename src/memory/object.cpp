#include "object.hpp"

LiteScript::Object::Object() :
    type(Type::NIL), size(0), data(nullptr)
{

}

LiteScript::Object::Object(Type & t, unsigned int n) :
    type(t), size(n), data(nullptr)
{
    if (n > 0)
        this->data = allocator.allocate(n);
}

LiteScript::Object::Object(const Object & obj) :
    type(obj.type), size(obj.size), allocator(obj.allocator), data(nullptr)
{
    if (this->size > 0) {
        this->data = this->allocator.allocate(this->size);
        memcpy(this->data, obj.data, this->size);
    }
}

LiteScript::Object::~Object() {
    if (this->size > 0) {
        this->size = 0;
        this->allocator.deallocate((char *)this->data, this->size);
    }
}

LiteScript::Type & LiteScript::Object::GetType() const {
    return this->type;
}

LiteScript::Object& LiteScript::Object::operator=(const LiteScript::Object& obj) {
    return this->type.OAssign(*this, obj);
}

LiteScript::Object LiteScript::Object::operator+() const {
    return this->type.OUnaryPlus(*this);
}
LiteScript::Object LiteScript::Object::operator-() const {
    return this->type.OUnaryMinus(*this);
}
LiteScript::Object& LiteScript::Object::operator++() {
    return this->type.OPreIncrement(*this);
}
LiteScript::Object LiteScript::Object::operator++(int) {
    return this->type.OPostIncrement(*this);
}
LiteScript::Object& LiteScript::Object::operator--() {
    return this->type.OPreDecrement(*this);
}
LiteScript::Object LiteScript::Object::operator--(int) {
    return this->type.OPostDecrement(*this);
}

LiteScript::Object LiteScript::Object::operator+(const LiteScript::Object& obj) const {
    return this->type.OAdd(*this, obj);
}
LiteScript::Object LiteScript::Object::operator-(const LiteScript::Object& obj) const {
    return this->type.OSubstract(*this, obj);
}
LiteScript::Object LiteScript::Object::operator*(const LiteScript::Object& obj) const {
    return this->type.OMultiply(*this, obj);
}
LiteScript::Object LiteScript::Object::operator/(const LiteScript::Object& obj) const {
    return this->type.ODivide(*this, obj);
}
LiteScript::Object LiteScript::Object::operator%(const LiteScript::Object& obj) const {
    return this->type.OModulo(*this, obj);
}

bool LiteScript::Object::operator==(const LiteScript::Object& obj) const {
    return this->type.OEqual(*this, obj);
}
bool LiteScript::Object::operator!=(const LiteScript::Object& obj) const {
    return this->type.ONotEqual(*this, obj);
}
bool LiteScript::Object::operator>(const LiteScript::Object& obj) const {
    return this->type.OGreater(*this, obj);
}
bool LiteScript::Object::operator<(const LiteScript::Object& obj) const {
    return this->type.OLess(*this, obj);
}
bool LiteScript::Object::operator>=(const LiteScript::Object& obj) const {
    return this->type.OGreaterOrEqual(*this, obj);
}
bool LiteScript::Object::operator<=(const LiteScript::Object& obj) const {
    return this ->type.OLessOrEqual(*this, obj);
}

LiteScript::Object LiteScript::Object::operator!() const {
    return this->type.OLogicalNot(*this);
}
LiteScript::Object LiteScript::Object::operator&&(const LiteScript::Object& obj) const {
    return this->type.OLogicalAnd(*this, obj);
}
LiteScript::Object LiteScript::Object::operator||(const LiteScript::Object& obj) const {
    return this->type.OLogicalOr(*this, obj);
}

LiteScript::Object LiteScript::Object::operator~() const {
    return this->type.OBitwiseNot(*this);
}
LiteScript::Object LiteScript::Object::operator&(const LiteScript::Object& obj) const {
    return this->type.OBitwiseAnd(*this, obj);
}
LiteScript::Object LiteScript::Object::operator|(const LiteScript::Object& obj) const {
    return this->type.OBitwiseOr(*this, obj);
}
LiteScript::Object LiteScript::Object::operator^(const LiteScript::Object& obj) const {
    return this->type.OBitwiseXor(*this, obj);
}
LiteScript::Object LiteScript::Object::operator<<(const LiteScript::Object& obj) const {
    return this->type.OLeftShift(*this, obj);
}
LiteScript::Object LiteScript::Object::operator>>(const LiteScript::Object& obj) const {
    return this->type.ORightShift(*this, obj);
}

LiteScript::Object& LiteScript::Object::operator+=(const LiteScript::Object& obj) {
    return this->type.OAddAndAssign(*this, obj);
}
LiteScript::Object& LiteScript::Object::operator-=(const LiteScript::Object& obj) {
    return this->type.OSubstractAndAssign(*this, obj);
}
LiteScript::Object& LiteScript::Object::operator*=(const LiteScript::Object& obj) {
    return this->type.OMultiplyAndAssign(*this, obj);
}
LiteScript::Object& LiteScript::Object::operator/=(const LiteScript::Object& obj) {
    return this->type.ODivideAndAssign(*this, obj);
}

LiteScript::Object& LiteScript::Object::operator[](const LiteScript::Object& obj) {
    return this->type.OArray(*this, obj);
}
LiteScript::Object& LiteScript::Object::GetMember(const char * name) {
    return this->type.OMember(*this, name);
}

LiteScript::Object LiteScript::Object::operator()(std::vector<std::unique_ptr<LiteScript::Object>>& args) {
    return this->type.OCall(*this, args);
}

std::string LiteScript::Object::ToString() const {
    return this->type.ToString(*this);
}