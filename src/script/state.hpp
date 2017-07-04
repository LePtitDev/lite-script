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

        // Internal friend static class for instructions execution
        friend class LiteScript::StateExecutor;

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
        std::vector<Variable> nsp_list;

        // The global namespace
        Variable nsp_global;

        // The current namespace
        Nullable<Variable> nsp_current;

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

        /**
         * Execute a single instruction that isn't in the instructions lists
         * (warning: need to be executed after all script execution)
         *
         * @param instr The instruction to execute
         */
        void ExecuteSingle(const Instruction& instr);

        /**
         * Find a variable by its name, search in :
         * - the last namespace
         * - the current namespace
         * - the global if different
         *
         * @param name The name of the variable
         */
        Nullable<Variable> GetVariable(const char * name) const;

        /**
         * Return the current namespace
         */
        Variable GetCurrentNamespace() const;

    };

}

#endif //LITESCRIPT_STATE_HPP

#endif
