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

#ifndef LITESCRIPT_MEMORY_MEMORY_HPP
#define LITESCRIPT_MEMORY_MEMORY_HPP

#include "type.hpp"

#include "../nullable.hpp"

#define LITESCRIPT_MEMORY_SIZE 256
#define LITESCRIPT_MEMORY_MAXCOUNT 16777216

namespace LiteScript {

    // Class declarations
    class Variable;

    // Class Memory that contain objects
    class Memory {

        ////////////////////////
        ////// ATTRIBUTES //////
        ////////////////////////

        // Array of internal memories
        std::array<void *, LITESCRIPT_MEMORY_SIZE> arr;

        // Array of not full index
        std::array<short, LITESCRIPT_MEMORY_SIZE> nfull;

        // The first not full block
        short first_nfull;

        // Count of objects in the memory
        unsigned int count;

    public:

        //////////////////////
        ////// ACCESSOR //////
        //////////////////////

        // Public accessor of the objects counter
        const unsigned int& Count;

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor of a memory
         */
        Memory();

        /**
         * Destructor of a memory
         */
        ~Memory();

        /////////////////////
        ////// METHODS //////
        /////////////////////

        /**
         * Indicate if the memory is full
         */
        inline bool isFull() const { return this->count == LITESCRIPT_MEMORY_MAXCOUNT; }

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
         * @return The variable if success and null otherwise
         */
        Nullable<Variable> GetVariable(unsigned int id);

        /**
         * Execute the garbage collector
         */
        void GarbageCollector();

    };

}

#endif //LITESCRIPT_MEMORY_HPP