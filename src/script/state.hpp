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

        // The LIFO of namespaces
        std::vector<Variable> nsp_list;

        // The global namespace
        Variable nsp_global;

        // The current namespace
        Nullable<Variable> nsp_current;

        // The LIFO of arguments
        std::vector<std::vector<Variable>> args;

        // The LIFO of returns
        std::vector<Variable> rets;

        // The LIFO of operations
        std::vector<Variable> op_lifo;

        // The LIFO of calling
        std::vector<Callback> call_lifo;

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

        /**
         * Return the count of args in the top of LIFO
         */
        unsigned int GetArgsCount() const;

        /**
         * Get an argument in the top of LIFO
         *
         * @param i The index of the argument
         */
        Variable GetArg(unsigned int i) const;


    };

}

#endif //LITESCRIPT_STATE_HPP

#endif
