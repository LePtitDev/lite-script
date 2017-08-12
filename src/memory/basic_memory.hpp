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

#define LITESCRIPT_MEMORY_0_SIZE 256
#define LITESCRIPT_MEMORY_0_MAXCOUNT 256
#define LITESCRIPT_MEMORY_1_SIZE 256
#define LITESCRIPT_MEMORY_1_MAXCOUNT 65536

#include "memory.hpp"

namespace LiteScript {

    // Internal element of memory level 0
    class _BasicMemory_0 {

        ////////////////////////
        ////// ATTRIBUTES //////
        ////////////////////////

        // The object array
        unsigned char arr[LITESCRIPT_MEMORY_0_SIZE * sizeof(Object)];

        // The reference counters array
        std::array<unsigned int, LITESCRIPT_MEMORY_0_SIZE> ref_cpt;

        // The free blocks array
        std::array<short, LITESCRIPT_MEMORY_0_SIZE> free;

        // The first free block
        short first_free;

        // The count of objects
        unsigned int count;

        // Object allocator
        std::allocator<Object> allocator;

        // Flags
        std::array<bool, LITESCRIPT_MEMORY_0_SIZE> flags;

    public:

        // The main memory
        Memory& memory;

        /////////////////////
        ////// ACESSOR //////
        /////////////////////

        // Public accessor to the objects counter
        const unsigned int& Count;

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor of a bassic memory L0
         *
         * @param mem The main memory
         */
        _BasicMemory_0(Memory& mem);

        /**
         * Destructor of the basic memory
         */
        ~_BasicMemory_0();

        /////////////////////
        ////// METHODS //////
        /////////////////////

        /**
         * Indicate if the memory is full
         */
        inline bool isFull() const { return this->count == LITESCRIPT_MEMORY_0_MAXCOUNT; }

        /**
         * Create a null object
         *
         * @param type The type of the object
         * @param id The ID of the object
         * @return The variable that refer to the object
         */
        Variable Create(Type& type, unsigned int id);

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

    private:

        // Operator for access to a variable
        inline Object& operator[](unsigned int index) {
            return ((Object *)this->arr)[index];
        }

    public:

        /**
         * Initialise garbage collector flags
         */
        void FlagsInit();

        /**
         * Protect an object of the erasing
         *
         * @param i The index of the object
         */
        void FlagsProtect(unsigned int i);

        /**
         * Apply the garbage collector
         */
        void FlagsErase();

    };

    // Internal element of memory level 1
    class _BasicMemory_1 {

        ////////////////////////
        ////// ATTRIBUTES //////
        ////////////////////////

        // Array of internal memories
        std::array<LiteScript::_BasicMemory_0 *, LITESCRIPT_MEMORY_1_SIZE> arr;

        // Array of not full index
        std::array<short, LITESCRIPT_MEMORY_1_SIZE> nfull;

        // The first not full block
        short first_nfull;

        // Count of objects in the memory
        unsigned int count;

    public:

        // The main memory
        Memory& memory;

        //////////////////////
        ////// ACCESSOR //////
        //////////////////////

        // Public accessor of the object counter
        const unsigned int& Count;

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor of the basic memory L1
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
         * @param id The ID of the object
         * @return The variable that refer to the object
         */
        Variable Create(Type& type, unsigned int id);

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

        /**
         * Initialise garbage collector flags
         */
        void FlagsInit();

        /**
         * Protect an object of the erasing
         *
         * @param i The index of the object
         */
        void FlagsProtect(unsigned int i);

        /**
         * Apply the garbage collector
         */
        void FlagsErase();

    };

}

#endif //LITESCRIPT_BASIC_MEMORY_HPP
