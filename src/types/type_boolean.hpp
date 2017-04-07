#ifndef LITESCRIPT_TYPE_BOOL_HPP
#define LITESCRIPT_TYPE_BOOL_HPP

#include "../litescript.hpp"

namespace LiteScript {

    class _Type_BOOLEAN : public Type {

    public:

        _Type_BOOLEAN();

        Object CreateObject() override;

        std::string ToString(const Object&) const override;

    };

    extern _Type_BOOLEAN _type_boolean;

}

#endif //LITESCRIPT_TYPE_BOOL_HPP
