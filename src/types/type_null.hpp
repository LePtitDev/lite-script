#ifndef LITESCRIPT_TYPES_TYPE_NULL_HPP
#define LITESCRIPT_TYPES_TYPE_NULL_HPP

#include "../litescript.hpp"

namespace LiteScript {

    class _Type_NIL : public Type {

    public:

        _Type_NIL();

        Object CreateObject() override;

        std::string ToString(const Object&) const override;

    };

    extern _Type_NIL _type_nil;

}

#endif //LITESCRIPT_TYPES_TYPE_NULL_HPP
