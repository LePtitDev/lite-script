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
/////////////////////////////////////////////////////////////////////

#ifndef LITESCRIPT_LITESCRIPT_HPP

#include "../litescript.hpp"

#else

#ifndef LITESCRIPT_SCRIPT_STATE_HPP
#define LITESCRIPT_SCRIPT_STATE_HPP

namespace LiteScript {

    class State {

        ////////////////////////
        ////// ATTRIBUTES //////
        ////////////////////////

        // Multiple list of instructions
        std::vector<std::vector<Instruction>> instr;

        // The index of current instructions
        unsigned int instr_index;

        // The current line number
        unsigned int line_num;

        // The pile of major namespaces
        std::vector<Variable> nsp_major;

        // The pile of minor namespaces
        std::vector<Variable> nsp_minor;

        // The current namespace as object type
        Variable nsp_current;

        // The pile of arguments
        std::vector<std::vector<Variable>> args;

        // The pile of returns
        std::vector<Variable> rets;

    public:

        // The main memory
        Memory& memory;

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor
         *
         * @param memory The main memory
         */
        State(Memory& memory);

        /**
         * Constructor with instructions
         *
         * @param memory The main memory
         * @param instrs The list of instructions
         */
        State(Memory& memory, std::vector<Instruction> instrs);

        /////////////////////
        ////// METHODS //////
        /////////////////////

        /**
         * Execute all the script
         */
        void Execute();

        /**
         * Execute a single instruction
         */
        void ExecuteSingle();

    };

}

#endif //LITESCRIPT_STATE_HPP

#endif
