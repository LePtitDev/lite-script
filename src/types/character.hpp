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

#ifndef LITESCRIPT_CHARACTER_HPP
#define LITESCRIPT_CHARACTER_HPP

#include "../litescript.hpp"

namespace LiteScript {

    // The derived type CHARACTER (hidden)
    class _Type_CHARACTER : public Type {

    public:

        /////////////////////////
        ////// CONSTRUCTOR //////
        /////////////////////////

        /**
         * Basic constructor of the character type
         */
        _Type_CHARACTER();

        /////////////////////////////
        ////// DERIVED METHODS //////
        /////////////////////////////

        /**
         * Create a character object
         *
         * @return The character object
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

        ////// OPERATIONS //////

        /**
         * Assign an object type character by an other object
         *
         * @param obj1 The character object
         * @param obj2 The other object
         * @return The assigned object
         */
        Object& OAssign(Object& obj1, const Object& obj2) const override;

        /**
         * Apply addition operation between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Object OAdd(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply mutliplication operation between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Object OMultiply(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply equality comparison between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Object OEqual(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply inequality comparison between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Object ONotEqual(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply superiority comparison between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Object OGreater(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply inferiority comparison between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Object OLess(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply superiority or equality comparison between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Object OGreaterOrEqual(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply inferiority or equality comparison between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Object OLessOrEqual(const Object& obj1, const Object& obj2) const override;

        /**
         * Apply addition and assign operation between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The first operand
         */
        Object& OAddAndAssign(Object& obj1, const Object& obj2) const override;

        /**
         * Apply multiplication and assign operation between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The first operand
         */
        Object& OMultiplyAndAssign(Object& obj1, const Object& obj2) const override;

        /**
         * Create a string that contain the value of the character object
         *
         * @param object An object typed CHARACTER
         * @return The string
         */
        std::string ToString(const Object& object) const override;

    };

    // An internal variable that contain the type CHARACTER
    extern _Type_CHARACTER _type_character;

}

#endif //LITESCRIPT_CHARACTER_HPP
