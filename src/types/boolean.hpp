#ifndef LITESCRIPT_TYPES_BOOL_HPP
#define LITESCRIPT_TYPES_BOOL_HPP

#include "../litescript.hpp"

namespace LiteScript {

    class _Type_BOOLEAN : public Type {

    public:

        _Type_BOOLEAN();

        Object CreateObject() override;

        Object Convert(const Object&, const Type&) const override;
        Object& AssignObject(Object&) override;

        Object& OAssign(Object&, const Object&) const override;

        Object OEqual(const Object&, const Object&) const override;
        Object ONotEqual(const Object&, const Object&) const override;

        Object OLogicalNot(const Object&) const override;
        Object OLogicalAnd(const Object&, const Object&) const override;
        Object OLogicalOr(const Object&, const Object&) const override;

        std::string ToString(const Object&) const override;

    };

    extern _Type_BOOLEAN _type_boolean;

}

#endif //LITESCRIPT_TYPE_BOOL_HPP
