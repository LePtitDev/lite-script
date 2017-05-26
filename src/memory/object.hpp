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

#ifndef LITESCRIPT_MEMORY_VARIABLE_HPP
#define LITESCRIPT_MEMORY_VARIABLE_HPP

namespace LiteScript {

    // Allocator of each objects
    extern std::allocator<char> ObjectAllocator;

    // Basic class of an object
    class Object {

    public:

        ///////////////////////////////
        ////// STATIC ATTRIBUTES //////
        ///////////////////////////////

        // Object of undefined type
        static Object UNDEFINED;

    private:

        ///////////////////////////////
        ////// OBJECT ATTRIBUTES //////
        ///////////////////////////////

        // Type of the object
        Type * type;

        // Size of internal datas in bytes
        unsigned int size;

        // Pointer to internal data (nullptr if size equal to 0)
        void * data;

    public:

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor of an object
         * The type of this object is "null"
         */
        Object();

        /**
         * Constructor of an object by type and datas size
         *
         * @param type The type of the object
         * @param size The size of internal datas
         */
        Object(Type& type, unsigned int size);

        /**
         * Constructor of an object by copy
         *
         * @param object The object to be copied
         */
        Object(const Object& object);

        /**
         * Destructor of an object
         * If size of datas is more than zero, unallocate datas
         */
        ~Object();

        /////////////////////
        ////// METHODS //////
        /////////////////////

        /**
         * Clear the content of the object
         */
        void Clear();

        /**
         * Get the type of the object
         *
         * @return The type of the object
         */
        Type& GetType() const;

        /**
         * Get the internal datas by cast
         * Example:
         *   // Create an object of type "boolean"
         *   Object object = Type::BOOLEAN.CreateObject();
         *   // Display the content of the object (see the datas type casts by each object type)
         *   cout << object.GetData<bool>() << endl;
         *
         * @return The casted datas
         */
        template<typename T>
        T& GetData() { return *(T*)this->data; }

        /**
         * Get the internal datas by cast (constant method)
         * Example:
         *   // Create a constant object of type "boolean"
         *   const Object object = Type::BOOLEAN.CreateObject();
         *   // Display the content of the object (see the datas type casts by each object type)
         *   cout << object.GetData<bool>() << endl;
         *
         * @return The casted datas (constant)
         */
        template<typename T>
        const T& GetData() const { return *(T*)this->data; }


        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // !!!!!! If the operation can't be applied on the object, !!!!!!
        // !!!!!! the object returned is typed "null"              !!!!!!
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        // !!!!!!!!!!!!!!!!!!!!!!!!! FROM HERE !!!!!!!!!!!!!!!!!!!!!!!!!!


        /**
         * Try to convert an object by an other typed object
         *
         * @param type The type for convertion
         * @return An object converted or a null object
         */
        Object Convert(const Type& type) const;

        /**
         * Reassign datas and type
         * Destroy and reallocate datas and type
         *
         * @param type The new type of the object
         * @param size The size of new datas of the object
         */
        Object& Reassign(Type& type, unsigned int size);

        ///////////////////////////////////
        ////// OPERATORS OVERLOADING //////
        ///////////////////////////////////

        /**
         * Assign the object by an other object
         *
         * @param object The other object
         * @return This object
         */
        Object& operator=(const Object& object);

        ////// ARITHMETIC OPERATIONS //////

        /**
         * Apply unary plus operation
         *
         * @return The result of operation
         */
        Object operator+() const;

        /**
         * Apply unary minus operation
         *
         * @return The result of operation
         */
        Object operator-() const;

        /**
         * Apply pre-increment operation
         *
         * @return This object
         */
        Object& operator++();

        /**
         * Apply post-increment operation
         *
         * @return The result of operation
         */
        Object operator++(int);

        /**
         * Apply pre-decrement operation
         *
         * @return This object
         */
        Object& operator--();

        /**
         * Apply post-decrement operation
         *
         * @return The result of operation
         */
        Object operator--(int);


