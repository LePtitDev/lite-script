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

#ifndef LITESCRIPT_API_TYPES_HPP
#define LITESCRIPT_API_TYPES_HPP

namespace LiteScript {

    // Class declarations
    class Variable;
    class Memory;
    class State;

    /**
     * Nullify a variable
     *
     * @param v The variable
     */
    void NullifyVariable(Variable& v);

    /**
     * Create an undefined variable
     *
     * @param memory The main memory
     */
    Variable CreateUndefined(Memory& memory);

    /**
     * Create a null variable
     *
     * @param memory The main memory
     */
    Variable CreateVariable(Memory& memory);

    /**
     * Create a boolean variable
     *
     * @param memory The main memory
     * @param value The value to assign
     */
    Variable CreateVariable(Memory& memory, bool value);

    /**
     * Create a number variable
     *
     * @param memory The main memory
     * @param value The value to assign
     */
    Variable CreateVariable(Memory& memory, int value);

    /**
     * Create a number variable
     *
     * @param memory The main memory
     * @param value The value to assign
     */
    Variable CreateVariable(Memory& memory, float value);

    /**
     * Create a string variable
     *
     * @param memory The main memory
     * @param value The value to assign
     */
    Variable CreateVariable(Memory& memory, const char * value);

    /**
     * Create a callback variable
     *
     * @param memory The main memory
     * @param value The value to assign
     */
    Variable CreateVariable(Memory& memory, Variable (*value)(State& s, std::vector<Variable>& args));

    /**
     * Create an array variable (numeric index)
     *
     * @param memory The main memory
     * @param count The values count
     * @param values The values array
     */
    Variable CreateVariable(Memory& memory, int count, const Variable * values);

    /**
     * Create an array variable (named index)
     *
     * @param memory The main memory
     * @param count The values count
     * @param keys The keys array
     * @param values The values array
     */
    Variable CreateVariable(Memory& memory, int count, const char * keys[], const Variable * values);

    /**
     * Create a class variable
     *
     * @param memory The main memory
     * @param value The value to assign
     */
    Variable CreateVariable(Memory& memory, Class& value);

    /**
     * Create a namespace
     *
     * @param memory The main memory
     * @param count The variables count
     * @param keys The names array
     * @param values The variables array
     */
    Variable CreateNamespace(Memory& memory, int count, const char * names[], const Variable * values);

}

#endif //LITESCRIPT_TYPES_HPP
