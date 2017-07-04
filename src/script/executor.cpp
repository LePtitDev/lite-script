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

#include "executor.hpp"

void LiteScript::StateExecutor::Execute(State &state, Instruction &instr) {
    StateExecutor::ARRAY[instr.code](state);
}

// THE FUNCTION ARRAY
std::array<void(*)(LiteScript::State&), LiteScript::InstrCode::INSTR_NUMBER> LiteScript::StateExecutor::ARRAY({
    LiteScript::StateExecutor::I_INVALID,

    LiteScript::StateExecutor::I_DEFINE_VARIABLE,
    LiteScript::StateExecutor::I_DEFINE_ARG,
    LiteScript::StateExecutor::I_DEFINE_RETURN,

    LiteScript::StateExecutor::I_VALUE_UNDEFINED,
    LiteScript::StateExecutor::I_VALUE_NULL,
    LiteScript::StateExecutor::I_VALUE_BOOLEAN,
    LiteScript::StateExecutor::I_VALUE_NUMBER,
    LiteScript::StateExecutor::I_VALUE_STRING,
    LiteScript::StateExecutor::I_VALUE_CALLBACK,
    LiteScript::StateExecutor::I_VALUE_ARRAY,
    LiteScript::StateExecutor::I_VALUE_OBJECT,
    LiteScript::StateExecutor::I_VALUE_CLASS,
    LiteScript::StateExecutor::I_VALUE_NAME,
    LiteScript::StateExecutor::I_VALUE_ARGS,
    
    LiteScript::StateExecutor::I_PUSH_MAJOR,
    LiteScript::StateExecutor::I_PUSH_MINOR,
    LiteScript::StateExecutor::I_PUSH_ARGS,
    LiteScript::StateExecutor::I_POP_MAJOR,
    LiteScript::StateExecutor::I_POP_MINOR,
    
    LiteScript::StateExecutor::I_OP_ASSIGN,
    LiteScript::StateExecutor::I_OP_UNARY_PLUS,
    LiteScript::StateExecutor::I_OP_UNARY_MINUS,
    LiteScript::StateExecutor::I_OP_PRE_INCR,
    LiteScript::StateExecutor::I_OP_POST_INCR,
    LiteScript::StateExecutor::I_OP_PRE_DECR,
    LiteScript::StateExecutor::I_OP_POST_DECR,
    
    LiteScript::StateExecutor::I_OP_ADD,
    LiteScript::StateExecutor::I_OP_SUB,
    LiteScript::StateExecutor::I_OP_MUL,
    LiteScript::StateExecutor::I_OP_DIV,
    LiteScript::StateExecutor::I_OP_MOD,
    
    LiteScript::StateExecutor::I_OP_EQU,
    LiteScript::StateExecutor::I_OP_DIF,
    LiteScript::StateExecutor::I_OP_GREAT,
    LiteScript::StateExecutor::I_OP_LESS,
    LiteScript::StateExecutor::I_OP_GREAT_EQU,
    LiteScript::StateExecutor::I_OP_LESS_EQU,
    
    LiteScript::StateExecutor::I_OP_LOG_NOT,
    LiteScript::StateExecutor::I_OP_LOG_AND,
    LiteScript::StateExecutor::I_OP_LOG_OR,
    
    LiteScript::StateExecutor::I_OP_BIT_NOT,
    LiteScript::StateExecutor::I_OP_BIT_AND,
    LiteScript::StateExecutor::I_OP_BIT_OR,
    LiteScript::StateExecutor::I_OP_BIT_XOR,
    LiteScript::StateExecutor::I_OP_LSHIFT,
    LiteScript::StateExecutor::I_OP_RSHIFT,
    
    LiteScript::StateExecutor::I_OP_ADD_ASSIGN,
    LiteScript::StateExecutor::I_OP_SUB_ASSIGN,
    LiteScript::StateExecutor::I_OP_MUL_ASSIGN,
    LiteScript::StateExecutor::I_OP_DIV_ASSIGN,
    
    LiteScript::StateExecutor::I_OP_ARRAY,
    LiteScript::StateExecutor::I_OP_MEMBER,
    LiteScript::StateExecutor::I_OP_CALL,
    
    LiteScript::StateExecutor::I_JUMP_TO,
    LiteScript::StateExecutor::I_JUMP_IF,
    LiteScript::StateExecutor::I_JUMP_ELSE,
    
    LiteScript::StateExecutor::I_ARRAY_PUSH,

    LiteScript::StateExecutor::I_OBJECT_PUSH_NUMERIC,
    LiteScript::StateExecutor::I_OBJECT_PUSH_LITERAL,

    LiteScript::StateExecutor::I_CLASS_PUSH_STATIC,
    LiteScript::StateExecutor::I_CLASS_PUSH_USTATIC,
    LiteScript::StateExecutor::I_CLASS_INHERIT,

    LiteScript::StateExecutor::I_NAMESPACE_USE,
    LiteScript::StateExecutor::I_NAMESPACE_RESET
});

/////////////////////////////
////// PRIVATE METHODS //////
/////////////////////////////

// INVALID
void LiteScript::StateExecutor::I_INVALID(State& state) {}

// DEFINITIONS
void LiteScript::StateExecutor::I_DEFINE_VARIABLE(State& state) {}
void LiteScript::StateExecutor::I_DEFINE_ARG(State& state) {}
void LiteScript::StateExecutor::I_DEFINE_RETURN(State& state) {}

