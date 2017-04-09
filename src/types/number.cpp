#include "number.hpp"

LiteScript::_Type_NUMBER LiteScript::_type_number;

LiteScript::_Type_NUMBER::_Type_NUMBER() : Type("Number") {}

LiteScript::Object LiteScript::_Type_NUMBER::CreateObject() {
    Object res(Type::NUMBER, sizeof(Number));
    res.GetData<Number>() = Number(0);
    return res;
}

LiteScript::Object LiteScript::_Type_NUMBER::Convert(const LiteScript::Object& obj, const LiteScript::Type& type) const {
    if (type == Type::BOOLEAN) {
        Object res = Type::BOOLEAN.CreateObject();
        res.GetData<bool>() = (obj.GetData<Number>() != Number(0));
        return res;
    }
    else if (type == Type::NUMBER) {
        Object res = Type::NUMBER.CreateObject();
        res.GetData<Number>() = obj.GetData<Number>();
        return res;
    }
    else if (type == Type::STRING) {
        Object res = Type::STRING.CreateObject();
        res.GetData<String>() = String((std::string)obj);
        return res;
    }
    else {
        return Type::NIL.CreateObject();
    }
}
LiteScript::Object& LiteScript::_Type_NUMBER::AssignObject(LiteScript::Object& obj) {
    obj.Reassign(*this, sizeof(Number));
    obj.GetData<Number>() = Number(0);
}

std::string LiteScript::_Type_NUMBER::ToString(const Object & obj) const {
    return (std::string)obj.GetData<Number>();
}

LiteScript::Object& LiteScript::_Type_NUMBER::OAssign(LiteScript::Object& src, const LiteScript::Object& dest) const {
    if (dest.GetType() != *this) {
        dest.GetType().AssignObject(src);
        src.GetType().OAssign(src, dest);
    }
    else {
        src.GetData<Number>() = dest.GetData<Number>();
    }
    return src;
}

LiteScript::Object LiteScript::_Type_NUMBER::OUnaryPlus(const LiteScript::Object& obj) const {
    Object res = LiteScript::Type::NUMBER.CreateObject();
    res.GetData<Number>() = obj.GetData<Number>();
    return res;
}
LiteScript::Object LiteScript::_Type_NUMBER::OUnaryMinus(const LiteScript::Object& obj) const {
    Object res = LiteScript::Type::NUMBER.CreateObject();
    res.GetData<Number>() = -obj.GetData<Number>();
    return res;
}
LiteScript::Object& LiteScript::_Type_NUMBER::OPreIncrement(LiteScript::Object& obj) const {
    ++obj.GetData<Number>();
    return obj;
}
LiteScript::Object LiteScript::_Type_NUMBER::OPostIncrement(LiteScript::Object& obj) const {
    Object res = LiteScript::Type::NUMBER.CreateObject();
    res.GetData<Number>() = obj.GetData<Number>()++;
    return res;
}
LiteScript::Object& LiteScript::_Type_NUMBER::OPreDecrement(LiteScript::Object& obj) const {
    --obj.GetData<Number>();
    return obj;
}
LiteScript::Object LiteScript::_Type_NUMBER::OPostDecrement(LiteScript::Object& obj) const {
    Object res = LiteScript::Type::NUMBER.CreateObject();
    res.GetData<Number>() = obj.GetData<Number>()--;
    return res;
}

LiteScript::Object LiteScript::_Type_NUMBER::OAdd(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() != *this)
            return Type::NIL.CreateObject();
        Object res = Type::NUMBER.CreateObject();
        res.GetData<Number>() = obj1.GetData<Number>() + tmp.GetData<Number>();
        return res;
    }
    Object res = Type::NUMBER.CreateObject();
    res.GetData<Number>() = obj1.GetData<Number>() + obj2.GetData<Number>();
    return res;
}
LiteScript::Object LiteScript::_Type_NUMBER::OSubstract(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() != *this)
            return Type::NIL.CreateObject();
        Object res = Type::NUMBER.CreateObject();
        res.GetData<Number>() = obj1.GetData<Number>() - tmp.GetData<Number>();
        return res;
    }
    Object res = Type::NUMBER.CreateObject();
    res.GetData<Number>() = obj1.GetData<Number>() - obj2.GetData<Number>();
    return res;
}
LiteScript::Object LiteScript::_Type_NUMBER::OMultiply(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() != *this)
            return Type::NIL.CreateObject();
        Object res = Type::NUMBER.CreateObject();
        res.GetData<Number>() = obj1.GetData<Number>() * tmp.GetData<Number>();
        return res;
    }
    Object res = Type::NUMBER.CreateObject();
    res.GetData<Number>() = obj1.GetData<Number>() * obj2.GetData<Number>();
    return res;
}
LiteScript::Object LiteScript::_Type_NUMBER::ODivide(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() != *this)
            return Type::NIL.CreateObject();
        Object res = Type::NUMBER.CreateObject();
        res.GetData<Number>() = obj1.GetData<Number>() / tmp.GetData<Number>();
        return res;
    }
    Object res = Type::NUMBER.CreateObject();
    res.GetData<Number>() = obj1.GetData<Number>() / obj2.GetData<Number>();
    return res;
}
LiteScript::Object LiteScript::_Type_NUMBER::OModulo(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() != *this)
            return Type::NIL.CreateObject();
        Object res = Type::NUMBER.CreateObject();
        res.GetData<Number>() = obj1.GetData<Number>() % tmp.GetData<Number>();
        return res;
    }
    Object res = Type::NUMBER.CreateObject();
    res.GetData<Number>() = obj1.GetData<Number>() % obj2.GetData<Number>();
    return res;
}

