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

#ifndef LITESCRIPT_SCRIPT_NAMER_HPP
#define LITESCRIPT_SCRIPT_NAMER_HPP

#include "../memory/variable.hpp"

#include "../nullable.hpp"

namespace LiteScript {

    // Namespaces manager
    class Namer {

        ////////////////////////
        ////// ATTRIBUTES //////
        ////////////////////////

        // The global namespace
        Variable global;

        // The current global namespace
        Nullable<Variable> current;

        // Callbacks and classes heap
        std::vector<Variable> heap;

    public:

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor
         *
         * @param global The global namespace
         */
        Namer(const Variable& global);

        /**
         * Copy constructor
         *
         * @param n The namer to copy
         */
        Namer(const Namer& n);

        /////////////////////
        ////// METHODS //////
        /////////////////////

        /**
         * Use an other global namespace
         *
         * @param nsp The namespace to use
         */
        void Use(const Variable& nsp);

        /**
         * Push a namespace/class on the heap
         *
         * @param nsp The namespace or class
         */
        void Push(const Variable& nsp);

        /**
         * Pop a namespace in the heap
         *
         * @param nsp The nsp to pop
         */
        void Pop(const Variable& nsp);

        /**
         * Get a variable by its name
         *
         * @param name The name of the variable
         */
        Variable Get(const char * name) const;

        /**
         * Declare a variable
         *
         * @param name The name of the variable
         * @param v The variable
         */
        void Declare(const char * name, const Variable& v);

        // The assign operator overloading
        Namer& operator=(const Namer& n);

    };

}

#endif //LITESCRIPT_NAMER_HPP