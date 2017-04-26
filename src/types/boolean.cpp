#include "boolean.hpp"

LiteScript::_Type_BOOLEAN LiteScript::_type_boolean;

LiteScript::_Type_BOOLEAN::_Type_BOOLEAN() : Type("BOOLEAN") {}

LiteScript::Object LiteScript::_Type_BOOLEAN::CreateObject() {
    Object res(Type::BOOLEAN, sizeof(bool));
    res.GetData<bool>() = false;
    return res;
}

LiteScript::Object LiteScript::_Type_BOOLEAN::Convert(const LiteScript::Object& obj, const LiteScript::Type& type) const {
    if (type == Type::BOOLEAN) {
        Object res = Type::BOOLEAN.CreateObject();
        res.GetData<bool>() = obj.GetData<bool>();
        return res;
    }
    else if (type == Type::NUMBER) {
        Object res = Type::NUMBER.CreateObject();
        res.GetData<Number>() = Number(obj.GetData<bool>() ? 1 : 0);
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
LiteScript::Object& LiteScript::_Type_BOOLEAN::AssignObject(LiteScript::Object& obj) {
    obj.Reassign(*this, sizeof(bool));
    obj.GetData<bool>() = false;
}

std::string LiteScript::_Type_BOOLEAN::ToString(const Object & obj) const {
    return ((obj.GetData<bool>()) ? "true" : "false");
}

LiteScript::Object& LiteScript::_Type_BOOLEAN::OAssign(LiteScript::Object& src, const LiteScript::Object& dest) const {
    if (dest.GetType() != *this) {
        dest.GetType().AssignObject(src);
        src.GetType().OAssign(src, dest);
    }
    else {
        src.GetData<bool>() = dest.GetData<bool>();
    }
    return src;
}

LiteScript::Object LiteScript::_Type_BOOLEAN::OEqual(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<bool>() == tmp.GetData<bool>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<bool>() == obj2.GetData<bool>());
    return res;
}
LiteScript::Object LiteScript::_Type_BOOLEAN::ONotEqual(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<bool>() != tmp.GetData<bool>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<bool>() != obj2.GetData<bool>());
    return res;
}

LiteScript::Object LiteScript::_Type_BOOLEAN::OLogicalNot(const LiteScript::Object& obj) const {
    Object res = Type::BOOLEAN.CreateObject();
    res.GetData<bool>() = !obj.GetData<bool>();
    return res;
}
LiteScript::Object LiteScript::_Type_BOOLEAN::OLogicalAnd(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<bool>() && tmp.GetData<bool>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<bool>() && obj2.GetData<bool>());
    return res;
}
LiteScript::Object LiteScript::_Type_BOOLEAN::OLogicalOr(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<bool>() || tmp.GetData<bool>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<bool>() || obj2.GetData<bool>());
    return res;
}
