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

#ifndef LITESCRIPT_TYPES_BOOL_HPP
#define LITESCRIPT_TYPES_BOOL_HPP

#include "../memory/type.hpp"

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
         * Convert the referenced object to an other typed object
         *
         * @param object The referenced object
         * @param type The type of the converted object
         * @return The converted object if success and a null object otherwise
         */
        Variable Convert(const Variable& object, const Type& type) const override;

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
        Variable OAssign(Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply equality comparison between two objects
         *
         * @param obj1 The first operand (boolean object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Variable OEqual(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply inequality comparison between two objects
         *
         * @param obj1 The first operand (boolean object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Variable ONotEqual(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply logical not operation on an object
         *
         * @param obj1 The object (boolean object)
         * @return The result of the operation (boolean object)
         */
        Variable OLogicalNot(const Variable& object) const override;

        /**
         * Apply logical and operation between two objects
         *
         * @param obj1 The first operand (boolean object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Variable OLogicalAnd(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Apply logical or operation between two objects
         *
         * @param obj1 The first operand (boolean object)
         * @param obj2 The second operand
         * @return The result of the operation (boolean object)
         */
        Variable OLogicalOr(const Variable& obj1, const Variable& obj2) const override;

        /**
         * Create a string that contain the value of the boolean object ("true" or "false")
         *
         * @param object An object typed BOOLEAN
         * @return The string
         */
        std::string ToString(const Variable& object) const override;

        /**
         * Save the content of the object in a binary stream
         *
         * @param stream The stream
         * @param object The object to save
         * @param caller Caller to save a variable
         */
        void Save(std::ostream& stream, Object& object, bool (Memory::*caller)(std::ostream&, unsigned int)) const override;

        /**
         * Load the content of the object in a binary stream
         *
         * @param stream The stream
         * @param object The object to load
         * @param caller Caller to load a variable
         */
        void Load(std::istream& stream, Object& object, unsigned int (Memory::*caller)(std::istream&)) const override;

    };

    // An internal variable that contain the type BOOLEAN
    extern _Type_BOOLEAN _type_boolean;

}

#endif //LITESCRIPT_TYPE_BOOL_HPP
