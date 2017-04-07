#include "type_boolean.hpp"

LiteScript::_Type_BOOLEAN LiteScript::_type_boolean;

LiteScript::_Type_BOOLEAN::_Type_BOOLEAN() :
    Type("Boolean")
{

}

LiteScript::Object LiteScript::_Type_BOOLEAN::CreateObject() {
    Object res(Type::BOOLEAN, sizeof(bool));
    res.GetData<bool>() = false;
    return res;
}

std::string LiteScript::_Type_BOOLEAN::ToString(const Object & obj) const {
    return ((obj.GetData<bool>()) ? "true" : "false");
}
