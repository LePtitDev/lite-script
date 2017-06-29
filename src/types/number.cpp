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

#include "number.hpp"

LiteScript::_Type_NUMBER LiteScript::_type_number;

LiteScript::_Type_NUMBER::_Type_NUMBER() : Type("NUMBER") {}

void LiteScript::_Type_NUMBER::CreateObject(LiteScript::Object& obj) {
    obj.Reassign(Type::NUMBER, sizeof(Number));
    obj.GetData<Number>() = Number(0);
}

LiteScript::Variable LiteScript::_Type_NUMBER::Convert(const LiteScript::Variable& obj, const LiteScript::Type& type) const {
    if (type == Type::BOOLEAN) {
        Variable res = obj->memory.Create(Type::BOOLEAN);
        res->GetData<bool>() = (obj->GetData<Number>() != Number(0));
        return res;
    }
    else if (type == Type::NUMBER) {
        Variable res = obj->memory.Create(Type::NUMBER);
        res->GetData<Number>() = obj->GetData<Number>();
        return res;
    }
    else if (type == Type::STRING) {
        Variable res = obj->memory.Create(Type::STRING);
        res->GetData<String>() = String(obj);
        return res;
    }
    else {
        return obj->memory.Create(Type::NIL);
    }
}
LiteScript::Object& LiteScript::_Type_NUMBER::AssignObject(LiteScript::Object& obj) {
    obj.Reassign(*this, sizeof(Number));
    obj.GetData<Number>() = Number(0);
    return obj;
}

std::string LiteScript::_Type_NUMBER::ToString(const Variable & obj) const {
    return (std::string)obj->GetData<Number>();
}

LiteScript::Variable LiteScript::_Type_NUMBER::OAssign(LiteScript::Variable& src, const LiteScript::Variable& dest) const {
    if (dest->GetType() != *this) {
        dest->GetType().AssignObject(*src);
        src->GetType().OAssign(src, dest);
    }
    else {
        src->GetData<Number>() = dest->GetData<Number>();
    }
    return Variable(src);
}

LiteScript::Variable LiteScript::_Type_NUMBER::OUnaryPlus(const LiteScript::Variable& obj) const {
    Variable res = obj->memory.Create(Type::NUMBER);
    res->GetData<Number>() = obj->GetData<Number>();
    return res;
}
LiteScript::Variable LiteScript::_Type_NUMBER::OUnaryMinus(const LiteScript::Variable& obj) const {
    Variable res = obj->memory.Create(Type::NUMBER);
    res->GetData<Number>() = -obj->GetData<Number>();
    return res;
}
LiteScript::Variable LiteScript::_Type_NUMBER::OPreIncrement(LiteScript::Variable& obj) const {
    ++obj->GetData<Number>();
    return obj;
}
LiteScript::Variable LiteScript::_Type_NUMBER::OPostIncrement(LiteScript::Variable& obj) const {
    Variable res = obj->memory.Create(Type::NUMBER);
    res->GetData<Number>() = obj->GetData<Number>()++;
    return res;
}
LiteScript::Variable LiteScript::_Type_NUMBER::OPreDecrement(LiteScript::Variable& obj) const {
    --obj->GetData<Number>();
    return obj;
}
LiteScript::Variable LiteScript::_Type_NUMBER::OPostDecrement(LiteScript::Variable& obj) const {
    Variable res = obj->memory.Create(Type::NUMBER);
    res->GetData<Number>() = obj->GetData<Number>()--;
    return res;
}

