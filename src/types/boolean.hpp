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

#ifndef LITESCRIPT_TYPES_BOOL_HPP
#define LITESCRIPT_TYPES_BOOL_HPP

#include "../litescript.hpp"

namespace LiteScript {

    // The derived type BOOLEAN
    class _Type_BOOLEAN : public Type {

    public:

        /////////////////////////
        ////// CONSTRUCTOR //////
        /////////////////////////

        /**
         * Basic constructor of the boolean type
         */
        _Type_BOOLEAN();

        /////////////////////////////
        ////// DERIVED METHODS //////
        /////////////////////////////

        /**
         * Create a boolean object (false is the default assigned value)
         *
         * @return The boolean object
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
         * Assign the referenced object by a boolean object
         *
         * @param object The referenced object
         * @return The same object assigned
         */
        Object& AssignObject(Object& object) override;

        ////// OPERATIONS //////

        /**
         * Assign an object type boolean by an other object
         *
         * @param obj1 The boolean object
         * @param obj2 The other object
         * @return The assigned object
         */
        Object& OAssign(Object& obj1, const Object& obj2) const override;

        /**
         * Apply equality comparison between two objects
         *
         * @param obj1 The first operand (boolean object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Object OEqual(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply inequality comparison between two objects
         *
         * @param obj1 The first operand (boolean object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Object ONotEqual(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply logical not operation on an object
         *
         * @param obj1 The object (boolean object)
         * @return The result of the operation (boolean object)
         */
        Object OLogicalNot(const Object& object) const override;

        /**
         * Apply logical and operation between two objects
         *
         * @param obj1 The first operand (boolean object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Object OLogicalAnd(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply logical or operation between two objects
         *
         * @param obj1 The first operand (boolean object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Object OLogicalOr(const Object& obj1, const Object& obj2) const override;

        /**
         * Create a string that contain the value of the boolean object ("true" or "false")
         *
         * @param object An object typed BOOLEAN
         * @return The string
         */
        std::string ToString(const Object& object) const override;

    };

    // An internal variable that contain the type BOOLEAN
    extern _Type_BOOLEAN _type_boolean;

}

#endif //LITESCRIPT_TYPE_BOOL_HPP
