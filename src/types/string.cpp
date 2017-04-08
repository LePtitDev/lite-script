#include "string.hpp"

LiteScript::_Type_STRING _type_string;

LiteScript::_Type_STRING::_Type_STRING() : Type("String") {};

LiteScript::Object LiteScript::_Type_STRING::CreateObject() {
    Object res(*this, sizeof(String));
    ObjectAllocator.construct(&res.GetData<String>());
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
    obj.Reassign(*this, sizeof(String));
    ObjectAllocator.construct(&obj.GetData<String>());
}

void LiteScript::_Type_STRING::ODestroy(LiteScript::Object& obj) {
    ObjectAllocator.destroy(&obj.GetData<String>());
}

LiteScript::Object& LiteScript::_Type_STRING::OAssign(LiteScript::Object&, const LiteScript::Object&) const {

}

LiteScript::Object LiteScript::_Type_STRING::OAdd(const LiteScript::Object&, const LiteScript::Object&) const;
LiteScript::Object LiteScript::_Type_STRING::OMultiply(const LiteScript::Object&, const LiteScript::Object&) const;

LiteScript::Object LiteScript::_Type_STRING::OEqual(const LiteScript::Object&, const LiteScript::Object&) const;
LiteScript::Object LiteScript::_Type_STRING::ONotEqual(const LiteScript::Object&, const LiteScript::Object&) const;
LiteScript::Object LiteScript::_Type_STRING::OGreater(const LiteScript::Object&, const LiteScript::Object&) const;
LiteScript::Object LiteScript::_Type_STRING::OLess(const LiteScript::Object&, const LiteScript::Object&) const;
LiteScript::Object LiteScript::_Type_STRING::OGreaterOrEqual(const LiteScript::Object&, const LiteScript::Object&) const;
LiteScript::Object LiteScript::_Type_STRING::OLessOrEqual(const LiteScript::Object&, const LiteScript::Object&) const;

LiteScript::Object& LiteScript::_Type_STRING::OAddAndAssign(LiteScript::Object&, const LiteScript::Object&) const;
LiteScript::Object& LiteScript::_Type_STRING::OMultiplyAndAssign(LiteScript::Object&, const LiteScript::Object&) const;

LiteScript::Object& LiteScript::_Type_STRING::OArray(LiteScript::Object&, const LiteScript::Object&) const;
LiteScript::Object& LiteScript::_Type_STRING::OMember(LiteScript::Object&, const char *) const;

std::string LiteScript::_Type_STRING::ToString(const LiteScript::Object&) const;