LiteScript::Object LiteScript::_Type_NUMBER::OEqual(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<Number>() == tmp.GetData<Number>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<Number>() == obj2.GetData<Number>());
    return res;
}
LiteScript::Object LiteScript::_Type_NUMBER::ONotEqual(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<Number>() != tmp.GetData<Number>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<Number>() != obj2.GetData<Number>());
    return res;
}
LiteScript::Object LiteScript::_Type_NUMBER::OGreater(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<Number>() > tmp.GetData<Number>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<Number>() > obj2.GetData<Number>());
    return res;
}
LiteScript::Object LiteScript::_Type_NUMBER::OLess(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<Number>() < tmp.GetData<Number>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<Number>() < obj2.GetData<Number>());
    return res;
}
LiteScript::Object LiteScript::_Type_NUMBER::OGreaterOrEqual(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<Number>() >= tmp.GetData<Number>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<Number>() >= obj2.GetData<Number>());
    return res;
}
LiteScript::Object LiteScript::_Type_NUMBER::OLessOrEqual(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<Number>() <= tmp.GetData<Number>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<Number>() <= obj2.GetData<Number>());
    return res;
}

LiteScript::Object LiteScript::_Type_NUMBER::OBitwiseNot(const LiteScript::Object& obj) const {
    Object res = LiteScript::Type::NUMBER.CreateObject();
    res.GetData<Number>() = ~obj.GetData<Number>();
    return res;
}
LiteScript::Object LiteScript::_Type_NUMBER::OBitwiseAnd(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() != *this)
            return Type::NIL.CreateObject();
        Object res = Type::NUMBER.CreateObject();
        res.GetData<Number>() = obj1.GetData<Number>() & tmp.GetData<Number>();
        return res;
    }
    Object res = Type::NUMBER.CreateObject();
    res.GetData<Number>() = obj1.GetData<Number>() & obj2.GetData<Number>();
    return res;
}
LiteScript::Object LiteScript::_Type_NUMBER::OBitwiseOr(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() != *this)
            return Type::NIL.CreateObject();
        Object res = Type::NUMBER.CreateObject();
        res.GetData<Number>() = obj1.GetData<Number>() | tmp.GetData<Number>();
        return res;
    }
    Object res = Type::NUMBER.CreateObject();
    res.GetData<Number>() = obj1.GetData<Number>() | obj2.GetData<Number>();
    return res;
}
LiteScript::Object LiteScript::_Type_NUMBER::OBitwiseXor(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() != *this)
            return Type::NIL.CreateObject();
        Object res = Type::NUMBER.CreateObject();
        res.GetData<Number>() = obj1.GetData<Number>() ^ tmp.GetData<Number>();
        return res;
    }
    Object res = Type::NUMBER.CreateObject();
    res.GetData<Number>() = obj1.GetData<Number>() ^ obj2.GetData<Number>();
    return res;
}
LiteScript::Object LiteScript::_Type_NUMBER::OLeftShift(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() != *this)
            return Type::NIL.CreateObject();
        Object res = Type::NUMBER.CreateObject();
        res.GetData<Number>() = obj1.GetData<Number>() << tmp.GetData<Number>();
        return res;
    }
    Object res = Type::NUMBER.CreateObject();
    res.GetData<Number>() = obj1.GetData<Number>() << obj2.GetData<Number>();
    return res;
}
LiteScript::Object LiteScript::_Type_NUMBER::ORightShift(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() != *this)
            return Type::NIL.CreateObject();
        Object res = Type::NUMBER.CreateObject();
        res.GetData<Number>() = obj1.GetData<Number>() >> tmp.GetData<Number>();
        return res;
    }
    Object res = Type::NUMBER.CreateObject();
    res.GetData<Number>() = obj1.GetData<Number>() >> obj2.GetData<Number>();
    return res;
}

LiteScript::Object& LiteScript::_Type_NUMBER::OAddAndAssign(LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() != *this) {
            obj1 = Type::NIL.CreateObject();
            return obj1;
        }
        obj1.GetData<Number>() += tmp.GetData<Number>();
        return obj1;
    }
    obj1.GetData<Number>() += obj2.GetData<Number>();
    return obj1;
}
LiteScript::Object& LiteScript::_Type_NUMBER::OSubstractAndAssign(LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() != *this) {
            obj1 = Type::NIL.CreateObject();
            return obj1;
        }
        obj1.GetData<Number>() -= tmp.GetData<Number>();
        return obj1;
    }
    obj1.GetData<Number>() -= obj2.GetData<Number>();
    return obj1;
}
LiteScript::Object& LiteScript::_Type_NUMBER::OMultiplyAndAssign(LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() != *this) {
            obj1 = Type::NIL.CreateObject();
            return obj1;
        }
        obj1.GetData<Number>() *= tmp.GetData<Number>();
        return obj1;
    }
    obj1.GetData<Number>() *= obj2.GetData<Number>();
    return obj1;
}
LiteScript::Object& LiteScript::_Type_NUMBER::ODivideAndAssign(LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() != *this) {
            obj1 = Type::NIL.CreateObject();
            return obj1;
        }
        obj1.GetData<Number>() /= tmp.GetData<Number>();
        return obj1;
    }
    obj1.GetData<Number>() /= obj2.GetData<Number>();
    return obj1;
}
