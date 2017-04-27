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

#ifndef LITESCRIPT_MEMORY_TYPE_HPP
#define LITESCRIPT_MEMORY_TYPE_HPP

namespace LiteScript {

    // Abstract type of an object
    class Type {

    public:

        ///////////////////////////////
        ////// STATIC ATTRIBUTES //////
        ///////////////////////////////

        // Type "nullptr"
        static Type& NIL;
        // Type "bool"
        static Type& BOOLEAN;
        // Type "LiteScript::Number"
        static Type& NUMBER;
        // Type "LiteScript::Character" (only for string)
        static Type& CHARACTER;
        // Type "LiteScript::String"
        static Type& STRING;

    private:

        ///////////////////////////////
        ////// OBJECT ATTRIBUTES //////
        ///////////////////////////////

        // ID of the type
        unsigned int id;

        // Name of the type
        std::string name;

    public:

        /////////////////////////
        ////// CONSTRUCTOR //////
        /////////////////////////

        /**
         * Basic constructor of a type, need to be call once by type in the code
         * The ID is obtained by a static incrementer
         *
         * @param name The name of the type
         */
        Type(const char * name);


        ////////////////////////////////
        ////// NO VIRTUAL METHODS //////
        ////////////////////////////////


        /**
         * Return the ID of the type
         *
         * @return The ID of the type
         */
        unsigned int GetID() const;

        /**
         * Return the name of the type
         *
         * @return The name of the type
         */
        const char * GetName() const;

        /**
         * Determines whether the types are equal (compare the IDs)
         *
         * @return True if equal and false otherwise
         */
        bool operator==(const Type&) const;

        /**
         * Determines whether the types are different (compare the IDs)
         *
         * @return True if equal and false otherwise
         */
        bool operator!=(const Type&) const;


        /////////////////////////////
        ////// VIRTUAL METHODS //////
        /////////////////////////////


        /**
         * Create an empty object formated by this type
         *
         * @return An empty object
         */
        virtual Object CreateObject() = 0;


        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // !!!!!! If the operation can't be applied on the object, !!!!!!
        // !!!!!! the object returned is typed "null"              !!!!!!
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        // !!!!!!!!!!!!!!!!!!!!!!!!! FROM HERE !!!!!!!!!!!!!!!!!!!!!!!!!!


        /**
         * Convert an object to an other object on referenced type format
         *
         * @param object The object to convert
         * @param type The type of conversion
         * @return A new object converted if success, or a new null object otherwise
         */
        virtual Object Convert(const Object& object, const Type& type) const;

        /**
         * Try to assign the objet referenced with default values of this type
         *
         * @param object The object to assign
         * @return The same object referenced
         */
        virtual Object& AssignObject(Object& object);


        ////// OPERATIONS //////


        /**
         * Frees allocated memory of an object (called when an object is destroyed)
         *
         * @param object The object which will be destroyed
         */
        virtual void ODestroy(Object& object);

        /**
         * Assign an object with the content of an other object
         *
         * @param object_target The object which must be assign
         * @param object_src The object which must be copied
         * @return The object assigned referenced
         */
        virtual Object& OAssign(Object& object_target, const Object& object_src) const;


        ////// ARITHMETIC OPERATIONS //////


        /**
         * Create the result object of unary plus operation
         *
         * @param object The object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Object OUnaryPlus(const Object& object) const;

        /**
         * Create the result object of unary minus operation
         *
         * @param object The object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Object OUnaryMinus(const Object& object) const;

        /**
         * Apply an increment operation and return referenced object
         *
         * @param object The object operand
         * @return The object referenced
         */
        virtual Object& OPreIncrement(Object& object) const;

        /**
         * Create an object copy of the referenced object and apply an increment operation on the
         * referenced object
         *
         * @param object The object operand
         * @return The object copy
         */
        virtual Object OPostIncrement(Object& object) const;

        /**
         * Apply a decrement operation and return referenced object
         *
         * @param object The object operand
         * @return The object referenced
         */
        virtual Object& OPreDecrement(Object& object) const;

        /**
         * Create an object copy of the referenced object and apply a decrement operation on the
         * referenced object
         *
         * @param object The object operand
         * @return The object copy
         */
        virtual Object OPostDecrement(Object& object) const;



        /**
         * Create the result object of addition operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Object OAdd(const Object& object_1, const Object& object_2) const;

        /**
         * Create the result object of substraction operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Object OSubstract(const Object& object_1, const Object& object_2) const;

        /**
         * Create the result object of multiplication operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Object OMultiply(const Object& object_1, const Object& object_2) const;

        /**
         * Create the result object of division operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Object ODivide(const Object& object_1, const Object& object_2) const;

        /**
         * Create the result object of modulo operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Object OModulo(const Object& object_1, const Object& object_2) const;


        ////// COMPARISON OPERATIONS //////


        /**
         * Create the result object of equality comparison
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        virtual Object OEqual(const Object& object_1, const Object& object_2) const;

        /**
         * Create the result object of inequality comparison
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        virtual Object ONotEqual(const Object& object_1, const Object& object_2) const;

        /**
         * Create the result object of superiority comparison
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        virtual Object OGreater(const Object& object_1, const Object& object_2) const;

        /**
         * Create the result object of inferiority comparison
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        virtual Object OLess(const Object& object_1, const Object& object_2) const;

        /**
         * Create the result object of superiority or equality comparison
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        virtual Object OGreaterOrEqual(const Object& object_1, const Object& object_2) const;

        /**
         * Create the result object of inferiority or equality comparison
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        virtual Object OLessOrEqual(const Object& object_1, const Object& object_2) const;


        ////// LOGICAL OPERATIONS //////


        /**
         * Create the result object of logical not operation
         *
         * @param object The object operand
         * @return The result of operation as boolean object
         */
        virtual Object OLogicalNot(const Object& object) const;

