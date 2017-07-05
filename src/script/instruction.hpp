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

#ifndef LITESCRIPT_SCRIPT_INSTRUCTION_HPP
#define LITESCRIPT_SCRIPT_INSTRUCTION_HPP

namespace LiteScript {

    // Instruction codes
    enum InstrCode {
        // INVALID
        INVALID,

        // DEFINITIONS
        DEFINE_VARIABLE,
        DEFINE_ARG,
        DEFINE_RETURN,

        // VALUE CREATIONS
        VALUE_UNDEFINED,
        VALUE_NULL,
        VALUE_BOOLEAN,
        VALUE_NUMBER,
        VALUE_STRING,
        VALUE_CALLBACK,
        VALUE_ARRAY,
        VALUE_CLASS,
        VALUE_ARGS,
        VALUE_THIS,

        // PILES MANAGEMENT
        PUSH_NSP,
        PUSH_ARGS,
        POP_NSP,
        POP_AGRS,

        // OPERATIONS
        // Assignation et unary operations
        OP_ASSIGN,
        OP_UNARY_PLUS,
        OP_UNARY_MINUS,
        OP_PRE_INCR,
        OP_POST_INCR,
        OP_PRE_DECR,
        OP_POST_DECR,

        // Arithmetic operations
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_DIV,
        OP_MOD,

        // Comparison
        OP_EQU,
        OP_DIF,
        OP_GREAT,
        OP_LESS,
        OP_GREAT_EQU,
        OP_LESS_EQU,

        // Logical operation
        OP_LOG_NOT,
        OP_LOG_AND,
        OP_LOG_OR,

        // Binary operations
        OP_BIT_NOT,
        OP_BIT_AND,
        OP_BIT_OR,
        OP_BIT_XOR,
        OP_LSHIFT,
        OP_RSHIFT,

        // Arithmetic and assignation
        OP_ADD_ASSIGN,
        OP_SUB_ASSIGN,
        OP_MUL_ASSIGN,
        OP_DIV_ASSIGN,

        // Special operations
        OP_ARRAY,
        OP_MEMBER,
        OP_CALL,

        // CONTROL INSTRUCTIONS
        JUMP_TO,
        JUMP_IF,
        JUMP_ELSE,

        // COMPLEX VALUES COMPLETION
        // Array
        ARRAY_PUSH_NUMERIC,
        ARRAY_PUSH_LITERAL,

        // Class
        CLASS_PUSH_STATIC,
        CLASS_PUSH_USTATIC,
        CLASS_PUSH_OPERATOR,
        CLASS_INHERIT,

        // NAMESPACES
        NAMESPACE_USE,

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

        /**
         * Save the instruction in the stream
         *
         * @param stream The stream
         */
        void Save(std::ostream& stream);

        ////////////////////////////
        ////// STATIC METHODS //////
        ////////////////////////////

        /**
         * Save an instruction in a stream
         *
         * @param stream The stream
         * @param instr The instruction
         */
        static void Save(std::ostream& stream, Instruction instr);

        /**
         * Load an instruction from a stream
         *
         * @param stream The stream
         * @return The instruction
         */
        static Instruction Load(std::istream& stream);

    };

}

#endif //LITESCRIPT_INSTRUCTION_HPP

#endif
