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

#ifndef LITESCRIPT_TYPES_NUMBER_HPP
#define LITESCRIPT_TYPES_NUMBER_HPP

#include "../litescript.hpp"

namespace LiteScript {

    // The derived type NUMBER
    class _Type_NUMBER : public Type {

    public:

        /////////////////////////
        ////// CONSTRUCTOR //////
        /////////////////////////

        /**
         * Basic constructor of the number type
         */
        _Type_NUMBER();

        /////////////////////////////
        ////// DERIVED METHODS //////
        /////////////////////////////

        /**
         * Create a number object
         *
         * @return The number object
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
         * Assign the referenced object by a number object
         *
         * @param object The referenced object
         * @return The same object assigned
         */
        Object& AssignObject(Object& object) override;

        ////// OPERATIONS //////

        /**
         * Assign an object type number by an other object
         *
         * @param obj1 The number object
         * @param obj2 The other object
         * @return The assigned object
         */
        Object& OAssign(Object& obj1, const Object& obj2) const override;

        /**
         * Apply unary plus operation on a referenced object
         *
         * @param object The referenced object
         * @return The result of the operation
         */
        Object OUnaryPlus(const Object& object) const override;

        /**
         * Apply unary minus operation on a referenced object
         *
         * @param object The referenced object
         * @return The result of the operation
         */
        Object OUnaryMinus(const Object& object) const override;

        /**
         * Apply pre-increment operation on a referenced object
         *
         * @param object The referenced object
         * @return The same referenced object
         */
        Object& OPreIncrement(Object& object) const override;

        /**
         * Apply post-increment operation on a referenced object
         *
         * @param object The referenced object
         * @return The result of the operation
         */
        Object OPostIncrement(Object& object) const override;

        /**
         * Apply pre-decrement operation on a referenced object
         *
         * @param object The referenced object
         * @return The same referenced object
         */
        Object& OPreDecrement(Object& object) const override;

        /**
         * Apply post-decrement operation on a referenced object
         *
         * @param object The referenced object
         * @return The result of the operation
         */
        Object OPostDecrement(Object& object) const override;

        /**
         * Apply addition operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Object OAdd(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply substraction operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Object OSubstract(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply multiplication operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Object OMultiply(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply division operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Object ODivide(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply modulo operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Object OModulo(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply equality comparison between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Object OEqual(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply inequality comparison between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Object ONotEqual(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply superiority comparison between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Object OGreater(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply inferiority comparison between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Object OLess(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply superiority or equality comparison between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Object OGreaterOrEqual(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply inferiority or equality comparison between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Object OLessOrEqual(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply binary not operation on a referenced number object
         *
         * @param object The referenced object
         * @return The result of the operation
         */
        Object OBitwiseNot(const Object& object) const override;

        /**
         * Apply binary and operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Object OBitwiseAnd(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply binary or operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Object OBitwiseOr(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply binary xor operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Object OBitwiseXor(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply left shift operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Object OLeftShift(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply right shift operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Object ORightShift(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply addition and assign operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The first operand assigned
         */
        Object& OAddAndAssign(Object& obj1, const Object& obj2) const override;

        /**
         * Apply substraction and assign operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The first operand assigned
         */
        Object& OSubstractAndAssign(Object& obj1, const Object& obj2) const override;

        /**
         * Apply multiplication and assign operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The first operand assigned
         */
        Object& OMultiplyAndAssign(Object& obj1, const Object& obj2) const override;

        /**
         * Apply division and assign operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The first operand assigned
         */
        Object& ODivideAndAssign(Object& obj1, const Object& obj2) const override;

        /**
         * Create a string that contain the value of the number object
         *
         * @param object An object typed NUMBER
         * @return The string
         */
        std::string ToString(const Object& object) const override;

    };

    // An internal variable that contain the type NUMBER
    extern _Type_NUMBER _type_number;

}

#endif //LITESCRIPT_TYPE_NUMBER_HPP
