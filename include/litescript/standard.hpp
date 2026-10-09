#ifndef LITESCRIPT_STANDARD_HPP
#define LITESCRIPT_STANDARD_HPP

#include "state.hpp"

// The standard library: optional functions that the application can give to its scripts
// The functions are registered in the memory with names starting with "std.", the states that use
// them can be saved and loaded (the library must be installed in the memory before the loading).
namespace LiteScript::Standard {

    /**
     * Install every module
     */
    void Install(State& state);

    /**
     * The namespace "Math": abs, floor, ceil, round, trunc, sign, min, max, pow, sqrt, exp, log,
     * sin, cos, tan, asin, acos, atan, atan2, random, seed, and the constants PI and E
     */
    void InstallMath(State& state);

    /**
     * The global functions typeOf, toString, parseNumber, parseInt, isNaN and char
     */
    void InstallConversions(State& state);

    /**
     * The methods of the arrays: push, pop, insert, remove, clear, indexOf, contains, join,
     * slice, reverse, sort, keys, map, filter, forEach
     */
    void InstallArrays(State& state);

    /**
     * The methods of the strings: substring, indexOf, contains, startsWith, endsWith, split,
     * replace, trim, toUpperCase, toLowerCase, charCode
     */
    void InstallStrings(State& state);

}

#endif //LITESCRIPT_STANDARD_HPP
