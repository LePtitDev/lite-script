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

#ifndef LITESCRIPT_STATE_HPP
#define LITESCRIPT_STATE_HPP

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

    public:

        // The main memory
        Memory& memory;

        ////////////////////////
        ////// ACCESSSORS //////
        ////////////////////////

        // The index of current instructions
        const unsigned int& CurrentInstructions;

        // The current line number
        const unsigned int& CurrentLine;

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

        /**
         * Jump to an other instruction
         *
         * @param intr The index of instructions
         * @param line The line of the instruction
         */
        void JumpTo(unsigned int intr, unsigned int line);

    };

}

#endif //LITESCRIPT_STATE_HPP

#endif
