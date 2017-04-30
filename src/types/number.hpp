/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/*
    Copyright (C) 2017 LePtitDev
    All rights reserved.

    This software may be modified and distributed under the terms
    of the BSD license.  See the LICENSE file for details.

    Author: Arthur Ferré <leptitdev.com>
*/
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#ifndef LITESCRIPT_TYPES_NUMBER_HPP
#define LITESCRIPT_TYPES_NUMBER_HPP

#include "../litescript.hpp"

namespace LiteScript {

    class _Type_NUMBER : public Type {

    public:

        _Type_NUMBER();

        Object CreateObject() override;

        Object Convert(const Object&, const Type&) const override;
        Object& AssignObject(Object&) override;

        Object& OAssign(Object&, const Object&) const override;

        Object OUnaryPlus(const Object&) const override;
        Object OUnaryMinus(const Object&) const override;
        Object& OPreIncrement(Object&) const override;
        Object OPostIncrement(Object&) const override;
        Object& OPreDecrement(Object&) const override;
        Object OPostDecrement(Object&) const override;

        Object OAdd(const Object&, const Object&) const override;
        Object OSubstract(const Object&, const Object&) const override;
        Object OMultiply(const Object&, const Object&) const override;
        Object ODivide(const Object&, const Object&) const override;
        Object OModulo(const Object&, const Object&) const override;

        Object OEqual(const Object&, const Object&) const override;
        Object ONotEqual(const Object&, const Object&) const override;
        Object OGreater(const Object&, const Object&) const override;
        Object OLess(const Object&, const Object&) const override;
        Object OGreaterOrEqual(const Object&, const Object&) const override;
        Object OLessOrEqual(const Object&, const Object&) const override;

        Object OBitwiseNot(const Object&) const override;
        Object OBitwiseAnd(const Object&, const Object&) const override;
        Object OBitwiseOr(const Object&, const Object&) const override;
        Object OBitwiseXor(const Object&, const Object&) const override;
        Object OLeftShift(const Object&, const Object&) const override;
        Object ORightShift(const Object&, const Object&) const override;

        Object& OAddAndAssign(Object&, const Object&) const override;
        Object& OSubstractAndAssign(Object&, const Object&) const override;
        Object& OMultiplyAndAssign(Object&, const Object&) const override;
        Object& ODivideAndAssign(Object&, const Object&) const override;

        std::string ToString(const Object&) const override;

    };

    extern _Type_NUMBER _type_number;

}

#endif //LITESCRIPT_TYPE_NUMBER_HPP
