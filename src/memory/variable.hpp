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

#ifndef LITESCRIPT_MEMORY_VARIABLE_HPP
#define LITESCRIPT_MEMORY_VARIABLE_HPP

#include <string>
#include <vector>

namespace LiteScript {

    // Class declarations
    class Object;
    class Type;
    class Memory;
    class State;

    // Variable class definition
    class Variable {

        // The object
        Object& obj;

        // The counter of variables that refer to this object
        unsigned int& nb_ref;

    public:

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor of a variable
         *
         * @param memory The memory that contain the object
         * @param obj The object
         * @param nb_ref The counter of variables that refer to this object
         */
        Variable(Object& obj, unsigned int& nb_ref);

        /**
         * Copy constructor
         *
         * @param v The variable to copy
         */
        Variable(const Variable& v);

        /**
         * Destructor of a variable
         */
        ~Variable();

        //////////////////////////////
        ////// ACCESS OPERATORS //////
        //////////////////////////////

        // Pointer operator to an object
        Object* operator->();

        // Pointer operator to an object (constant)
        const Object* operator->() const;

        // Pointer operator to an object
        Object& operator*();

        // Pointer operator to an object (constant)
        const Object& operator*() const;

        ///////////////////////////////////
        ////// OPERATORS OVERLOADING //////
        ///////////////////////////////////

        /**
         * Try to convert an object by an other typed object
         *
         * @param type The type for convertion
         * @return An object converted or a null object
         */
        Variable Convert(const Type& type) const;

        /**
         * Assign the object by an other object
         *
         * @param object The other object
         * @return This object
         */
        Variable operator=(const Variable& object);

        ////// ARITHMETIC OPERATIONS //////

        /**
         * Apply unary plus operation
         *
         * @return The result of operation
         */
        Variable operator+() const;

        /**
         * Apply unary minus operation
         *
         * @return The result of operation
         */
        Variable operator-() const;

        /**
         * Apply pre-increment operation
         *
         * @return This object
         */
        Variable operator++();

        /**
         * Apply post-increment operation
         *
         * @return The result of operation
         */
        Variable operator++(int);

        /**
         * Apply pre-decrement operation
         *
         * @return This object
         */
        Variable operator--();

        /**
         * Apply post-decrement operation
         *
         * @return The result of operation
         */
        Variable operator--(int);

        /**
         * Apply addition operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator+(const Variable& object) const;

        /**
         * Apply substraction operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator-(const Variable& object) const;

        /**
         * Apply multiplication operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator*(const Variable& object) const;

        /**
         * Apply division operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator/(const Variable& object) const;

        /**
         * Apply modulo operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator%(const Variable& object) const;

        ////// COMPARISON OPERATIONS //////

        /**
         * Apply equality comparison
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator==(const Variable& object) const;

        /**
         * Apply inequality comparison
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator!=(const Variable& object) const;

        /**
         * Apply superiority comparison
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator>(const Variable& object) const;

        /**
         * Apply inferiority comparison
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator<(const Variable& object) const;

        /**
         * Apply superiority or equality comparison
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator>=(const Variable& object) const;

        /**
         * Apply inferiority or equality comparison
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator<=(const Variable& object) const;

        ////// LOGICAL OPERATIONS //////

        /**
         * Apply logical not operation
         *
         * @return The result of operation
         */
        Variable operator!() const;

        /**
         * Apply logical and operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator&&(const Variable& object) const;

        /**
         * Apply logical or operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator||(const Variable& object) const;

        ////// BINARY OPERATIONS //////

        /**
         * Apply binary not operation
         *
         * @return The result of operation
         */
        Variable operator~() const;

        /**
         * Apply binary and operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator&(const Variable& object) const;

        /**
         * Apply binary or operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator|(const Variable& object) const;

        /**
         * Apply binary xor operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator^(const Variable& object) const;

        /**
         * Apply left shift operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator<<(const Variable& object) const;

        /**
         * Apply right shift operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator>>(const Variable& object) const;

        ////// DIRECT MODIFICATION OPERATIONS //////

        /**
         * Apply addition and assignation operation
         *
         * @param object The second operand object
         * @return This object
         */
        Variable operator+=(const Variable& object);

        /**
         * Apply substraction and assignation operation
         *
         * @param object The second operand object
         * @return This object
         */
        Variable operator-=(const Variable& object);

        /**
         * Apply multipilcation and assignation operation
         *
         * @param object The second operand object
         * @return This object
         */
        Variable operator*=(const Variable& object);

        /**
         * Apply division and assignation operation
         *
         * @param object The second operand object
         * @return This object
         */
        Variable operator/=(const Variable& object);


        /**
         * Apply array access
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Variable operator[](const Variable& object);

        /**
         * Apply member access
         *
         * @param name The name of member
         * @return The result of operation
         */
        Variable GetMember(const char * name);

        ////// CALLING OPERATION //////

        /**
         * Apply calling operation
         *
         * @param state The script state
         * @param args The argument list
         */
        Variable operator()(State& state, std::vector<Variable>& args);

        ////// STRING CONVERSION //////

        /**
         * Return a string that describe the content of the object
         */
        operator std::string() const;

        ///////////////////////////////
        ////// GARBAGE COLLECTOR //////
        ///////////////////////////////

        /**
         * Refer itself and all variables in the object for the garbage collector
         *
         * @param caller The calling function for referring
         */
        void GarbageCollector(void (Memory::*caller)(unsigned int));

    };

}

/**
 * Send the content of the object in the stream
 *
 * @param stream The stream
 * @param object The object
 * @return The stream
 */
std::ostream& operator<<(std::ostream& stream, const LiteScript::Variable& object);

#endif //LITESCRIPT_MEMORY_VARIABLE_HPP