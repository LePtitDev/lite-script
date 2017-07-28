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

#ifndef LITESCRIPT_SCRIPT_SYNTAX_HPP
#define LITESCRIPT_SCRIPT_SYNTAX_HPP

#include "../litescript.hpp"

namespace LiteScript {

    // Namespace that contain functions syntax
    namespace Syntax {

        /**
         * Try to read an undefined value
         *
         * @param text The text that contain the undefined value
         * @return The number of read characters (zero if failure)
         */
        unsigned int ReadUndefined(const char * text);

        /**
         * Try to read a null value
         *
         * @param text The text that contain the null value
         * @return The number of read characters (zero if failure)
         */
        unsigned int ReadNull(const char * text);

        /**
         * Try to read a boolean
         *
         * @param text The text that contain the boolean
         * @param res The result
         * @return The number of read characters (zero if failure)
         */
        unsigned int ReadBoolean(const char * text, bool& res);

        /**
         * Try to read an integer
         *
         * @param text The text that contain the integer
         * @param res The result
         * @return The number of read characters (zero if failure)
         */
        unsigned int ReadInteger(const char * text, int& res);

        /**
         * Try to read an unsigned integer
         *
         * @param text The text that contain the integer
         * @param res The result
         * @return The number of read characters (zero if failure)
         */
        unsigned int ReadUInteger(const char * text, unsigned int& res);

        /**
         * Try to read a number
         *
         * @param text The text that contain the number
         * @param res The result
         * @return The number of read characters (zero if failure)
         */
        unsigned int ReadNumber(const char * text, float& res);

        /**
         * Try to read an unsigned number
         *
         * @param text The text that contain the number
         * @param res The result
         * @return The number of read characters (zero if failure)
         */
        unsigned int ReadUNumber(const char * text, float& res);

        /**
         * Try to read a name
         *
         * @param text The text that contain the name
         * @param name The result
         * @return The number of read characters (zero if failure)
         */
        unsigned int ReadName(const char * text, std::string& name);

        /**
         * Try to read a string
         *
         * @param text The text that contain the string
         * @param string The result
         * @return The number of characters read if success and a negative number if an error occurred
         */
        int ReadString(const char * text, std::string& string);

        /**
         * Try to read a comment
         *
         * @param text The text that contain the comment
         * @return The number of read characters (zero if failure)
         */
        unsigned int ReadComment(const char * text);

        /**
         * Try to read a white space (comments included)
         *
         * @param text The text that contain the white space
         * @return The number of read characters (zero if failure)
         */
        unsigned int ReadWhitespace(const char * text);

        /**
         * Try to read an array
         *
         * @param text The text that contain the array
         * @param instrl The instructions list that result
         * @param errorType The error type
         * @return The number of characters read if success and a negative number if an error occurred
         * (if error the return value is the opposite of the position where occurred)
         */
        int ReadArray(const char * text, std::vector<Instruction>& instrl, Script::ErrorType& errorType);

        /**
         * Try to read an anonymous callback
         *
         * @param text The text that contain the anonymous callback
         * @param instrl The instructions list that result
         * @param errorType The error type
         * @return The number of characters read if success and a negative number if an error occurred
         * (if error the return value is the opposite of the position where occurred)
         */
        int ReadCallbackValue(const char * text, std::vector<Instruction>& instrl, Script::ErrorType& errorType);

        /**
         * Try to read callback arguments
         *
         * @param text The text that contain the callback arguments
         * @param instrl The instructions list that result
         * @param errorType The error type
         * @return The number of characters read if success and a negative number if an error occurred
         * (if error the return value is the opposite of the position where occurred)
         */
        int ReadCallbackArguments(const char * text, std::vector<Instruction>& instrl, Script::ErrorType& errorType);

        /**
         * Try to read an anonymous class
         *
         * @param text The text that contain the anonymous class
         * @param instrl The instructions list that result
         * @param errorType The error type
         * @return The number of characters read if success and a negative number if an error occurred
         * (if error the return value is the opposite of the position where occurred)
         */
        int ReadClassValue(const char * text, std::vector<Instruction>& instrl, Script::ErrorType& errorType);

        /**
         * Try to read class inherits
         *
         * @param text The text that contain the class inherits
         * @param instrl The instructions list that result
         * @param errorType The error type
         * @return The number of characters read if success and a negative number if an error occurred
         * (if error the return value is the opposite of the position where occurred)
         */
        int ReadClassInherits(const char * text, std::vector<Instruction>& instrl, Script::ErrorType& errorType);