        /**
         * Apply addition operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object operator+(const Object& object) const;

        /**
         * Apply substraction operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object operator-(const Object& object) const;

        /**
         * Apply multiplication operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object operator*(const Object& object) const;

        /**
         * Apply division operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object operator/(const Object& object) const;

        /**
         * Apply modulo operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object operator%(const Object& object) const;

        ////// COMPARISON OPERATIONS //////

        /**
         * Apply equality comparison
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object operator==(const Object& object) const;

        /**
         * Apply inequality comparison
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object operator!=(const Object& object) const;

        /**
         * Apply superiority comparison
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object operator>(const Object& object) const;

        /**
         * Apply inferiority comparison
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object operator<(const Object& object) const;

        /**
         * Apply superiority or equality comparison
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object operator>=(const Object& object) const;

        /**
         * Apply inferiority or equality comparison
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object operator<=(const Object& object) const;

        ////// LOGICAL OPERATIONS //////

        /**
         * Apply logical not operation
         *
         * @return The result of operation
         */
        Object operator!() const;

        /**
         * Apply logical and operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object operator&&(const Object& object) const;

        /**
         * Apply logical or operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object operator||(const Object& object) const;

        ////// BINARY OPERATIONS //////

        /**
         * Apply binary not operation
         *
         * @return The result of operation
         */
        Object operator~() const;

        /**
         * Apply binary and operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object operator&(const Object& object) const;

        /**
         * Apply binary or operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object operator|(const Object& object) const;

        /**
         * Apply binary xor operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object operator^(const Object& object) const;

        /**
         * Apply left shift operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object operator<<(const Object& object) const;

        /**
         * Apply right shift operation
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object operator>>(const Object& object) const;

        ////// DIRECT MODIFICATION OPERATIONS //////

        /**
         * Apply addition and assignation operation
         *
         * @param object The second operand object
         * @return This object
         */
        Object& operator+=(const Object& object);

        /**
         * Apply substraction and assignation operation
         *
         * @param object The second operand object
         * @return This object
         */
        Object& operator-=(const Object& object);

        /**
         * Apply multipilcation and assignation operation
         *
         * @param object The second operand object
         * @return This object
         */
        Object& operator*=(const Object& object);

        /**
         * Apply division and assignation operation
         *
         * @param object The second operand object
         * @return This object
         */
        Object& operator/=(const Object& object);


        // !!!!!!!!!!!!!!!!!!!!!!!!!! TO HERE !!!!!!!!!!!!!!!!!!!!!!!!!!!


        ////// ACCESS OPERATIONS //////


        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // !!!!!! If the operation can't be applied on the object, !!!!!!
        // !!!!!! the object returned is typed "undefined"         !!!!!!
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        // !!!!!!!!!!!!!!!!!!!!!!!!! FROM HERE !!!!!!!!!!!!!!!!!!!!!!!!!!


        /**
         * Apply array access
         *
         * @param object The second operand object
         * @return The result of operation
         */
        Object& operator[](const Object& object);

        /**
         * Apply member access
         *
         * @param name The name of member
         * @return The result of operation
         */
        Object& GetMember(const char * name);


        // !!!!!!!!!!!!!!!!!!!!!!!!!! TO HERE !!!!!!!!!!!!!!!!!!!!!!!!!!!


        ////// CALLING OPERATION //////


        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // !!!!!! If the operation can't be applied on the object, !!!!!!
        // !!!!!! the object returned is typed "null"              !!!!!!
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        // !!!!!!!!!!!!!!!!!!!!!!!!! FROM HERE !!!!!!!!!!!!!!!!!!!!!!!!!!


        /**
         * Apply calling operation
         *
         * @param args The argument list
         * @return The result of operation
         */
        Object operator()(std::vector<std::unique_ptr<Object>>& args);


        // !!!!!!!!!!!!!!!!!!!!!!!!!! TO HERE !!!!!!!!!!!!!!!!!!!!!!!!!!!


        ////// STRING CONVERSION //////

        /**
         * Return a string that describe the content of the object
         */
        operator std::string() const;

    };

}

/**
 * Send the content of the object in the stream
 *
 * @param stream The stream
 * @param object The object
 * @return The stream
 */
std::ostream& operator<<(std::ostream& stream, const LiteScript::Object& object);

#endif //LITESCRIPT_MEMORY_VARIABLE_HPP

#endif //LITESCRIPT_LITESCRIPT_HPP
