#include "undefined.hpp"

LiteScript::_Type_UNDEFINED LiteScript::_type_undefined;

LiteScript::_Type_UNDEFINED::_Type_UNDEFINED() : Type("UNDEFINED") {}

LiteScript::Object LiteScript::_Type_UNDEFINED::CreateObject() {
    return Object(_type_undefined, 0);
}

std::string LiteScript::_Type_UNDEFINED::ToString(const LiteScript::Object& obj) const {
    return "";
}
