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

#ifndef LITESCRIPT_TYPES_CLASS_HPP
#define LITESCRIPT_TYPES_CLASS_HPP

#include "../litescript.hpp"

namespace LiteScript {

    class _Type_CLASS : public Type {

    public:

        _Type_CLASS();

        Object& AssignObject(Object& obj) override;

    };

    extern _Type_CLASS _type_class;

}

#endif //LITESCRIPT_CLASS_HPP