        /**
         * Create the result object of logical and operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        virtual Object OLogicalAnd(const Object& object_1, const Object& object_2) const;

        /**
         * Create the result object of logical or operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        virtual Object OLogicalOr(const Object& object_1, const Object& object_2) const;


        ////// BINARY OPERATIONS //////


        /**
         * Create the result object of binary not operation
         *
         * @param object The object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Object OBitwiseNot(const Object& object) const;

        /**
         * Create the result object of binary and operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Object OBitwiseAnd(const Object& object_1, const Object& object_2) const;

        /**
         * Create the result object of binary or operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Object OBitwiseOr(const Object& object_1, const Object& object_2) const;

        /**
         * Create the result object of binary xor operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Object OBitwiseXor(const Object& object_1, const Object& object_2) const;

        /**
         * Create the result object of left shift operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Object OLeftShift(const Object& object_1, const Object& object_2) const;

        /**
         * Create the result object of right shift operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Object ORightShift(const Object& object_1, const Object& object_2) const;


        ////// DIRECT MODIFICATION OPERATIONS //////


        /**
         * Add the source object to the target object and return it
         *
         * @param object_target The target object
         * @param object_src The source object
         * @return The target object referenced
         */
        virtual Object& OAddAndAssign(Object& object_target, const Object& object_src) const;

        /**
         * Substract the source object to the target object and return it
         *
         * @param object_target The target object
         * @param object_src The source object
         * @return The target object referenced
         */
        virtual Object& OSubstractAndAssign(Object& object_target, const Object& object_src) const;

        /**
         * Multiply the source object to the target object and return it
         *
         * @param object_target The target object
         * @param object_src The source object
         * @return The target object referenced
         */
        virtual Object& OMultiplyAndAssign(Object& object_target, const Object& object_src) const;

        /**
         * Divide the source object to the target object and return it
         *
         * @param object_target The target object
         * @param object_src The source object
         * @return The target object referenced
         */
        virtual Object& ODivideAndAssign(Object& object_target, const Object& object_src) const;


        // !!!!!!!!!!!!!!!!!!!!!!!!!! TO HERE !!!!!!!!!!!!!!!!!!!!!!!!!!!


        ////// ACCESS OPERATIONS //////


        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // !!!!!! If the operation can't be applied on the object, !!!!!!
        // !!!!!! the object returned is typed "undefined"         !!!!!!
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        // !!!!!!!!!!!!!!!!!!!!!!!!! FROM HERE !!!!!!!!!!!!!!!!!!!!!!!!!!


        /**
         * Return the object contained in the object source and identified by the key object
         *
         * @param object_src The object source
         * @param object_key The object key
         * @return The object contained in the object source if success or the undefined object otherwise
         */
        virtual Object& OArray(Object& object_src, const Object& object_key) const;

        /**
         * Return the object contained in the object source and identified by the member name
         *
         * @param object_src The object source
         * @param member_name The member name
         * @return The object contained in the object source if success or the undefined object otherwise
         */
        virtual Object& OMember(Object& object_src, const char * member_name) const;


        // !!!!!!!!!!!!!!!!!!!!!!!!!! TO HERE !!!!!!!!!!!!!!!!!!!!!!!!!!!


        ////// CALLING OPERATION //////


        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // !!!!!! If the operation can't be applied on the object, !!!!!!
        // !!!!!! the object returned is typed "null"              !!!!!!
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        // !!!!!!!!!!!!!!!!!!!!!!!!! FROM HERE !!!!!!!!!!!!!!!!!!!!!!!!!!


        /**
         * Return the result of calling operation
         *
         * @param object The callable object
         * @param args The argument list
         * @return The return result of calling operation
         */
        virtual Object OCall(Object& object, std::vector<std::unique_ptr<Object>>& args) const;


        // !!!!!!!!!!!!!!!!!!!!!!!!!! TO HERE !!!!!!!!!!!!!!!!!!!!!!!!!!!


        ////// STRING CONVERSION //////


        /**
         * Return a string for describe the content of the object referenced
         *
         * @param object The object to describe
         * @return A description string
         */
        virtual std::string ToString(const Object& object) const;

    };

}

#endif //LITESCRIPT_MEMORY_TYPE_HPP

#endif //LITESCRIPT_LITESCRIPT_HPP
