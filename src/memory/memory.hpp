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

#ifndef LITESCRIPT_MEMORY_MEMORY_HPP
#define LITESCRIPT_MEMORY_MEMORY_HPP

namespace LiteScript {

    // Class Memory that contain objects
    class Memory {

        ////////////////////////
        ////// ATTRIBUTES //////
        ////////////////////////

        // Array of internal memories
        std::array<void *, 256> arr;

        // Array of free index
        std::array<short, 256> free;

        // Array of not full index
        std::array<short, 256> nfull;

        // The first free block
        short first_free;

        // The first not full block
        short first_ufull;

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
         * Create a null object
         *
         * @return The variable that refer to the object
         */
        Variable CreateObject();

        /**
         * Remove an object
         *
         * @param id The ID of the object
         */
        void RemoveObject(unsigned int id);

        /**
         * Get a variable that refer to an object
         *
         * @param id The ID of the object
         * @return The variable
         */
        Variable GetVariable(unsigned int id);

    };

}

#endif //LITESCRIPT_MEMORY_HPP

#endif
