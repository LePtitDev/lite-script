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
         * Convert the referenced object to an other typed object
         *
         * @param object The referenced object
         * @param type The type of the converted object
         * @return The converted object if success and a null object otherwise
         */
        Variable Convert(const Variable& object, const Type& type) const override;

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
        Variable OAssign(Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply unary plus operation on a referenced object
         *
         * @param object The referenced object
         * @return The result of the operation
         */
        Variable OUnaryPlus(const Variable& object) const override;

        /**
         * Apply unary minus operation on a referenced object
         *
         * @param object The referenced object
         * @return The result of the operation
         */
        Variable OUnaryMinus(const Variable& object) const override;

        /**
         * Apply pre-increment operation on a referenced object
         *
         * @param object The referenced object
         * @return The same referenced object
         */
        Variable OPreIncrement(Variable& object) const override;

        /**
         * Apply post-increment operation on a referenced object
         *
         * @param object The referenced object
         * @return The result of the operation
         */
        Variable OPostIncrement(Variable& object) const override;

        /**
         * Apply pre-decrement operation on a referenced object
         *
         * @param object The referenced object
         * @return The same referenced object
         */
        Variable OPreDecrement(Variable& object) const override;

        /**
         * Apply post-decrement operation on a referenced object
         *
         * @param object The referenced object
         * @return The result of the operation
         */
        Variable OPostDecrement(Variable& object) const override;

        /**
         * Apply addition operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Variable OAdd(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply substraction operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Variable OSubstract(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply multiplication operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Variable OMultiply(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply division operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Variable ODivide(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply modulo operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Variable OModulo(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply equality comparison between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Variable OEqual(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply inequality comparison between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Variable ONotEqual(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply superiority comparison between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Variable OGreater(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply inferiority comparison between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Variable OLess(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply superiority or equality comparison between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Variable OGreaterOrEqual(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply inferiority or equality comparison between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Variable OLessOrEqual(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply binary not operation on a referenced number object
         *
         * @param object The referenced object
         * @return The result of the operation
         */
        Variable OBitwiseNot(const Variable& object) const override;

        /**
         * Apply binary and operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Variable OBitwiseAnd(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply binary or operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Variable OBitwiseOr(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply binary xor operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Variable OBitwiseXor(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply left shift operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Variable OLeftShift(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply right shift operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Variable ORightShift(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply addition and assign operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The first operand assigned
         */
        Variable OAddAndAssign(Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply substraction and assign operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The first operand assigned
         */
        Variable OSubstractAndAssign(Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply multiplication and assign operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The first operand assigned
         */
        Variable OMultiplyAndAssign(Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply division and assign operation between two objects
         *
         * @param obj1 The first operand (number object)
         * @param obj2 The second operand
         * @return The first operand assigned
         */
        Variable ODivideAndAssign(Variable& obj1, const Variable& obj2) const override;

        /**
         * Create a string that contain the value of the number object
         *
         * @param object An object typed NUMBER
         * @return The string
         */
        std::string ToString(const Variable& object) const override;

    };

    // An internal variable that contain the type NUMBER
    extern _Type_NUMBER _type_number;

}

#endif //LITESCRIPT_TYPE_NUMBER_HPP
