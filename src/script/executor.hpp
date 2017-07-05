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

#ifndef LITESCRIPT_EXECUTOR_HPP
#define LITESCRIPT_EXECUTOR_HPP

#include "../litescript.hpp"

namespace LiteScript {

    class StateExecutor {

    public:

        ///////////////////////////
        ////// PUBLIC MEMBER //////
        ///////////////////////////

        /**
         * Execute the instruction
         *
         * @param state The script state
         * @param instr The instruction to execute
         */
        static void Execute(State& state, Instruction& instr);

    private:

        /////////////////////////////
        ////// PRIVATE MEMBERS //////
        /////////////////////////////

        // The functions array (for speed execution increasing)
        static std::array<void(*)(State&, Instruction&), LiteScript::InstrCode::INSTR_NUMBER> ARRAY;

        /******
         * All instructions are typed like this :
         * - Parameter: The script state
         * -    Return: No return
         ******/

        // INVALID
        static void I_INVALID(State&, Instruction&);

        // DEFINITIONS
        static void I_DEFINE_VARIABLE(State&, Instruction&);
        static void I_DEFINE_ARG(State&, Instruction&);
        static void I_DEFINE_RETURN(State&, Instruction&);

        // VALUE CREATIONS
        static void I_VALUE_UNDEFINED(State&, Instruction&);
        static void I_VALUE_NULL(State&, Instruction&);
        static void I_VALUE_BOOLEAN(State&, Instruction&);
        static void I_VALUE_NUMBER(State&, Instruction&);
        static void I_VALUE_STRING(State&, Instruction&);
        static void I_VALUE_CALLBACK(State&, Instruction&);
        static void I_VALUE_ARRAY(State&, Instruction&);
        static void I_VALUE_CLASS(State&, Instruction&);
        static void I_VALUE_ARGS(State&, Instruction&);
        static void I_VALUE_THIS(State&, Instruction&);

        // PILES MANAGEMENT
        static void I_PUSH_NSP(State&, Instruction&);
        static void I_PUSH_ARGS(State&, Instruction&);
        static void I_POP_NSP(State&, Instruction&);
        static void I_POP_ARGS(State&, Instruction&);

        // OPERATIONS
        // Assignation et unary operations
        static void I_OP_ASSIGN(State&, Instruction&);
        static void I_OP_UNARY_PLUS(State&, Instruction&);
        static void I_OP_UNARY_MINUS(State&, Instruction&);
        static void I_OP_PRE_INCR(State&, Instruction&);
        static void I_OP_POST_INCR(State&, Instruction&);
        static void I_OP_PRE_DECR(State&, Instruction&);
        static void I_OP_POST_DECR(State&, Instruction&);

        // Arithmetic operations
        static void I_OP_ADD(State&, Instruction&);
        static void I_OP_SUB(State&, Instruction&);
        static void I_OP_MUL(State&, Instruction&);
        static void I_OP_DIV(State&, Instruction&);
        static void I_OP_MOD(State&, Instruction&);

        // Comparison
        static void I_OP_EQU(State&, Instruction&);
        static void I_OP_DIF(State&, Instruction&);
        static void I_OP_GREAT(State&, Instruction&);
        static void I_OP_LESS(State&, Instruction&);
        static void I_OP_GREAT_EQU(State&, Instruction&);
        static void I_OP_LESS_EQU(State&, Instruction&);

        // Logical operation
        static void I_OP_LOG_NOT(State&, Instruction&);
        static void I_OP_LOG_AND(State&, Instruction&);
        static void I_OP_LOG_OR(State&, Instruction&);

        // Binary operations
        static void I_OP_BIT_NOT(State&, Instruction&);
        static void I_OP_BIT_AND(State&, Instruction&);
        static void I_OP_BIT_OR(State&, Instruction&);
        static void I_OP_BIT_XOR(State&, Instruction&);
        static void I_OP_LSHIFT(State&, Instruction&);
        static void I_OP_RSHIFT(State&, Instruction&);

        // Arithmetic and assignation
        static void I_OP_ADD_ASSIGN(State&, Instruction&);
        static void I_OP_SUB_ASSIGN(State&, Instruction&);
        static void I_OP_MUL_ASSIGN(State&, Instruction&);
        static void I_OP_DIV_ASSIGN(State&, Instruction&);

        // Special operations
        static void I_OP_ARRAY(State&, Instruction&);
        static void I_OP_MEMBER(State&, Instruction&);
        static void I_OP_CALL(State&, Instruction&);

        // CONTROL INSTRUCTIONS
        static void I_JUMP_TO(State&, Instruction&);
        static void I_JUMP_IF(State&, Instruction&);
        static void I_JUMP_ELSE(State&, Instruction&);

        // COMPLEX VALUES COMPLETION
        // Array
        static void I_ARRAY_PUSH_NUMERIC(State&, Instruction&);
        static void I_ARRAY_PUSH_LITERAL(State&, Instruction&);

        // Class
        static void I_CLASS_PUSH_STATIC(State&, Instruction&);
        static void I_CLASS_PUSH_USTATIC(State&, Instruction&);
        static void I_CLASS_PUSH_OPERATOR(State&, Instruction&);
        static void I_CLASS_INHERIT(State&, Instruction&);

        // NAMESPACES
        static void I_NAMESPACE_USE(State&, Instruction&);

    };

}

#endif //LITESCRIPT_EXECUTOR_HPP