// VALUE CREATIONS
void LiteScript::StateExecutor::I_VALUE_UNDEFINED(State& state) {}
void LiteScript::StateExecutor::I_VALUE_NULL(State& state) {}
void LiteScript::StateExecutor::I_VALUE_BOOLEAN(State& state) {}
void LiteScript::StateExecutor::I_VALUE_NUMBER(State& state) {}
void LiteScript::StateExecutor::I_VALUE_STRING(State& state) {}
void LiteScript::StateExecutor::I_VALUE_CALLBACK(State& state) {}
void LiteScript::StateExecutor::I_VALUE_ARRAY(State& state) {}
void LiteScript::StateExecutor::I_VALUE_OBJECT(State& state) {}
void LiteScript::StateExecutor::I_VALUE_CLASS(State& state) {}
void LiteScript::StateExecutor::I_VALUE_NAME(State& state) {}
void LiteScript::StateExecutor::I_VALUE_ARGS(State& state) {}

// PILES MANAGEMENT
void LiteScript::StateExecutor::I_PUSH_MAJOR(State& state) {}
void LiteScript::StateExecutor::I_PUSH_MINOR(State& state) {}
void LiteScript::StateExecutor::I_PUSH_ARGS(State& state) {}
void LiteScript::StateExecutor::I_POP_MAJOR(State& state) {}
void LiteScript::StateExecutor::I_POP_MINOR(State& state) {}

// OPERATIONS
// Assignation et unary operations
void LiteScript::StateExecutor::I_OP_ASSIGN(State& state) {}
void LiteScript::StateExecutor::I_OP_UNARY_PLUS(State& state) {}
void LiteScript::StateExecutor::I_OP_UNARY_MINUS(State& state) {}
void LiteScript::StateExecutor::I_OP_PRE_INCR(State& state) {}
void LiteScript::StateExecutor::I_OP_POST_INCR(State& state) {}
void LiteScript::StateExecutor::I_OP_PRE_DECR(State& state) {}
void LiteScript::StateExecutor::I_OP_POST_DECR(State& state) {}

// Arithmetic operations
void LiteScript::StateExecutor::I_OP_ADD(State& state) {}
void LiteScript::StateExecutor::I_OP_SUB(State& state) {}
void LiteScript::StateExecutor::I_OP_MUL(State& state) {}
void LiteScript::StateExecutor::I_OP_DIV(State& state) {}
void LiteScript::StateExecutor::I_OP_MOD(State& state) {}

// Comparison
void LiteScript::StateExecutor::I_OP_EQU(State& state) {}
void LiteScript::StateExecutor::I_OP_DIF(State& state) {}
void LiteScript::StateExecutor::I_OP_GREAT(State& state) {}
void LiteScript::StateExecutor::I_OP_LESS(State& state) {}
void LiteScript::StateExecutor::I_OP_GREAT_EQU(State& state) {}
void LiteScript::StateExecutor::I_OP_LESS_EQU(State& state) {}

// Logical operation
void LiteScript::StateExecutor::I_OP_LOG_NOT(State& state) {}
void LiteScript::StateExecutor::I_OP_LOG_AND(State& state) {}
void LiteScript::StateExecutor::I_OP_LOG_OR(State& state) {}

// Binary operations
void LiteScript::StateExecutor::I_OP_BIT_NOT(State& state) {}
void LiteScript::StateExecutor::I_OP_BIT_AND(State& state) {}
void LiteScript::StateExecutor::I_OP_BIT_OR(State& state) {}
void LiteScript::StateExecutor::I_OP_BIT_XOR(State& state) {}
void LiteScript::StateExecutor::I_OP_LSHIFT(State& state) {}
void LiteScript::StateExecutor::I_OP_RSHIFT(State& state) {}

// Arithmetic and assignation
void LiteScript::StateExecutor::I_OP_ADD_ASSIGN(State& state) {}
void LiteScript::StateExecutor::I_OP_SUB_ASSIGN(State& state) {}
void LiteScript::StateExecutor::I_OP_MUL_ASSIGN(State& state) {}
void LiteScript::StateExecutor::I_OP_DIV_ASSIGN(State& state) {}

// Special operations
void LiteScript::StateExecutor::I_OP_ARRAY(State& state) {}
void LiteScript::StateExecutor::I_OP_MEMBER(State& state) {}
void LiteScript::StateExecutor::I_OP_CALL(State& state) {}

// CONTROL INSTRUCTIONS
void LiteScript::StateExecutor::I_JUMP_TO(State& state) {}
void LiteScript::StateExecutor::I_JUMP_IF(State& state) {}
void LiteScript::StateExecutor::I_JUMP_ELSE(State& state) {}

// COMPLEX VALUES COMPLETION
// Array
void LiteScript::StateExecutor::I_ARRAY_PUSH(State& state) {}

// Object
void LiteScript::StateExecutor::I_OBJECT_PUSH_NUMERIC(State& state) {}
void LiteScript::StateExecutor::I_OBJECT_PUSH_LITERAL(State& state) {}

// Class
void LiteScript::StateExecutor::I_CLASS_PUSH_STATIC(State& state) {}
void LiteScript::StateExecutor::I_CLASS_PUSH_USTATIC(State& state) {}
void LiteScript::StateExecutor::I_CLASS_INHERIT(State& state) {}

// NAMESPACES
void LiteScript::StateExecutor::I_NAMESPACE_USE(State& state) {}
void LiteScript::StateExecutor::I_NAMESPACE_RESET(State& state) {}