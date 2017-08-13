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

#include "../memory/type.hpp"

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
         * Assign the referenced object by an undefined object
         *
         * @param object The referenced object
         * @return The same object assigned
         */
        Object& AssignObject(Object& object) override;

        /**
         * Convert the referenced object to an other typed object
         *
         * @param object The referenced object
         * @param type The type of the converted object
         * @return The converted object if success and a null object otherwise
         */
        Variable Convert(const Variable& object, const Type& type) const override;

        ////// OPERATIONS //////

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
