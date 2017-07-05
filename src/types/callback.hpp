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

#ifndef LITESCRIPT_TYPES_CALLBACK_HPP
#define LITESCRIPT_TYPES_CALLBACK_HPP

#include "../litescript.hpp"

namespace LiteScript {

    class _Type_CALLBACK : public Type {

    public:

        _Type_CALLBACK();

        /////////////////////////////
        ////// DERIVED METHODS //////
        /////////////////////////////

        /**
         * Convert the referenced object to an other typed object
         *
         * @param object The referenced object
         * @param type The type of the converted object
         * @return The converted object if success and a null object otherwise
         */
        Variable Convert(const Variable& object, const Type& type) const override;

        /**
         * Assign the referenced object by a callback object
         *
         * @param object The referenced object
         * @return The same object assigned
         */
        Object& AssignObject(Object& object) override;

        ////// OPERATIONS //////

        /**
         * Assign an object type callback by an other object
         *
         * @param obj1 The callback object
         * @param obj2 The other object
         * @return The assigned object
         */
        Variable OAssign(Variable& obj1, const Variable& obj2) const override;

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
         * Apply logical not operation on an object (always false)
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
         * @return The result of the operation (boolean object)
         */
        Variable OBitwiseOr(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Return the result of calling operation
         *
         * @param object The callable object
         * @param args The argument list
         * @return The return result of calling operation
         */
        Variable OCall(Variable& object, std::vector<Variable>& args) const override;

        /**
         * Create a string that contain the reference of the callback object
         *
         * @param object An object typed CALLBACK
         * @return The string
         */
        std::string ToString(const Variable& object) const override;

    };

    // An internal variable that contain the type CALLBACK
    extern _Type_CALLBACK _type_callback;

}

#endif //LITESCRIPT_CALLBACK_HPP
