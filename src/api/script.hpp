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

#ifndef LITESCRIPT_API_SCRIPT_HPP
#define LITESCRIPT_API_SCRIPT_HPP

namespace LiteScript {

    // Class declarations
    class Variable;
    class Memory;
    class State;

    /**
     * Add a callback to the callback list
     *
     * @param value The callback to add
     * @return The index of the callback in the list
     */
    unsigned int AddCallback(Variable (*value)(State& s, std::vector<Variable>& args));

    /**
     * Create a callback variable contained in the callback list
     *
     * @param memory The main memory
     * @param id The index of the callback in the list
     */
    Variable CreateCallback(Memory& memory, unsigned int id);

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
