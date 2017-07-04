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

        // Type "undefined"
        static Type& UNDEFINED;
        // Type "nullptr"
        static Type& NIL;
        // Type "bool"
        static Type& BOOLEAN;
        // Type "LiteScript::Number"
        static Type& NUMBER;
        // Type "LiteScript::String"
        static Type& STRING;
        // Type "LiteScript::Callback"
        static Type& CALLBACK;
        // Type "LiteScript::Array"
        static Type& ARRAY;
        // Type "LiteScript::Class"
        static Type& CLASS;
        // Type "LiteScript::Namespace"
        static Type& NAMESPACE;

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
        virtual Variable Convert(const Variable& object, const Type& type) const;

        /**
         * Try to assign the objet referenced with default values of this type
         *
         * @param object The object to assign
         * @return The same object referenced
         */
        virtual Object& AssignObject(Object& object) = 0;


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
        virtual Variable OAssign(Variable& object_target, const Variable& object_src) const;


        ////// ARITHMETIC OPERATIONS //////


        /**
         * Create the result object of unary plus operation
         *
         * @param object The object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Variable OUnaryPlus(const Variable& object) const;

        /**
         * Create the result object of unary minus operation
         *
         * @param object The object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Variable OUnaryMinus(const Variable& object) const;

        /**
         * Apply an increment operation and return referenced object
         *
         * @param object The object operand
         * @return The object referenced
         */
        virtual Variable OPreIncrement(Variable& object) const;

        /**
         * Create an object copy of the referenced object and apply an increment operation on the
         * referenced object
         *
         * @param object The object operand
         * @return The object copy
         */
        virtual Variable OPostIncrement(Variable& object) const;

        /**
         * Apply a decrement operation and return referenced object
         *
         * @param object The object operand
         * @return The object referenced
         */
        virtual Variable OPreDecrement(Variable& object) const;

        /**
         * Create an object copy of the referenced object and apply a decrement operation on the
         * referenced object
         *
         * @param object The object operand
         * @return The object copy
         */
        virtual Variable OPostDecrement(Variable& object) const;



        /**
         * Create the result object of addition operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Variable OAdd(const Variable& object_1, const Variable& object_2) const;

        /**
         * Create the result object of substraction operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Variable OSubstract(const Variable& object_1, const Variable& object_2) const;

        /**
         * Create the result object of multiplication operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Variable OMultiply(const Variable& object_1, const Variable& object_2) const;

        /**
         * Create the result object of division operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Variable ODivide(const Variable& object_1, const Variable& object_2) const;

        /**
         * Create the result object of modulo operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Variable OModulo(const Variable& object_1, const Variable& object_2) const;


        ////// COMPARISON OPERATIONS //////


        /**
         * Create the result object of equality comparison
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        virtual Variable OEqual(const Variable& object_1, const Variable& object_2) const;

        /**
         * Create the result object of inequality comparison
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        virtual Variable ONotEqual(const Variable& object_1, const Variable& object_2) const;

        /**
         * Create the result object of superiority comparison
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        virtual Variable OGreater(const Variable& object_1, const Variable& object_2) const;

        /**
         * Create the result object of inferiority comparison
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        virtual Variable OLess(const Variable& object_1, const Variable& object_2) const;

        /**
         * Create the result object of superiority or equality comparison
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        virtual Variable OGreaterOrEqual(const Variable& object_1, const Variable& object_2) const;

        /**
         * Create the result object of inferiority or equality comparison
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        virtual Variable OLessOrEqual(const Variable& object_1, const Variable& object_2) const;


        ////// LOGICAL OPERATIONS //////


        /**
         * Create the result object of logical not operation
         *
         * @param object The object operand
         * @return The result of operation as boolean object
         */
        virtual Variable OLogicalNot(const Variable& object) const;

        /**
         * Create the result object of logical and operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        virtual Variable OLogicalAnd(const Variable& object_1, const Variable& object_2) const;

        /**
         * Create the result object of logical or operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        virtual Variable OLogicalOr(const Variable& object_1, const Variable& object_2) const;


        ////// BINARY OPERATIONS //////


        /**
         * Create the result object of binary not operation
         *
         * @param object The object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Variable OBitwiseNot(const Variable& object) const;

        /**
         * Create the result object of binary and operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Variable OBitwiseAnd(const Variable& object_1, const Variable& object_2) const;

        /**
         * Create the result object of binary or operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Variable OBitwiseOr(const Variable& object_1, const Variable& object_2) const;

        /**
         * Create the result object of binary xor operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Variable OBitwiseXor(const Variable& object_1, const Variable& object_2) const;

        /**
         * Create the result object of left shift operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Variable OLeftShift(const Variable& object_1, const Variable& object_2) const;

        /**
         * Create the result object of right shift operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        virtual Variable ORightShift(const Variable& object_1, const Variable& object_2) const;


        ////// DIRECT MODIFICATION OPERATIONS //////


        /**
         * Add the source object to the target object and return it
         *
         * @param object_target The target object
         * @param object_src The source object
         * @return The target object referenced
         */
        virtual Variable OAddAndAssign(Variable& object_target, const Variable& object_src) const;

        /**
         * Substract the source object to the target object and return it
         *
         * @param object_target The target object
         * @param object_src The source object
         * @return The target object referenced
         */
        virtual Variable OSubstractAndAssign(Variable& object_target, const Variable& object_src) const;

        /**
         * Multiply the source object to the target object and return it
         *
         * @param object_target The target object
         * @param object_src The source object
         * @return The target object referenced
         */
        virtual Variable OMultiplyAndAssign(Variable& object_target, const Variable& object_src) const;

        /**
         * Divide the source object to the target object and return it
         *
         * @param object_target The target object
         * @param object_src The source object
         * @return The target object referenced
         */
        virtual Variable ODivideAndAssign(Variable& object_target, const Variable& object_src) const;


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
        virtual Variable OArray(Variable& object_src, const Variable& object_key) const;

        /**
         * Return the object contained in the object source and identified by the member name
         *
         * @param object_src The object source
         * @param member_name The member name
         * @return The object contained in the object source if success or the undefined object otherwise
         */
        virtual Variable OMember(Variable& object_src, const char * member_name) const;


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
        virtual Variable OCall(Variable& object, std::vector<Variable>& args) const;


        // !!!!!!!!!!!!!!!!!!!!!!!!!! TO HERE !!!!!!!!!!!!!!!!!!!!!!!!!!!


        ////// STRING CONVERSION //////


        /**
         * Return a string for describe the content of the object referenced
         *
         * @param object The object to describe
         * @return A description string
         */
        virtual std::string ToString(const Variable& object) const;

    };

}

#endif //LITESCRIPT_MEMORY_TYPE_HPP

#endif //LITESCRIPT_LITESCRIPT_HPP
