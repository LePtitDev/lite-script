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

#ifndef LITESCRIPT_TYPES_CLASS_HPP
#define LITESCRIPT_TYPES_CLASS_HPP

#include "../litescript.hpp"

namespace LiteScript {

    class _Type_CLASS : public Type {

    public:

        /**
         * Basic constructor
         */
        _Type_CLASS();

        /**
         * Try to assign the objet referenced with default values of this type
         *
         * @param object The object to assign
         * @return The same object referenced
         */
        Object& AssignObject(Object& obj) override;

        /**
         * Frees allocated memory of an object (called when an object is destroyed)
         *
         * @param object The object which will be destroyed
         */
        void ODestroy(Object& object) const override;

        /**
         * Assign an object with the content of an other object
         *
         * @param object_target The object which must be assign
         * @param object_src The object which must be copied
         * @return The object assigned referenced
         */
        Variable OAssign(Variable& object_target, const Variable& object_src) const override;

        /**
         * Return the object contained in the object source and identified by the member name
         *
         * @param object_src The object source
         * @param member_name The member name
         * @return The object contained in the object source if success or the undefined object otherwise
         */
        Variable OMember(Variable& object_src, const char * member_name) const override;

        /**
         * Return a string for describe the content of the object referenced
         *
         * @param object The object to describe
         * @return A description string
         */
        std::string ToString(const Variable& object) const override;

        /**
         * Refer itself and all variables in the referenced object for the garbage collector
         *
         * @param object The object to refer
         * @param caller The calling function for referring
         */
        void GarbageCollector(const Variable& object, void (Memory::*caller)(unsigned int)) const override;

    };

    extern _Type_CLASS _type_class;

}

#endif //LITESCRIPT_CLASS_HPP
