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

        ////// OPERATIONS //////

        /**
         * Assign an object type character by an other object
         *
         * @param obj1 The character object
         * @param obj2 The other object
         * @return The assigned object
         */
        Variable OAssign(Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply addition operation between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Variable OAdd(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply mutliplication operation between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The result of the operation
         */
        Variable OMultiply(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply equality comparison between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Variable OEqual(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply inequality comparison between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Variable ONotEqual(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply superiority comparison between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Variable OGreater(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply inferiority comparison between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Variable OLess(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply superiority or equality comparison between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Variable OGreaterOrEqual(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply inferiority or equality comparison between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Variable OLessOrEqual(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply addition and assign operation between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The first operand
         */
        Variable OAddAndAssign(Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply multiplication and assign operation between two objects
         *
         * @param obj1 The first operand (character object)
         * @param obj2 The second operand
         * @return The first operand
         */
        Variable OMultiplyAndAssign(Variable& obj1, const Variable& obj2) const override;

        /**
         * Create a string that contain the value of the character object
         *
         * @param object An object typed CHARACTER
         * @return The string
         */
        std::string ToString(const Variable& object) const override;

    };

    // An internal variable that contain the type CHARACTER
    extern _Type_CHARACTER _type_character;

}

#endif //LITESCRIPT_CHARACTER_HPP
