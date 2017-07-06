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

namespace LiteScript {

    // Namespace that contain functions syntax
    namespace Syntax {

        /**
         * Try to read a boolean
         *
         * @param text The text that contain the boolean
         * @param res The result
         * @return The number of readed characters (zero if failure)
         */
        unsigned int ReadBoolean(const char * text, bool& res);

        /**
         * Try to read an integer
         *
         * @param text The text that contain the integer
         * @param res The result
         * @return The number of readed characters (zero if failure)
         */
        unsigned int ReadInteger(const char * text, int& res);

        /**
         * Try to read an unsigned integer
         *
         * @param text The text that contain the integer
         * @param res The result
         * @return The number of readed characters (zero if failure)
         */
        unsigned int ReadUInteger(const char * text, unsigned int& res);

        /**
         * Try to read a number
         *
         * @param text The text that contain the number
         * @param res The result
         * @return The number of readed characters (zero if failure)
         */
        unsigned int ReadNumber(const char * text, float& res);

        /**
         * Try to read an unsigned number
         *
         * @param text The text that contain the number
         * @param res The result
         * @return The number of readed characters (zero if failure)
         */
        unsigned int ReadUNumber(const char * text, float& res);

    }

}

#endif //LITESCRIPT_SYNTAX_HPP
