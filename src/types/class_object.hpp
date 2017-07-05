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

#ifndef LITESCRIPT_CLASS_OBJECT_HPP
#define LITESCRIPT_CLASS_OBJECT_HPP

#include "../litescript.hpp"

namespace LiteScript {

    // The derived type CLASS OBJECT
    class _Type_CLASS_OBJECT : public Type {

    public:

        /**
         * Basic constructor
         */
        _Type_CLASS_OBJECT();

        /**
         * Try to assign the objet referenced with default values of this type
         *
         * @param object The object to assign
         * @return The same object referenced
         */
        Object& AssignObject(Object& obj) override;

        /**
         * Return a string for describe the content of the object referenced
         *
         * @param object The object to describe
         * @return A description string
         */
        std::string ToString(const Variable& object) const override;

    };

    extern _Type_CLASS_OBJECT _type_class_object;

}

#endif //LITESCRIPT_CLASS_OBJECT_HPP
