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

#ifndef LITESCRIPT_TYPES_NAMESPACE_HPP
#define LITESCRIPT_TYPES_NAMESPACE_HPP

#include "../memory/type.hpp"

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
         * Assign the referenced object by a namespace object
         *
         * @param object The referenced object
         * @return The same object assigned
         */
        Object& AssignObject(Object& object) override;

        ////// OPERATIONS //////

        /**
         * Frees allocated memory of an object (called when an object is destroyed)
         *
         * @param object The object which will be destroyed
         */
        void ODestroy(Object& object) const override;

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

        /**
         * Refer itself and all variables in the referenced object for the garbage collector
         *
         * @param object The object to refer
         * @param caller The calling function for referring
         */
        void GarbageCollector(const Variable& object, bool (Memory::*caller)(unsigned int)) const override;

    };

    // An internal variable that contain the type NAMESPACE
    extern _Type_NAMESPACE _type_namespace;

}

#endif //LITESCRIPT_NAMESPACE_HPP
