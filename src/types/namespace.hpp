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
/////////////////////////////////////////////////////////////////////

#ifndef LITESCRIPT_TYPES_NAMESPACE_HPP
#define LITESCRIPT_TYPES_NAMESPACE_HPP

#include "../litescript.hpp"

namespace LiteScript {

    // Derived type NAMESPACE
    class _Type_NAMESPACE : public Type {

    public:

        /////////////////////////
        ////// CONSTRUCTOR //////
        /////////////////////////

        /**
         * Basic constructor of the type NAMESPACE
         */
        _Type_NAMESPACE();

        /////////////////////////////
        ////// DERIVED METHODS //////
        /////////////////////////////

        /**
         * Create an empty object formated by this type
         *
         * @param object An empty object
         */
        void CreateObject(Object& obj) override;

        ////// OPERATIONS //////

        /**
         * Get the member object contained on the referenced object
         *
         * @param object The referenced object
         * @param name The name of the member
         * @return The member object if exist and UNDEFINED object otherwise
         */
        Variable OMember(Variable& object, const char * name) const override;

        /**
         * Create a string that contain the value of the character object
         *
         * @param object An object typed CHARACTER
         * @return The string
         */
        std::string ToString(const Variable& object) const override;

    };

    // An internal variable that contain the type NAMESPACE
    extern _Type_NAMESPACE _type_namespace;

}

#endif //LITESCRIPT_NAMESPACE_HPP
