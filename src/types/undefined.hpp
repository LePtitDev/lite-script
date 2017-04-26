#ifndef LITESCRIPT_TYPES_UNDEFINED_HPP
#define LITESCRIPT_TYPES_UNDEFINED_HPP

#include "../litescript.hpp"

namespace LiteScript {

    class _Type_UNDEFINED : public Type {

    public:

        _Type_UNDEFINED();

        Object CreateObject() override;

        std::string ToString(const Object&) const override;

    };

    extern _Type_UNDEFINED _type_undefined;

}

#endif //LITESCRIPT_UNDEFINED_HPP
