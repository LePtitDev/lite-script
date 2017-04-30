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

#ifndef LITESCRIPT_TYPES_NULL_HPP
#define LITESCRIPT_TYPES_NULL_HPP

#include "../litescript.hpp"

namespace LiteScript {

    // The derived type NULL
    class _Type_NIL : public Type {

    public:

        /////////////////////////
        ////// CONSTRUCTOR //////
        /////////////////////////

        /**
         * Basic constructor of a null object
         */
        _Type_NIL();

        /////////////////////////////
        ////// DERIVED METHODS //////
        /////////////////////////////

        /**
         * Create a null object
         *
         * @return The null object
         */
        Object CreateObject() override;

        /**
         * Convert the referenced object to an other typed object
         *
         * @param object The referenced object
         * @param type The type of the converted object
         * @return The converted object if success and a null object otherwise
         */
        Object Convert(const Object& object, const Type& type) const override;

        /**
         * Assign the referenced object by a null object
         *
         * @param object The referenced object
         * @return The same object assigned
         */
        Object& AssignObject(Object& object) override;

        /**
         * Create a string that contain "null"
         *
         * @param object An object typed NULL (not used)
         * @return The string
         */
        std::string ToString(const Object& object) const override;

    };

    // An internal variable that contain the type NULL
    extern _Type_NIL _type_nil;

}

#endif //LITESCRIPT_TYPES_TYPE_NULL_HPP