LiteScript::Variable LiteScript::_Type_NUMBER::OAdd(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() != *this)
            return obj1->memory.Create(Type::NIL);
        Variable res = obj1->memory.Create(Type::NUMBER);
        res->GetData<Number>() = obj1->GetData<Number>() + tmp->GetData<Number>();
        return res;
    }
    Variable res = obj1->memory.Create(Type::NUMBER);
    res->GetData<Number>() = obj1->GetData<Number>() + obj2->GetData<Number>();
    return res;
}
LiteScript::Variable LiteScript::_Type_NUMBER::OSubstract(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() != *this)
            return obj1->memory.Create(Type::NIL);
        Variable res = obj1->memory.Create(Type::NUMBER);
        res->GetData<Number>() = obj1->GetData<Number>() - tmp->GetData<Number>();
        return res;
    }
    Variable res = obj1->memory.Create(Type::NUMBER);
    res->GetData<Number>() = obj1->GetData<Number>() - obj2->GetData<Number>();
    return res;
}
LiteScript::Variable LiteScript::_Type_NUMBER::OMultiply(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() != *this)
            return obj1->memory.Create(Type::NIL);
        Variable res = obj1->memory.Create(Type::NUMBER);
        res->GetData<Number>() = obj1->GetData<Number>() * tmp->GetData<Number>();
        return res;
    }
    Variable res = obj1->memory.Create(Type::NUMBER);
    res->GetData<Number>() = obj1->GetData<Number>() * obj2->GetData<Number>();
    return res;
}
LiteScript::Variable LiteScript::_Type_NUMBER::ODivide(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() != *this)
            return obj1->memory.Create(Type::NIL);
        Variable res = obj1->memory.Create(Type::NUMBER);
        res->GetData<Number>() = obj1->GetData<Number>() / tmp->GetData<Number>();
        return res;
    }
    Variable res = obj1->memory.Create(Type::NUMBER);
    res->GetData<Number>() = obj1->GetData<Number>() / obj2->GetData<Number>();
    return res;
}
LiteScript::Variable LiteScript::_Type_NUMBER::OModulo(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() != *this)
            return obj1->memory.Create(Type::NIL);
        Variable res = obj1->memory.Create(Type::NUMBER);
        res->GetData<Number>() = obj1->GetData<Number>() % tmp->GetData<Number>();
        return res;
    }
    Variable res = obj1->memory.Create(Type::NUMBER);
    res->GetData<Number>() = obj1->GetData<Number>() % obj2->GetData<Number>();
    return res;
}

LiteScript::Variable LiteScript::_Type_NUMBER::OEqual(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() == *this)
            res->GetData<bool>() = (obj1->GetData<Number>() == tmp->GetData<Number>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<Number>() == obj2->GetData<Number>());
    return res;
}
LiteScript::Variable LiteScript::_Type_NUMBER::ONotEqual(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() == *this)
            res->GetData<bool>() = (obj1->GetData<Number>() != tmp->GetData<Number>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<Number>() != obj2->GetData<Number>());
    return res;
}
LiteScript::Variable LiteScript::_Type_NUMBER::OGreater(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() == *this)
            res->GetData<bool>() = (obj1->GetData<Number>() > tmp->GetData<Number>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<Number>() > obj2->GetData<Number>());
    return res;
}
LiteScript::Variable LiteScript::_Type_NUMBER::OLess(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() == *this)
            res->GetData<bool>() = (obj1->GetData<Number>() < tmp->GetData<Number>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<Number>() < obj2->GetData<Number>());
    return res;
}
LiteScript::Variable LiteScript::_Type_NUMBER::OGreaterOrEqual(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() == *this)
            res->GetData<bool>() = (obj1->GetData<Number>() >= tmp->GetData<Number>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<Number>() >= obj2->GetData<Number>());
    return res;
}
LiteScript::Variable LiteScript::_Type_NUMBER::OLessOrEqual(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    Variable res = obj1->memory.Create(Type::BOOLEAN);
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() == *this)
            res->GetData<bool>() = (obj1->GetData<Number>() <= tmp->GetData<Number>());
        return res;
    }
    res->GetData<bool>() = (obj1->GetData<Number>() <= obj2->GetData<Number>());
    return res;
}

