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

#ifndef LITESCRIPT_LITESCRIPT_HPP

#include "../litescript.hpp"

#else

#ifndef LITESCRIPT_SCRIPT_INSTRUCTION_HPP
#define LITESCRIPT_SCRIPT_INSTRUCTION_HPP

namespace LiteScript {

    // Instruction codes
    enum InstrCode {
        // INVALID
        INSTR_INVALID,

        // DEFINITIONS
        INSTR_DEFINE_VARIABLE,
        INSTR_DEFINE_ARG,
        INSTR_DEFINE_RETURN,

        // VALUE CREATIONS
        INSTR_VALUE_POP,
        INSTR_VALUE_UNDEFINED,
        INSTR_VALUE_NULL,
        INSTR_VALUE_BOOLEAN,
        INSTR_VALUE_NUMBER,
        INSTR_VALUE_STRING,
        INSTR_VALUE_CALLBACK,
        INSTR_VALUE_ARRAY,
        INSTR_VALUE_CLASS,
        INSTR_VALUE_OBJECT,
        INSTR_VALUE_ARG,
        INSTR_VALUE_ARGS,
        INSTR_VALUE_THIS,
        INSTR_VALUE_VARIABLE,

        // PILES MANAGEMENT
        INSTR_PUSH_NSP,
        INSTR_PUSH_ARGS,
        INSTR_POP_NSP,
        INSTR_POP_ARGS,
        INSTR_RETURN,

        // OPERATIONS
        // Assignation et unary operations
        INSTR_OP_ASSIGN,
        INSTR_OP_UNARY_PLUS,
        INSTR_OP_UNARY_MINUS,
        INSTR_OP_PRE_INCR,
        INSTR_OP_POST_INCR,
        INSTR_OP_PRE_DECR,
        INSTR_OP_POST_DECR,

        // Arithmetic operations
        INSTR_OP_ADD,
        INSTR_OP_SUB,
        INSTR_OP_MUL,
        INSTR_OP_DIV,
        INSTR_OP_MOD,

        // Comparison
        INSTR_OP_EQU,
        INSTR_OP_DIF,
        INSTR_OP_GREAT,
        INSTR_OP_LESS,
        INSTR_OP_GREAT_EQU,
        INSTR_OP_LESS_EQU,

        // Logical operation
        INSTR_OP_LOG_NOT,
        INSTR_OP_LOG_AND,
        INSTR_OP_LOG_OR,

        // Binary operations
        INSTR_OP_BIT_NOT,
        INSTR_OP_BIT_AND,
        INSTR_OP_BIT_OR,
        INSTR_OP_BIT_XOR,
        INSTR_OP_LSHIFT,
        INSTR_OP_RSHIFT,

        // Arithmetic and assignation
        INSTR_OP_ADD_ASSIGN,
        INSTR_OP_SUB_ASSIGN,
        INSTR_OP_MUL_ASSIGN,
        INSTR_OP_DIV_ASSIGN,

        // Special operations
        INSTR_OP_ARRAY,
        INSTR_OP_MEMBER,
        INSTR_OP_CALL,

        // CONTROL INSTRUCTIONS
        INSTR_JUMP_TO,
        INSTR_JUMP_IF,
        INSTR_JUMP_ELSE,

        // COMPLEX VALUES COMPLETION
        // Array
        INSTR_ARRAY_PUSH_NUMERIC,
        INSTR_ARRAY_PUSH_LITERAL,

        // Class
        INSTR_CLASS_PUSH_STATIC,
        INSTR_CLASS_PUSH_USTATIC,
        INSTR_CLASS_PUSH_OPERATOR,
        INSTR_CLASS_INHERIT,
        INSTR_CLASS_CONSTRUCTOR,

        // NAMESPACES
        INSTR_NAMESPACE_USE,

        // INSTRUCTION_NUMBER
        INSTR_NUMBER

    };

    // Reduced instruction
    struct Instruction {

        // Complement value type
        enum CompType {
            COMP_TYPE_NONE,
            COMP_TYPE_BOOLEAN,
            COMP_TYPE_INTEGER,
            COMP_TYPE_FLOAT,
            COMP_TYPE_STRING
        };

        ////////////////////////
        ////// ATTRIBUTES //////
        ////////////////////////
            
        // The instruction code
        unsigned char code;

        // Complement value type
        unsigned char comp_type;

        // Complement value
        union {
            bool    v_boolean;
            int     v_integer;
            float   v_float;
            char *  v_string;
        } comp_value;

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

    private:

        // Private constructor
        Instruction();

    public:

        // Basic constructor
        Instruction(InstrCode code);

        // Constructor with boolean value
        Instruction(InstrCode code, bool val);

        // Constructor with integer value
        Instruction(InstrCode code, int val);

        // Constructor with floatting value
        Instruction(InstrCode code, float val);

        // Constructor with string value
        Instruction(InstrCode code, const char * val);

        // Copy constructor
        Instruction(const Instruction& i);

        // Destructor
        ~Instruction();

        /////////////////////////////
        ////// UNSTATIC METHOD //////
        /////////////////////////////

        // The assign operator
        Instruction& operator=(const Instruction& in);

        ////////////////////////////
        ////// STATIC METHODS //////
        ////////////////////////////

        /**
         * Save an instruction in a stream
         *
         * @param stream The stream
         * @param instr The instruction
         */
        static void Save(std::ostream& stream, const std::vector<Instruction>& instr);

        /**
         * Load an instruction from a stream
         *
         * @param stream The stream
         * @return The instruction
         */
        static std::vector<Instruction> Load(std::istream& stream);

    };

}

#endif //LITESCRIPT_INSTRUCTION_HPP

#endif
