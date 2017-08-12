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

#ifndef LITESCRIPT_SCRIPT_STATE_HPP
#define LITESCRIPT_SCRIPT_STATE_HPP

#include <vector>

#include "../nullable.hpp"

#include "../memory/type.hpp"
#include "../memory/variable.hpp"
#include "../memory/memory.hpp"

#include "instruction.hpp"
#include "namer.hpp"

namespace LiteScript {

    // Class declarations
    class StateExecutor;

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

        // The global namespace
        Variable nsp_global;

        // The LIFO of arguments
        std::vector<std::vector<Variable>> args;

        // The LIFO of this
        std::vector<LiteScript::Nullable<LiteScript::Variable>> ths;

        // The LIFO of returns
        std::vector<LiteScript::Nullable<LiteScript::Variable>> rets;

        // The LIFO of operations
        std::vector<Variable> op_lifo;

        // The LIFO of calling
        std::vector<std::array<unsigned int, 2>> call_lifo;

        // The namespace LIFO
        std::vector<Namer> nsp_lifo;

    public:

        // The main memory
        Memory& memory;

        ///////////////////////
        ////// ACCESSORS //////
        ///////////////////////

        // The instructions list index
        const unsigned int& InstructionIndex;

        // The line in the instructions list
        const unsigned int& InstructionLine;

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
         * Copy constructor
         */
        State(const State& state);

        /////////////////////
        ////// METHODS //////
        /////////////////////

        /**
         * Execute all the script
         */
        Variable Execute();

        /**
         * Execute a single instruction
         */
        Variable ExecuteSingle();

        /**
         * Execute a single instruction that isn't in the instructions lists
         *
         * @param instr The instruction to execute
         */
        Variable ExecuteSingle(const Instruction& instr);



        /**
         * Add a single instruction in the current instruction list
         *
         * @param in The instruction to add
         */
        void AddInstruction(const Instruction& in);

        /**
         * Add a list of instructions
         *
         * @param in_list The instruction list to add
         */
        void AddInstructions(const std::vector<Instruction>& in_list);

        /**
         * Get a list of instruction
         *
         * @param i The index of instructions list
         */
        const std::vector<Instruction>& GetInstruction(unsigned int i) const;



        /**
         * Find a variable by its name, search in :
         * - the last namespace
         * - the current namespace
         * - the global if different
         *
         * @param name The name of the variable
         */
        Variable GetVariable(const char * name) const;

        /**
         * Get the current namer
         */
        Namer& GetNamer();

        /**
         * Change the current namespace (if name == "global",
         * the current namespace begin the global)
         *
         * @param name The name of the target namespace
         */
        void UseNamespace(const char * name = "global");



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

        /**
         * Get the current this
         */
        Variable GetThis();

        /**
         * Set the current this
         *
         * @param v The variable to assign
         */
        void SetThis(const Variable& v);

        /**
         * Set the current return
         *
         * @param v The variable to assign
         */
        void SetReturn(const Variable& v);



        /**
         * Add a calling return
         *
         * @param index The index of instructions list
         * @param line The line in the instructions list
         */
        void AddCallback(unsigned int index, unsigned int line);

        /**
         * Remove a calling return
         */
        void RemoveCallback();

    };

}

#endif //LITESCRIPT_STATE_HPP
