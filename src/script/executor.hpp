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
        static std::array<void(*)(State&), LiteScript::InstrCode::INSTR_NUMBER> ARRAY;

        /******
         * All instructions are typed like this :
         * - Parameter: The script state
         * -    Return: No return
         ******/

        // INVALID
        static void I_INVALID(State&);

        // DEFINITIONS
        static void I_DEFINE_VARIABLE(State&);
        static void I_DEFINE_ARG(State&);
        static void I_DEFINE_RETURN(State&);

        // VALUE CREATIONS
        static void I_VALUE_UNDEFINED(State&);
        static void I_VALUE_NULL(State&);
        static void I_VALUE_BOOLEAN(State&);
        static void I_VALUE_NUMBER(State&);
        static void I_VALUE_STRING(State&);
        static void I_VALUE_CALLBACK(State&);
        static void I_VALUE_ARRAY(State&);
        static void I_VALUE_OBJECT(State&);
        static void I_VALUE_CLASS(State&);
        static void I_VALUE_NAME(State&);
        static void I_VALUE_ARGS(State&);

        // PILES MANAGEMENT
        static void I_PUSH_MAJOR(State&);
        static void I_PUSH_MINOR(State&);
        static void I_PUSH_ARGS(State&);
        static void I_POP_MAJOR(State&);
        static void I_POP_MINOR(State&);

        // OPERATIONS
        // Assignation et unary operations
        static void I_OP_ASSIGN(State&);
        static void I_OP_UNARY_PLUS(State&);
        static void I_OP_UNARY_MINUS(State&);
        static void I_OP_PRE_INCR(State&);
        static void I_OP_POST_INCR(State&);
        static void I_OP_PRE_DECR(State&);
        static void I_OP_POST_DECR(State&);

        // Arithmetic operations
        static void I_OP_ADD(State&);
        static void I_OP_SUB(State&);
        static void I_OP_MUL(State&);
        static void I_OP_DIV(State&);
        static void I_OP_MOD(State&);

        // Comparison
        static void I_OP_EQU(State&);
        static void I_OP_DIF(State&);
        static void I_OP_GREAT(State&);
        static void I_OP_LESS(State&);
        static void I_OP_GREAT_EQU(State&);
        static void I_OP_LESS_EQU(State&);

        // Logical operation
        static void I_OP_LOG_NOT(State&);
        static void I_OP_LOG_AND(State&);
        static void I_OP_LOG_OR(State&);

        // Binary operations
        static void I_OP_BIT_NOT(State&);
        static void I_OP_BIT_AND(State&);
        static void I_OP_BIT_OR(State&);
        static void I_OP_BIT_XOR(State&);
        static void I_OP_LSHIFT(State&);
        static void I_OP_RSHIFT(State&);

        // Arithmetic and assignation
        static void I_OP_ADD_ASSIGN(State&);
        static void I_OP_SUB_ASSIGN(State&);
        static void I_OP_MUL_ASSIGN(State&);
        static void I_OP_DIV_ASSIGN(State&);

        // Special operations
        static void I_OP_ARRAY(State&);
        static void I_OP_MEMBER(State&);
        static void I_OP_CALL(State&);

        // CONTROL INSTRUCTIONS
        static void I_JUMP_TO(State&);
        static void I_JUMP_IF(State&);
        static void I_JUMP_ELSE(State&);

        // COMPLEX VALUES COMPLETION
        // Array
        static void I_ARRAY_PUSH(State&);

        // Object
        static void I_OBJECT_PUSH_NUMERIC(State&);
        static void I_OBJECT_PUSH_LITERAL(State&);

        // Class
        static void I_CLASS_PUSH_STATIC(State&);
        static void I_CLASS_PUSH_USTATIC(State&);
        static void I_CLASS_INHERIT(State&);

        // NAMESPACES
        static void I_NAMESPACE_USE(State&);
        static void I_NAMESPACE_RESET(State&);

    };

}

#endif //LITESCRIPT_EXECUTOR_HPP
