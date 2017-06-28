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

#ifndef LITESCRIPT_TYPES_UNDEFINED_HPP
#define LITESCRIPT_TYPES_UNDEFINED_HPP

#include "../litescript.hpp"

namespace LiteScript {

    // The derived type UNDEFINED (hidden)
    class _Type_UNDEFINED : public Type {

    public:

        /////////////////////////
        ////// CONSTRUCTOR //////
        /////////////////////////

        /**
         * Basic contructor of the undefined type
         */
        _Type_UNDEFINED();

        /////////////////////////////
        ////// DERIVED METHODS //////
        /////////////////////////////

        /**
         * Create an empty object formated by this type
         *
         * @param object An empty object
         */
        void CreateObject(Object& obj) override;

        /**
         * Create a string that contain "undefined"
         *
         * @param object An object typed UNDEFINED (not used)
         * @return The string
         */
        std::string ToString(const Variable& object) const override;

    };

    // An internal variable that contain the type UNDEFINED
    extern _Type_UNDEFINED _type_undefined;

}

#endif //LITESCRIPT_UNDEFINED_HPP
