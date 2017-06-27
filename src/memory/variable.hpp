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

#ifndef LITESCRIPT_LITESCRIPT_HPP

#include "../litescript.hpp"

#else

#ifndef LITESCRIPT_MEMORY_VARIABLE_HPP
#define LITESCRIPT_MEMORY_VARIABLE_HPP

namespace LiteScript {

    class Variable {

        // The memory that contain the object
        Memory& memory;

        // The ID of the object
        unsigned int id;

        // The object
        Object& obj;

        // The counter of variables that refer to this object
        unsigned int& nb_ref;

    public:

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor of a variable
         *
         * @param memory The memory that contain the object
         * @param id The ID of the object
         * @param obj The object
         * @param nb_ref The counter of variables that refer to this object
         */
        Variable(Memory& memory, unsigned int id, Object& obj, unsigned int& nb_ref);

        /**
         * Destructor of a variable
         */
        ~Variable();

        /////////////////////
        ////// METHODS //////
        /////////////////////

        // Pointer operator to an object
        Object* operator->();

    };

}

#endif //LITESCRIPT_MEMORY_VARIABLE_HPP

#endif
