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
         * Assign an object with the content of an other object
         *
         * @param object_target The object which must be assign
         * @param object_src The object which must be copied
         * @return The object assigned referenced
         */
        Variable OAssign(Variable& object_target, const Variable& object_src) const override;

        /**
         * Return the result of calling operation
         *
         * @param object The callable object
         * @param args The argument list
         * @return The return result of calling operation
         */
        void OCall(Variable& object, std::vector<Variable>& args) const override;

        /**
         * Return a string for describe the content of the object referenced
         *
         * @param object The object to describe
         * @return A description string
         */
        std::string ToString(const Variable& object) const override;

    };

    extern _Type_CLASS _type_class;

}

#endif //LITESCRIPT_CLASS_HPP
