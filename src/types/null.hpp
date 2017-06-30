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
         * Basic constructor of the null type
         */
        _Type_NIL();

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
         * Convert the referenced object to an other typed object
         *
         * @param object The referenced object
         * @param type The type of the converted object
         * @return The converted object if success and a null object otherwise
         */
        Variable Convert(const Variable& object, const Type& type) const override;

        /**
         * Assign the referenced object by a null object
         *
         * @param object The referenced object
         * @return The same object assigned
         */
        Object& AssignObject(Object& object) override;

        /**
         * Assign an object with the content of an other object
         *
         * @param object_target The object which must be assign
         * @param object_src The object which must be copied
         * @return The object assigned referenced
         */
        Variable OAssign(Variable& object_target, const Variable& object_src) const override;

        /**
         * Apply equality comparison between two objects
         *
         * @param obj1 The first operand
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Variable OEqual(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply inequality comparison between two objects
         *
         * @param obj1 The first operand
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Variable ONotEqual(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply logical not operation on an object (always true)
         *
         * @param obj1 The object
         * @return The result of the operation (boolean object)
         */
        Variable OLogicalNot(const Variable& object) const override;

        /**
         * Apply logical or operation between two objects
         *
         * @param obj1 The first operand
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Variable OBitwiseOr(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Create a string that contain "null"
         *
         * @param object An object typed NULL (not used)
         * @return The string
         */
        std::string ToString(const Variable& object) const override;

    };

    // An internal variable that contain the type NULL
    extern _Type_NIL _type_nil;

}

#endif //LITESCRIPT_TYPES_TYPE_NULL_HPP
