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

#ifndef LITESCRIPT_BASIC_MEMORY_HPP
#define LITESCRIPT_BASIC_MEMORY_HPP

#define LITESCRIPT_MEMORY_1_MAXCOUNT 65536

#include "../litescript.hpp"

namespace LiteScript {

    // Internal element of memory
    class _BasicMemory_1 {

        ////////////////////////
        ////// ATTRIBUTES //////
        ////////////////////////

        // The object counter
        unsigned int count;

    public:

        // The main memory
        Memory& memory;

        ///////////////////////
        ////// ACCESSORS //////
        ///////////////////////

        // Public accessor of the object counter
        const unsigned int& Count;

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor of the basic memory
         *
         * @param memory The main memory
         */
        _BasicMemory_1(Memory& memory);

        /**
         * Destructor of the memory
         */
        ~_BasicMemory_1();

        /////////////////////
        ////// METHODS //////
        /////////////////////

        /**
         * Indicate if the memory is full
         */
        inline bool isFull() const { return this->count == LITESCRIPT_MEMORY_1_MAXCOUNT; }

        /**
         * Create a null object
         *
         * @param type The type of the object
         * @return The variable that refer to the object
         */
        Variable Create(Type& type = Type::NIL);

        /**
         * Remove an object
         *
         * @param id The ID of the object
         */
        void Remove(unsigned int id);

        /**
         * Get a variable that refer to an object
         *
         * @param id The ID of the object
         * @return The variable
         */
        Nullable<Variable> GetVariable(unsigned int id);

    };

}

#endif //LITESCRIPT_BASIC_MEMORY_HPP
