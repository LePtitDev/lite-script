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

#ifndef LITESCRIPT_TYPES_CLASS_OBJECT_HPP
#define LITESCRIPT_TYPES_CLASS_OBJECT_HPP

#include "../litescript.hpp"

namespace LiteScript {

    // The derived type CLASS OBJECT
    class _Type_CLASS_OBJECT : public Type {

    public:

        /**
         * Basic constructor
         */
        _Type_CLASS_OBJECT();

        /**
         * Try to assign the objet referenced with default values of this type
         *
         * @param object The object to assign
         * @return The same object referenced
         */
        Object& AssignObject(Object& obj) override;

        /**
         * Assign an object with the content of an other object
         *
         * @param object_target The object which must be assign
         * @param object_src The object which must be copied
         * @return The object assigned referenced
         */
        Variable OAssign(Variable& object_target, const Variable& object_src) const override;

        /**
         * Create the result object of unary plus operation
         *
         * @param object The object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        Variable OUnaryPlus(const Variable& object) const override;

        /**
         * Create the result object of unary minus operation
         *
         * @param object The object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        Variable OUnaryMinus(const Variable& object) const override;

        /**
         * Apply an increment operation and return referenced object
         *
         * @param object The object operand
         * @return The object referenced
         */
        Variable OPreIncrement(Variable& object) const override;

        /**
         * Create an object copy of the referenced object and apply an increment operation on the
         * referenced object
         *
         * @param object The object operand
         * @return The object copy
         */
        Variable OPostIncrement(Variable& object) const override;

        /**
         * Apply a decrement operation and return referenced object
         *
         * @param object The object operand
         * @return The object referenced
         */
        Variable OPreDecrement(Variable& object) const override;

        /**
         * Create an object copy of the referenced object and apply a decrement operation on the
         * referenced object
         *
         * @param object The object operand
         * @return The object copy
         */
        Variable OPostDecrement(Variable& object) const override;

        /**
         * Create the result object of addition operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        Variable OAdd(const Variable& object_1, const Variable& object_2) const override;

        /**
         * Create the result object of substraction operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        Variable OSubstract(const Variable& object_1, const Variable& object_2) const override;

        /**
         * Create the result object of multiplication operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        Variable OMultiply(const Variable& object_1, const Variable& object_2) const override;

        /**
         * Create the result object of division operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        Variable ODivide(const Variable& object_1, const Variable& object_2) const override;

        /**
         * Create the result object of modulo operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        Variable OModulo(const Variable& object_1, const Variable& object_2) const override;

        /**
         * Create the result object of equality comparison
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        Variable OEqual(const Variable& object_1, const Variable& object_2) const override;

        /**
         * Create the result object of inequality comparison
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        Variable ONotEqual(const Variable& object_1, const Variable& object_2) const override;

        /**
         * Create the result object of superiority comparison
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        Variable OGreater(const Variable& object_1, const Variable& object_2) const override;

        /**
         * Create the result object of inferiority comparison
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        Variable OLess(const Variable& object_1, const Variable& object_2) const override;

        /**
         * Create the result object of superiority or equality comparison
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        Variable OGreaterOrEqual(const Variable& object_1, const Variable& object_2) const override;

        /**
         * Create the result object of inferiority or equality comparison
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        Variable OLessOrEqual(const Variable& object_1, const Variable& object_2) const override;

        /**
         * Create the result object of logical not operation
         *
         * @param object The object operand
         * @return The result of operation as boolean object
         */
        Variable OLogicalNot(const Variable& object) const override;

        /**
         * Create the result object of logical and operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        Variable OLogicalAnd(const Variable& object_1, const Variable& object_2) const override;

        /**
         * Create the result object of logical or operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation as boolean object
         */
        Variable OLogicalOr(const Variable& object_1, const Variable& object_2) const override;

        /**
         * Create the result object of binary not operation
         *
         * @param object The object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        Variable OBitwiseNot(const Variable& object) const override;

        /**
         * Create the result object of binary and operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        Variable OBitwiseAnd(const Variable& object_1, const Variable& object_2) const override;

        /**
         * Create the result object of binary or operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        Variable OBitwiseOr(const Variable& object_1, const Variable& object_2) const override;

        /**
         * Create the result object of binary xor operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        Variable OBitwiseXor(const Variable& object_1, const Variable& object_2) const override;

        /**
         * Create the result object of left shift operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        Variable OLeftShift(const Variable& object_1, const Variable& object_2) const override;

        /**
         * Create the result object of right shift operation
         *
         * @param object_1 The first object operand
         * @param object_2 The second object operand
         * @return The result of operation (null object if the type doesn't permit this operation)
         */
        Variable ORightShift(const Variable& object_1, const Variable& object_2) const override;
        
        /**
         * Add the source object to the target object and return it
         *
         * @param object_target The target object
         * @param object_src The source object
         * @return The target object referenced
         */
        Variable OAddAndAssign(Variable& object_target, const Variable& object_src) const override;

        /**
         * Substract the source object to the target object and return it
         *
         * @param object_target The target object
         * @param object_src The source object
         * @return The target object referenced
         */
        Variable OSubstractAndAssign(Variable& object_target, const Variable& object_src) const override;

        /**
         * Multiply the source object to the target object and return it
         *
         * @param object_target The target object
         * @param object_src The source object
         * @return The target object referenced
         */
        Variable OMultiplyAndAssign(Variable& object_target, const Variable& object_src) const override;

        /**
         * Divide the source object to the target object and return it
         *
         * @param object_target The target object
         * @param object_src The source object
         * @return The target object referenced
         */
        Variable ODivideAndAssign(Variable& object_target, const Variable& object_src) const override;
        
        /**
         * Return the object contained in the object source and identified by the key object
         *
         * @param object_src The object source
         * @param object_key The object key
         * @return The object contained in the object source if success or the undefined object otherwise
         */
        Variable OArray(Variable& object_src, const Variable& object_key) const override;

        /**
         * Return the object contained in the object source and identified by the member name
         *
         * @param object_src The object source
         * @param member_name The member name
         * @return The object contained in the object source if success or the undefined object otherwise
         */
        Variable OMember(Variable& object_src, const char * member_name) const override;

        /**
         * Return the result of calling operation
         *
         * @param object The callable object
         * @param state The script state
         * @param args The argument list
         * @return The return result of calling operation
         */
        Variable OCall(Variable& object, State& state, std::vector<Variable>& args) const override;


        /**
         * Return a string for describe the content of the object referenced
         *
         * @param object The object to describe
         * @return A description string
         */
        std::string ToString(const Variable& object) const override;

    };

    extern _Type_CLASS_OBJECT _type_class_object;

}

#endif //LITESCRIPT_CLASS_OBJECT_HPP
