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

#ifndef LITESCRIPT_SCRIPT_HPP
#define LITESCRIPT_SCRIPT_HPP

namespace LiteScript {

    // Class declarations
    class Variable;
    class Memory;
    class State;

    /**
     * Declare a variable in the script state
     *
     * @param state The script state
     * @param name The name of the variable
     * @param v The variable
     */
    void DeclareVariable(State& state, const char * name, const Variable& v);

}

#endif //LITESCRIPT_SCRIPT_HPP
