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

#ifndef LITESCRIPT_SCRIPT_ASSEMBLY_HPP
#define LITESCRIPT_SCRIPT_ASSEMBLY_HPP

namespace LiteScript {

    // This class interpret and execute an assembly script code
    class Assembly {

    public:

        /////////////////////////////
        ////// STATIC ELEMENTS //////
        /////////////////////////////

        // List of errors
        enum ErrorType {
            ASSM_ERROR_NO,
            ASSM_ERROR_UNKNOW_CMD,
            ASSM_ERROR_EXPECTED_BOOLEAN,
            ASSM_ERROR_EXPECTED_INTEGER,
            ASSM_ERROR_EXPECTED_NUMBER,
            ASSM_ERROR_EXPECTED_STRING,
            ASSM_ERROR_NUMBER
        };

        // List of message errors
        static std::array<const char *, ErrorType::ASSM_ERROR_NUMBER> ErrorMsg;

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
         * Basic constructor (create internal memory)
         */
        Assembly();

        /**
         * Constructor by referencing memory
         *
         * @param mem The main memory
         */
        Assembly(Memory& mem);

        /**
         * Destructor
         */
        ~Assembly();

        /////////////////////
        ////// METHODS //////
        /////////////////////

        /**
         * Execute an assembly script code
         *
         * @param code The assembly script code
         * @return The last variable returned
         */
        Variable Execute(const char * code);

        /**
         * Execute an assembly script code (push instructions in the current list)
         *
         * @param code The assembly script code
         * @return The last variable returned
         */
        Variable ExecuteAfter(const char * code);

        /**
         * Execute a single assembly script code line
         *
         * @param code The assembly script code line
         * @return The last variable returned
         */
        Variable ExecuteSingle(const char * code);

        /**
         * Get the formated string error
         */
        const char * GetError() const;

        ////////////////////////////
        ////// STATIC METHODS //////
        ////////////////////////////

        /**
         * Create a single instruction by a single line assembly script code
         *
         * @param code The assembly script code
         * @return The instruction if success and an invalid instruction otherwise
         */
        static Instruction GetInstructionSingle(const char * code);

        /**
         * Create a single instruction by a single line assembly script code
         *
         * @param code The assembly script code
         * @param err The error return
         * @return The instruction if success and an invalid instruction otherwise
         */
        static Instruction GetInstructionSingle(const char * code, Assembly::ErrorType& err);

        /**
         * Create a list of instructions by an assembly script code
         *
         * @param code The assembly script code
         * @return The instructions list
         */
        static std::vector<Instruction> GetInstructionList(const char * code);

        /**
         * Create a list of instructions by an assembly script code
         *
         * @param code The assembly script code
         * @param err The error return
         * @param line_err The line when error encounting
         * @return The instructions list
         */
        static std::vector<Instruction> GetInstructionList(const char * code, Assembly::ErrorType& err, unsigned int& line_err);

        /**
         * Get the formated string error
         *
         * @param type The type of error
         */
        static const char * GetError(Assembly::ErrorType type);

    };

}

#endif //LITESCRIPT_ASSEMBLY_HPP

#endif
