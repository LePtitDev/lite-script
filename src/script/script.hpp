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

#ifndef LITESCRIPT_SCRIPT_SCRIPT_HPP
#define LITESCRIPT_SCRIPT_SCRIPT_HPP

#include "state.hpp"

namespace LiteScript {

    // This class interpret and execute an oriented-object script code
    class Script {

    public:

        /////////////////////////////
        ////// STATIC ELEMENTS //////
        /////////////////////////////

        // List of errors
        enum ErrorType {
            SCRPT_ERROR_NO,

            SCRPT_ERROR_SIMPLEARRAY_END,
            SCRPT_ERROR_NAMEDARRAY_COLON,
            SCRPT_ERROR_NAMEDARRAY_EXPRESSION,
            SCRPT_ERROR_NAMEDARRAY_END,

            SCRPT_ERROR_CALLBACK_ARGUMENTS,
            SCRPT_ERROR_CALLBACK_ARGUMENT_NAME,
            SCRPT_ERROR_CALLBACK_ARGUMENT_END,
            SCRPT_ERROR_CALLBACK_INSTRUCTIONS,

            SCRPT_ERROR_CLASS_INHERIT,
            SCRPT_ERROR_CLASS_BEGIN,
            SCRPT_ERROR_CLASS_INVALID,
            SCRPT_ERROR_CLASS_OPERATOR_NAME,
            SCRPT_ERROR_CLASS_OPERATOR_INVALID,
            SCRPT_ERROR_CLASS_END,

            SCRPT_ERROR_FOR_INITIALISATION,
            SCRPT_ERROR_FOR_CONDITION,
            SCRPT_ERROR_SWITCH_KEYWORDS,
            SCRPT_ERROR_SWITCH_END,

            SCRPT_ERROR_NAME,
            SCRPT_ERROR_VALUE,
            SCRPT_ERROR_OPERATOR,
            SCRPT_ERROR_EXPRESSION,
            SCRPT_ERROR_PARENTHESIS_OPEN,
            SCRPT_ERROR_PARENTHESIS_CLOSE,
            SCRPT_ERROR_BRACKET_CLOSE,
            SCRPT_ERROR_BRACE_OPEN,
            SCRPT_ERROR_BRACE_CLOSE,
            SCRPT_ERROR_COLON,
            SCRPT_ERROR_SEMICOLON,
            SCRPT_ERROR_INSTRUCTION,
            SCRPT_ERROR_RETURN,
            SCRPT_ERROR_WHILE,
            SCRPT_ERROR_NAMESPACE,

            SCRPT_ERROR_UNKNOW,

            SCRPT_ERROR_NUMBER
        };

        // List of message errors
        static std::array<const char *, ErrorType::SCRPT_ERROR_NUMBER> ErrorMsg;

        ////////////////////////
        ////// ATTRIBUTES //////
        ////////////////////////

        // The last error code
        ErrorType error;

        // The line number of the last error
        unsigned int line_error;

        // The column number of the last error
        unsigned int col_error;

        // The main memory
        Memory& memory;

        // The script state
        State& state;

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor
         *
         * @param state The script state
         */
        Script(State& state);

        /**
         * Copy constructor
         *
         * @param sc The script to copy
         */
        Script(const Script& sc);

        /////////////////////
        ////// METHODS //////
        /////////////////////

        /**
         * Execute an oriented-object script code
         *
         * @param code The oriented-object script code
         * @return The last variable returned
         */
        void Execute(const char * code);

        /**
         * Get the formated string error
         */
        const char * GetError() const;

        ////////////////////////////
        ////// STATIC METHODS //////
        ////////////////////////////

        /**
         * Create a list of instructions by an oriented-object script code
         *
         * @param code The oriented-object script code
         * @return The instructions list
         */
        static std::vector<Instruction> GetInstructionList(const char * code);

        /**
         * Create a list of instructions by an oriented-object script code
         *
         * @param code The oriented-object script code
         * @param err The error return
         * @param line_err The line when error encounting
         * @param col_error The error column when error encounting
         * @return The instructions list
         */
        static std::vector<Instruction> GetInstructionList(const char * code, Script::ErrorType& err, unsigned int& line_err, unsigned int& col_error);

        /**
         * Get the formated string error
         *
         * @param type The type of error
         */
        static const char * GetError(Script::ErrorType type);

    };

}

#endif //LITESCRIPT_SCRIPT_HPP