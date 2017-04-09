#include "string.hpp"

LiteScript::_Type_STRING LiteScript::_type_string;

LiteScript::_Type_STRING::_Type_STRING() : Type("String") {};

LiteScript::Object LiteScript::_Type_STRING::CreateObject() {
    Object res(*this, sizeof(String) + sizeof(Object));
    ObjectAllocator.construct(&res.GetData<String>());
    ObjectAllocator.construct((Object *)(&res.GetData<char>() + sizeof(Object)));
    return res;
}

LiteScript::Object LiteScript::_Type_STRING::Convert(const LiteScript::Object& obj, const LiteScript::Type& type) const {
    if (type == Type::STRING) {
        Object res = Type::STRING.CreateObject();
        res.GetData<String>() = obj.GetData<String>();
        return res;
    }
    else {
        return Type::NIL.CreateObject();
    }
}
LiteScript::Object& LiteScript::_Type_STRING::AssignObject(LiteScript::Object& obj) {
    obj.Reassign(*this, sizeof(String) + sizeof(Object));
    ObjectAllocator.construct(&obj.GetData<String>());
    ObjectAllocator.construct((Object *)(&obj.GetData<char>() + sizeof(Object)));
    return obj;
}

void LiteScript::_Type_STRING::ODestroy(LiteScript::Object& obj) {
    ObjectAllocator.destroy((Object *)(&obj.GetData<char>() + sizeof(Object)));
}

LiteScript::Object& LiteScript::_Type_STRING::OAssign(LiteScript::Object& src, const LiteScript::Object& dest) const {
    if (dest.GetType() != *this) {
        dest.GetType().AssignObject(src);
        src.GetType().OAssign(src, dest);
    }
    else {
        src.GetData<String>() = dest.GetData<String>();
    }
    return src;
}

LiteScript::Object LiteScript::_Type_STRING::OAdd(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() != *this)
            return Type::NIL.CreateObject();
        Object res = Type::STRING.CreateObject();
        res.GetData<String>() = obj1.GetData<String>() + tmp.GetData<String>();
        return res;
    }
    Object res = Type::STRING.CreateObject();
    res.GetData<String>() = obj1.GetData<String>() + obj2.GetData<String>();
    return res;
}
LiteScript::Object LiteScript::_Type_STRING::OMultiply(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != Type::NUMBER) {
        Object tmp = obj2.Convert(Type::NUMBER);
        if (tmp.GetType() != Type::NUMBER)
            return Type::NIL.CreateObject();
        Object res = Type::STRING.CreateObject();
        res.GetData<String>() = obj1.GetData<String>() * (int)(tmp.GetData<Number>());
        return res;
    }
    Object res = Type::STRING.CreateObject();
    res.GetData<String>() = obj1.GetData<String>() * (int)(obj2.GetData<Number>());
    return res;
}

LiteScript::Object LiteScript::_Type_STRING::OEqual(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<String>() == tmp.GetData<String>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<String>() == obj2.GetData<String>());
    return res;
}
LiteScript::Object LiteScript::_Type_STRING::ONotEqual(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<String>() != tmp.GetData<String>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<String>() != obj2.GetData<String>());
    return res;
}
LiteScript::Object LiteScript::_Type_STRING::OGreater(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<String>() > tmp.GetData<String>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<String>() > obj2.GetData<String>());
    return res;
}
LiteScript::Object LiteScript::_Type_STRING::OLess(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<String>() < tmp.GetData<String>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<String>() < obj2.GetData<String>());
    return res;
}
LiteScript::Object LiteScript::_Type_STRING::OGreaterOrEqual(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<String>() >= tmp.GetData<String>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<String>() >= obj2.GetData<String>());
    return res;
}
LiteScript::Object LiteScript::_Type_STRING::OLessOrEqual(const LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object res = Type::BOOLEAN.CreateObject();
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() == *this)
            res.GetData<bool>() = (obj1.GetData<String>() <= tmp.GetData<String>());
        return res;
    }
    res.GetData<bool>() = (obj1.GetData<String>() <= obj2.GetData<String>());
    return res;
}

LiteScript::Object& LiteScript::_Type_STRING::OAddAndAssign(LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != *this) {
        Object tmp = obj2.Convert(*this);
        if (tmp.GetType() != *this) {
            obj1 = Type::NIL.CreateObject();
            return obj1;
        }
        obj1.GetData<String>() += tmp.GetData<String>();
        return obj1;
    }
    obj1.GetData<String>() += obj2.GetData<String>();
    return obj1;
}
LiteScript::Object& LiteScript::_Type_STRING::OMultiplyAndAssign(LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    if (obj2.GetType() != Type::NUMBER) {
        Object tmp = obj2.Convert(Type::NUMBER);
        if (tmp.GetType() != Type::NUMBER) {
            obj1 = Type::NIL.CreateObject();
            return obj1;
        }
        obj1.GetData<String>() *= (int)(tmp.GetData<Number>());
        return obj1;
    }
    obj1.GetData<String>() *= (int)(obj2.GetData<Number>());
    return obj1;
}

LiteScript::Object& LiteScript::_Type_STRING::OArray(LiteScript::Object& obj1, const LiteScript::Object& obj2) const {
    Object tmp = obj2.Convert(Type::NUMBER);
    if (tmp.GetType() != Type::NUMBER)
        return Object::UNDEFINED;
    if ((unsigned int)(int)tmp.GetData<Number>() >= obj1.GetData<String>().GetLength())
        tmp.GetData<Number>() = Number(0);
    ObjectAllocator.destroy((Object *)(&obj1.GetData<char>() + sizeof(Object)));
    ObjectAllocator.construct((Object *)(&obj1.GetData<char>() + sizeof(Object)), Type::CHARACTER, sizeof(Character));
    Object * obj_c = (Object *)(&obj1.GetData<char>() + sizeof(Object));
    ObjectAllocator.construct(&(obj_c->GetData<Character>()), obj1, (unsigned int)(int)tmp.GetData<Number>());
    return *(Object *)(&obj1.GetData<char>() + sizeof(Object));
}
LiteScript::Object& LiteScript::_Type_STRING::OMember(LiteScript::Object& obj, const char * name) const {
    return obj; //A MODIFIER AVEC UN CALLBACK (length, substring, etc.)
}

std::string LiteScript::_Type_STRING::ToString(const LiteScript::Object& obj) const {
    return (std::string)(obj.GetData<String>());
}