LiteScript::Variable LiteScript::_Type_NUMBER::OBitwiseNot(const LiteScript::Variable& obj) const {
    Variable res = obj->memory.Create(Type::NUMBER);
    res->GetData<Number>() = ~obj->GetData<Number>();
    return res;
}
LiteScript::Variable LiteScript::_Type_NUMBER::OBitwiseAnd(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() != *this)
            return obj1->memory.Create(Type::NIL);
        Variable res = obj1->memory.Create(Type::NUMBER);
        res->GetData<Number>() = obj1->GetData<Number>() & tmp->GetData<Number>();
        return res;
    }
    Variable res = obj1->memory.Create(Type::NUMBER);
    res->GetData<Number>() = obj1->GetData<Number>() & obj2->GetData<Number>();
    return res;
}
LiteScript::Variable LiteScript::_Type_NUMBER::OBitwiseOr(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() != *this)
            return obj1->memory.Create(Type::NIL);
        Variable res = obj1->memory.Create(Type::NUMBER);
        res->GetData<Number>() = obj1->GetData<Number>() | tmp->GetData<Number>();
        return res;
    }
    Variable res = obj1->memory.Create(Type::NUMBER);
    res->GetData<Number>() = obj1->GetData<Number>() | obj2->GetData<Number>();
    return res;
}
LiteScript::Variable LiteScript::_Type_NUMBER::OBitwiseXor(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() != *this)
            return obj1->memory.Create(Type::NIL);
        Variable res = obj1->memory.Create(Type::NUMBER);
        res->GetData<Number>() = obj1->GetData<Number>() ^ tmp->GetData<Number>();
        return res;
    }
    Variable res = obj1->memory.Create(Type::NUMBER);
    res->GetData<Number>() = obj1->GetData<Number>() ^ obj2->GetData<Number>();
    return res;
}
LiteScript::Variable LiteScript::_Type_NUMBER::OLeftShift(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() != *this)
            return obj1->memory.Create(Type::NIL);
        Variable res = obj1->memory.Create(Type::NUMBER);
        res->GetData<Number>() = obj1->GetData<Number>() << tmp->GetData<Number>();
        return res;
    }
    Variable res = obj1->memory.Create(Type::NUMBER);
    res->GetData<Number>() = obj1->GetData<Number>() << obj2->GetData<Number>();
    return res;
}
LiteScript::Variable LiteScript::_Type_NUMBER::ORightShift(const LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() != *this)
            return obj1->memory.Create(Type::NIL);
        Variable res = obj1->memory.Create(Type::NUMBER);
        res->GetData<Number>() = obj1->GetData<Number>() >> tmp->GetData<Number>();
        return res;
    }
    Variable res = obj1->memory.Create(Type::NUMBER);
    res->GetData<Number>() = obj1->GetData<Number>() >> obj2->GetData<Number>();
    return res;
}

LiteScript::Variable LiteScript::_Type_NUMBER::OAddAndAssign(LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() != *this) {
            obj1 = obj1->memory.Create(Type::NIL);
            return obj1;
        }
        obj1->GetData<Number>() += tmp->GetData<Number>();
        return obj1;
    }
    obj1->GetData<Number>() += obj2->GetData<Number>();
    return obj1;
}
LiteScript::Variable LiteScript::_Type_NUMBER::OSubstractAndAssign(LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() != *this) {
            obj1 = obj1->memory.Create(Type::NIL);
            return obj1;
        }
        obj1->GetData<Number>() -= tmp->GetData<Number>();
        return obj1;
    }
    obj1->GetData<Number>() -= obj2->GetData<Number>();
    return obj1;
}
LiteScript::Variable LiteScript::_Type_NUMBER::OMultiplyAndAssign(LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() != *this) {
            obj1 = obj1->memory.Create(Type::NIL);
            return obj1;
        }
        obj1->GetData<Number>() *= tmp->GetData<Number>();
        return obj1;
    }
    obj1->GetData<Number>() *= obj2->GetData<Number>();
    return obj1;
}
LiteScript::Variable LiteScript::_Type_NUMBER::ODivideAndAssign(LiteScript::Variable& obj1, const LiteScript::Variable& obj2) const {
    if (obj2->GetType() != *this) {
        Variable tmp = obj2.Convert(*this);
        if (tmp->GetType() != *this) {
            obj1 = obj1->memory.Create(Type::NIL);
            return obj1;
        }
        obj1->GetData<Number>() /= tmp->GetData<Number>();
        return obj1;
    }
    obj1->GetData<Number>() /= obj2->GetData<Number>();
    return obj1;
}