        /**
         * Try to read a class member
         *
         * @param text The text that contain the class member
         * @param instrl The instructions list that result
         * @param errorType The error type
         * @return The number of characters read if success and a negative number if an error occurred
         * (if error the return value is the opposite of the position where occurred)
         */
        int ReadClassMember(const char * text, std::vector<Instruction>& instrl, Script::ErrorType& errorType);

        /**
         * Try to read a class operator
         *
         * @param text The text that contain the class operator
         * @param instrl The instructions list that result
         * @param errorType The error type
         * @return The number of characters read if success and a negative number if an error occurred
         * (if error the return value is the opposite of the position where occurred)
         */
        int ReadClassOperator(const char * text, std::vector<Instruction>& instrl, Script::ErrorType& errorType);

        /**
         * Try to read a value
         *
         * @param text The text that contain the value
         * @param instrl The instructions list that result
         * @param errorType The error type
         * @return The number of characters read if success and a negative number if an error occurred
         * (if error the return value is the opposite of the position where occurred)
         */
        int ReadValue(const char * text, std::vector<Instruction>& instrl, Script::ErrorType& errorType);

        /**
         * Try to read an operand
         *
         * @param text The text that contain the operand
         * @param instrl The instructions list that result
         * @param errorType The error type
         * @return The number of characters read if success and a negative number if an error occurred
         * (if error the return value is the opposite of the position where occurred)
         */
        int ReadOperand(const char * text, std::vector<Instruction>& instrl, Script::ErrorType& errorType);

        /**
         * Try to read an expression
         *
         * @param text The text that contain the expression
         * @param instrl The instructions list that result
         * @param errorType The error type
         * @return The number of characters read if success and a negative number if an error occurred
         * (if error the return value is the opposite of the position where occurred)
         */
        int ReadExpression(const char * text, std::vector<Instruction>& instrl, Script::ErrorType& errorType);

        /**
         * Try to read a variable definition
         *
         * @param text The text that contain the variable definition
         * @param instrl The instructions list that result
         * @param errorType The error type
         * @return The number of characters read if success and a negative number if an error occurred
         * (if error the return value is the opposite of the position where occurred)
         */
        int ReadVariableDefinition(const char * text, std::vector<Instruction>& instrl, Script::ErrorType& errorType);

        /**
         * Try to read an if control instruction
         *
         * @param text The text that contain the if control instruction
         * @param instrl The instructions list that result
         * @param errorType The error type
         * @return The number of characters read if success and a negative number if an error occurred
         * (if error the return value is the opposite of the position where occurred)
         */
        int ReadControlIf(const char * text, std::vector<Instruction>& instrl, Script::ErrorType& errorType);

        /**
         * Try to read a callback
         *
         * @param text The text that contain the callback
         * @param instrl The instructions list that result
         * @param errorType The error type
         * @return The number of characters read if success and a negative number if an error occurred
         * (if error the return value is the opposite of the position where occurred)
         */
        int ReadCallback(const char * text, std::vector<Instruction>& instrl, Script::ErrorType& errorType);

        /**
         * Try to read a class
         *
         * @param text The text that contain the class
         * @param instrl The instructions list that result
         * @param errorType The error type
         * @return The number of characters read if success and a negative number if an error occurred
         * (if error the return value is the opposite of the position where occurred)
         */
        int ReadClass(const char * text, std::vector<Instruction>& instrl, Script::ErrorType& errorType);

        /**
         * Try to read an instruction
         *
         * @param text The text that contain the instruction
         * @param instrl The instructions list that result
         * @param errorType The error type
         * @return The number of characters read if success and a negative number if an error occurred
         * (if error the return value is the opposite of the position where occurred)
         */
        int ReadInstruction(const char * text, std::vector<Instruction>& instrl, Script::ErrorType& errorType);

        /**
         * Try to read an instruction block
         *
         * @param text The text that contain the instruction block
         * @param instrl The instructions list that result
         * @param errorType The error type
         * @return The number of characters read if success and a negative number if an error occurred
         * (if error the return value is the opposite of the position where occurred)
         */
        int ReadInstructionBlock(const char * text, std::vector<Instruction>& instrl, Script::ErrorType& errorType);

        /**
         * Try to read a script
         *
         * @param text The text that contain the script
         * @param instrl The instructions list that result
         * @param errorType The error type
         * @return The number of characters read if success and a negative number if an error occurred
         * (if error the return value is the opposite of the position where occurred)
         */
        int ReadScript(const char * text, std::vector<Instruction>& instrl, Script::ErrorType& errorType);

    }

}

#endif //LITESCRIPT_SYNTAX_HPP
