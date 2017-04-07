#include "type_null.hpp"

LiteScript::_Type_NIL LiteScript::_type_nil;

LiteScript::_Type_NIL::_Type_NIL() :
    Type("Null")
{

}

LiteScript::Object LiteScript::_Type_NIL::CreateObject() {
    return Object(*this, 0);
}

std::string LiteScript::_Type_NIL::ToString(const Object &) const {
    return "null";
}