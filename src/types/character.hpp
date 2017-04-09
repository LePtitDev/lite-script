#ifndef LITESCRIPT_CHARACTER_HPP
#define LITESCRIPT_CHARACTER_HPP

#include "../litescript.hpp"

namespace LiteScript {

    class _Type_CHARACTER : public Type {

    public:

        _Type_CHARACTER();

        Object CreateObject() override;
        Object Convert(const Object&, const Type&) const override;

        Object& OAssign(Object&, const Object&) const override;

        Object OAdd(const Object&, const Object&) const override;
        Object OMultiply(const Object&, const Object&) const override;

        Object OEqual(const Object&, const Object&) const override;
        Object ONotEqual(const Object&, const Object&) const override;
        Object OGreater(const Object&, const Object&) const override;
        Object OLess(const Object&, const Object&) const override;
        Object OGreaterOrEqual(const Object&, const Object&) const override;
        Object OLessOrEqual(const Object&, const Object&) const override;

        Object& OAddAndAssign(Object&, const Object&) const override;
        Object& OMultiplyAndAssign(Object&, const Object&) const override;

        std::string ToString(const Object&) const override;

    };

    extern _Type_CHARACTER _type_character;

}

#endif //LITESCRIPT_CHARACTER_HPP
