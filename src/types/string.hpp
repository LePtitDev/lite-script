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

#ifndef LITESCRIPT_STRING_HPP
#define LITESCRIPT_STRING_HPP

#include "../litescript.hpp"

namespace LiteScript {

    class _Type_STRING : public Type {

    public:

        _Type_STRING();

        Object CreateObject() override;

        Object Convert(const Object&, const Type&) const override;
        Object& AssignObject(Object&) override;

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

        Object& OArray(Object&, const Object&) const override;
        Object& OMember(Object&, const char *) const override;

        std::string ToString(const Object&) const override;

    };

    extern _Type_STRING _type_string;

}

#endif //LITESCRIPT_STRING_HPP
