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

#ifndef LITESCRIPT_SCRIPT_SCRIPT_HPP
#define LITESCRIPT_SCRIPT_SCRIPT_HPP

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

            SCRPT_ERROR_VALUE_SIMPLEARRAY_END,
            SCRPT_ERROR_VALUE_NAMEDARRAY_COLON,
            SCRPT_ERROR_VALUE_NAMEDARRAY_EXPRESSION,
            SCRPT_ERROR_VALUE_NAMEDARRAY_END,

            SCRPT_ERROR_VALUE_CALLBACK_ARGUMENTS,
            SCRPT_ERROR_VALUE_CALLBACK_INSTRUCTIONS,

            SCRPT_ERROR_CALLBACK_ARGUMENT_NAME,
            SCRPT_ERROR_CALLBACK_ARGUMENT_END,

            SCRPT_ERROR_NUMBER
        };

        // List of message errors
        static std::array<const char *, ErrorType::SCRPT_ERROR_NUMBER> ErrorMsg;

    private:

        ////////////////////////
        ////// ATTRIBUTES //////
        ////////////////////////

        // Data for memory if created
        char _mem_d[sizeof(Memory)];

        // Indicate if memory is internal
        bool _mem_a;

        // Data for state
        char _sta_d[sizeof(State)];

    public:

        // The last error code
        ErrorType error;

        // The line number of the last error
        unsigned int line_error;

        ///////////////////////
        ////// ACCESSORS //////
        ///////////////////////

        // The main memory
        Memory& memory;

        // The script state
        State& state;

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor of a script
         */
        Script();

        /**
         * Constructor by referencing memory
         *
         * @param mem The main memory
         */
        Script(Memory& mem);

        /**
         * Destructor
         */
        ~Script();

        /////////////////////
        ////// METHODS //////
        /////////////////////

        /**
         * Execute an oriented-object script code
         *
         * @param code The oriented-object script code
         * @return The last variable returned
         */
        Variable Execute(const char * code);

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
         * @return The instructions list
         */
        static std::vector<Instruction> GetInstructionList(const char * code, Script::ErrorType& err, unsigned int& line_err);

        /**
         * Get the formated string error
         *
         * @param type The type of error
         */
        static const char * GetError(Script::ErrorType type);

    };

}

#endif //LITESCRIPT_SCRIPT_HPP

#endif