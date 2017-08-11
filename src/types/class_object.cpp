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

#include "class_object.hpp"

LiteScript::_Type_CLASS_OBJECT LiteScript::_type_class_object;

LiteScript::_Type_CLASS_OBJECT::_Type_CLASS_OBJECT() : Type("CLASS OBJECT") {}

LiteScript::Object & LiteScript::_Type_CLASS_OBJECT::AssignObject(Object &obj) {
    obj.Reassign(*this, sizeof(ClassObject));
    std::allocator<ClassObject> allocator;
    allocator.construct(&obj.GetData<ClassObject>());
    return obj;
}

std::string LiteScript::_Type_CLASS_OBJECT::ToString(const Variable &object) const {
    std::stringstream ss;
    const ClassObject& co = object->GetData<ClassObject>();
    ss << "class(" << &(*co.ClassBase) << "):{";
    for (unsigned int i = 0, sz = co.GetMemberCount(); i < sz; i++) {
        ss << co.GetMemberName(i) << ":" << ((std::string)(co.GetMemberVariable(i))).c_str();
        if (i < sz - 1)
            ss << ",";
    }
    ss << "}";
    return ss.str();
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OAssign(Variable &object_target, const Variable &object_src) const {
    Variable v = object_target->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_ASSIGN);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_target);
        std::vector<Variable> args;
        args.push_back(Variable(object_src));
        return call(*(object_target->GetData<ClassObject>().ScriptState), args);
    }
    else {
        object_src->GetType().AssignObject(*object_target);
        object_target->GetType().OAssign(object_target, object_src);
    }
    return object_src;
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OUnaryPlus(const Variable& object) const {
    Variable v = object->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_UNARY_PLUS);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object);
        std::vector<Variable> args;
        return call(*(Variable(object)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OUnaryMinus(const Variable& object) const {
    Variable v = object->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_UNARY_MINUS);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object);
        std::vector<Variable> args;
        return call(*(Variable(object)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OPreIncrement(Variable& object) const {
    Variable v = object->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_PRE_INCR);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object);
        std::vector<Variable> args;
        return call(*(object->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OPostIncrement(Variable& object) const {
    Variable v = object->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_POST_INCR);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object);
        std::vector<Variable> args;
        return call(*(object->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OPreDecrement(Variable& object) const {
    Variable v = object->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_PRE_DECR);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object);
        std::vector<Variable> args;
        return call(*(object->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OPostDecrement(Variable& object) const {
    Variable v = object->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_POST_DECR);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object);
        std::vector<Variable> args;
        return call(*(object->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OAdd(const Variable& object_1, const Variable& object_2) const {
    Variable v = object_1->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_ADD);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_1);
        std::vector<Variable> args;
        args.push_back(Variable(object_2));
        return call(*(Variable(object_1)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_1->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OSubstract(const Variable& object_1, const Variable& object_2) const {
    Variable v = object_1->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_SUB);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_1);
        std::vector<Variable> args;
        args.push_back(Variable(object_2));
        return call(*(Variable(object_1)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_1->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OMultiply(const Variable& object_1, const Variable& object_2) const {
    Variable v = object_1->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_MUL);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_1);
        std::vector<Variable> args;
        args.push_back(Variable(object_2));
        return call(*(Variable(object_1)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_1->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::ODivide(const Variable& object_1, const Variable& object_2) const {
    Variable v = object_1->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_DIV);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_1);
        std::vector<Variable> args;
        args.push_back(Variable(object_2));
        return call(*(Variable(object_1)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_1->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OModulo(const Variable& object_1, const Variable& object_2) const {
    Variable v = object_1->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_MOD);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_1);
        std::vector<Variable> args;
        args.push_back(Variable(object_2));
        return call(*(Variable(object_1)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_1->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OEqual(const Variable& object_1, const Variable& object_2) const {
    Variable v = object_1->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_EQU);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_1);
        std::vector<Variable> args;
        args.push_back(Variable(object_2));
        return call(*(Variable(object_1)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_1->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::ONotEqual(const Variable& object_1, const Variable& object_2) const {
    Variable v = object_1->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_DIF);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_1);
        std::vector<Variable> args;
        args.push_back(Variable(object_2));
        return call(*(Variable(object_1)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_1->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OGreater(const Variable& object_1, const Variable& object_2) const {
    Variable v = object_1->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_GREAT);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_1);
        std::vector<Variable> args;
        args.push_back(Variable(object_2));
        return call(*(Variable(object_1)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_1->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OLess(const Variable& object_1, const Variable& object_2) const {
    Variable v = object_1->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_LESS);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_1);
        std::vector<Variable> args;
        args.push_back(Variable(object_2));
        return call(*(Variable(object_1)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_1->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OGreaterOrEqual(const Variable& object_1, const Variable& object_2) const {
    Variable v = object_1->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_GREAT_EQU);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_1);
        std::vector<Variable> args;
        args.push_back(Variable(object_2));
        return call(*(Variable(object_1)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_1->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OLessOrEqual(const Variable& object_1, const Variable& object_2) const {
    Variable v = object_1->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_LESS_EQU);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_1);
        std::vector<Variable> args;
        args.push_back(Variable(object_2));
        return call(*(Variable(object_1)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_1->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OLogicalNot(const Variable& object) const {
    Variable v = object->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_LOG_NOT);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object);
        std::vector<Variable> args;
        return call(*(Variable(object)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OLogicalAnd(const Variable& object_1, const Variable& object_2) const {
    Variable v = object_1->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_LOG_AND);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_1);
        std::vector<Variable> args;
        args.push_back(Variable(object_2));
        return call(*(Variable(object_1)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_1->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OLogicalOr(const Variable& object_1, const Variable& object_2) const {
    Variable v = object_1->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_LOG_OR);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_1);
        std::vector<Variable> args;
        args.push_back(Variable(object_2));
        return call(*(Variable(object_1)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_1->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OBitwiseNot(const Variable& object) const {
    Variable v = object->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_BIT_NOT);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object);
        std::vector<Variable> args;
        return call(*(Variable(object)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OBitwiseAnd(const Variable& object_1, const Variable& object_2) const {
    Variable v = object_1->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_BIT_AND);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_1);
        std::vector<Variable> args;
        args.push_back(Variable(object_2));
        return call(*(Variable(object_1)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_1->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OBitwiseOr(const Variable& object_1, const Variable& object_2) const {
    Variable v = object_1->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_BIT_OR);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_1);
        std::vector<Variable> args;
        args.push_back(Variable(object_2));
        return call(*(Variable(object_1)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_1->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OBitwiseXor(const Variable& object_1, const Variable& object_2) const {
    Variable v = object_1->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_BIT_XOR);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_1);
        std::vector<Variable> args;
        args.push_back(Variable(object_2));
        return call(*(Variable(object_1)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_1->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OLeftShift(const Variable& object_1, const Variable& object_2) const {
    Variable v = object_1->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_LSHIFT);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_1);
        std::vector<Variable> args;
        args.push_back(Variable(object_2));
        return call(*(Variable(object_1)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_1->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::ORightShift(const Variable& object_1, const Variable& object_2) const {
    Variable v = object_1->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_RSHIFT);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_1);
        std::vector<Variable> args;
        args.push_back(Variable(object_2));
        return call(*(Variable(object_1)->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_1->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OAddAndAssign(Variable& object_target, const Variable& object_src) const {
    Variable v = object_target->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_ADD_ASSIGN);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_target);
        std::vector<Variable> args;
        args.push_back(Variable(object_src));
        return call(*(object_target->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_target->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OSubstractAndAssign(Variable& object_target, const Variable& object_src) const {
    Variable v = object_target->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_SUB_ASSIGN);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_target);
        std::vector<Variable> args;
        args.push_back(Variable(object_src));
        return call(*(object_target->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_target->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OMultiplyAndAssign(Variable& object_target, const Variable& object_src) const {
    Variable v = object_target->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_MUL_ASSIGN);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_target);
        std::vector<Variable> args;
        args.push_back(Variable(object_src));
        return call(*(object_target->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_target->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::ODivideAndAssign(Variable& object_target, const Variable& object_src) const {
    Variable v = object_target->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_DIV_ASSIGN);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_target);
        std::vector<Variable> args;
        args.push_back(Variable(object_src));
        return call(*(object_target->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_target->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OArray(Variable& object_src, const Variable& object_key) const {
    Variable v = object_src->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_ARRAY);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object_src);
        std::vector<Variable> args;
        args.push_back(Variable(object_key));
        return call(*(object_src->GetData<ClassObject>().ScriptState), args);
    }
    else {
        return object_src->memory.Create(Type::NIL);
    }
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OMember(Variable& object_src, const char * member_name) const {
    return object_src->GetData<ClassObject>().GetMember(member_name);
}

LiteScript::Variable LiteScript::_Type_CLASS_OBJECT::OCall(Variable& object, State& state, std::vector<Variable>& args) const {
    Variable v = object->GetData<ClassObject>().ClassBase->GetOperator(Class::OperatorType::OP_TYPE_CALL);
    if (v->GetType() == Type::CALLBACK) {
        Callback &call = v->GetData<Callback>();
        call.This = Nullable<Variable>(object);
        return call(state, args);
    }
    else {
        return object->memory.Create(Type::NIL);
    }
}
