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

#include "executor.hpp"

void LiteScript::StateExecutor::Execute(State &state, Instruction &instr) {
    StateExecutor::ARRAY[instr.code](state, instr);
}

// THE FUNCTION ARRAY
std::array<void(*)(LiteScript::State&, LiteScript::Instruction&), LiteScript::InstrCode::INSTR_NUMBER> LiteScript::StateExecutor::ARRAY({
    LiteScript::StateExecutor::I_INVALID,

    LiteScript::StateExecutor::I_DEFINE_VARIABLE,
    LiteScript::StateExecutor::I_DEFINE_ARG,
    LiteScript::StateExecutor::I_DEFINE_RETURN,

    LiteScript::StateExecutor::I_VALUE_POP,
    LiteScript::StateExecutor::I_VALUE_COPY,
    LiteScript::StateExecutor::I_VALUE_UNDEFINED,
    LiteScript::StateExecutor::I_VALUE_NULL,
    LiteScript::StateExecutor::I_VALUE_BOOLEAN,
    LiteScript::StateExecutor::I_VALUE_NUMBER,
    LiteScript::StateExecutor::I_VALUE_STRING,
    LiteScript::StateExecutor::I_VALUE_CALLBACK,
    LiteScript::StateExecutor::I_VALUE_ARRAY,
    LiteScript::StateExecutor::I_VALUE_CLASS,
    LiteScript::StateExecutor::I_VALUE_OBJECT,
    LiteScript::StateExecutor::I_VALUE_ARG,
    LiteScript::StateExecutor::I_VALUE_ARGS,
    LiteScript::StateExecutor::I_VALUE_THIS,
    LiteScript::StateExecutor::I_VALUE_VARIABLE,
    
    LiteScript::StateExecutor::I_PUSH_NSP,
    LiteScript::StateExecutor::I_PUSH_ARGS,
    LiteScript::StateExecutor::I_POP_NSP,
    LiteScript::StateExecutor::I_POP_ARGS,
    LiteScript::StateExecutor::I_RETURN,
    
    LiteScript::StateExecutor::I_OP_ASSIGN,
    LiteScript::StateExecutor::I_OP_UNARY_PLUS,
    LiteScript::StateExecutor::I_OP_UNARY_MINUS,
    LiteScript::StateExecutor::I_OP_PRE_INCR,
    LiteScript::StateExecutor::I_OP_POST_INCR,
    LiteScript::StateExecutor::I_OP_PRE_DECR,
    LiteScript::StateExecutor::I_OP_POST_DECR,
    
    LiteScript::StateExecutor::I_OP_ADD,
    LiteScript::StateExecutor::I_OP_SUB,
    LiteScript::StateExecutor::I_OP_MUL,
    LiteScript::StateExecutor::I_OP_DIV,
    LiteScript::StateExecutor::I_OP_MOD,
    
    LiteScript::StateExecutor::I_OP_EQU,
    LiteScript::StateExecutor::I_OP_DIF,
    LiteScript::StateExecutor::I_OP_GREAT,
    LiteScript::StateExecutor::I_OP_LESS,
    LiteScript::StateExecutor::I_OP_GREAT_EQU,
    LiteScript::StateExecutor::I_OP_LESS_EQU,
    
    LiteScript::StateExecutor::I_OP_LOG_NOT,
    LiteScript::StateExecutor::I_OP_LOG_AND,
    LiteScript::StateExecutor::I_OP_LOG_OR,
    
    LiteScript::StateExecutor::I_OP_BIT_NOT,
    LiteScript::StateExecutor::I_OP_BIT_AND,
    LiteScript::StateExecutor::I_OP_BIT_OR,
    LiteScript::StateExecutor::I_OP_BIT_XOR,
    LiteScript::StateExecutor::I_OP_LSHIFT,
    LiteScript::StateExecutor::I_OP_RSHIFT,
    
    LiteScript::StateExecutor::I_OP_ADD_ASSIGN,
    LiteScript::StateExecutor::I_OP_SUB_ASSIGN,
    LiteScript::StateExecutor::I_OP_MUL_ASSIGN,
    LiteScript::StateExecutor::I_OP_DIV_ASSIGN,
    
    LiteScript::StateExecutor::I_OP_ARRAY,
    LiteScript::StateExecutor::I_OP_MEMBER,
    LiteScript::StateExecutor::I_OP_CALL,
    
    LiteScript::StateExecutor::I_JUMP_TO,
    LiteScript::StateExecutor::I_JUMP_IF,
    LiteScript::StateExecutor::I_JUMP_ELSE,

    LiteScript::StateExecutor::I_ARRAY_PUSH_NUMERIC,
    LiteScript::StateExecutor::I_ARRAY_PUSH_LITERAL,

    LiteScript::StateExecutor::I_CLASS_PUSH_STATIC,
    LiteScript::StateExecutor::I_CLASS_PUSH_USTATIC,
    LiteScript::StateExecutor::I_CLASS_PUSH_OPERATOR,
    LiteScript::StateExecutor::I_CLASS_INHERIT,
    LiteScript::StateExecutor::I_CLASS_CONSTRUCTOR,

    LiteScript::StateExecutor::I_NAMESPACE_USE
});

/////////////////////////////
////// PRIVATE METHODS //////
/////////////////////////////

void LiteScript::StateExecutor::PopValue(State &st) {
    if (st.op_lifo.back()->GetType() == Type::CLASS)
        st.nsp_lifo.back().Pop(st.op_lifo.back());
    st.op_lifo.pop_back();
}

// INVALID
void LiteScript::StateExecutor::I_INVALID(State& state, Instruction& instr) {
    state.line_num++;
}

// DEFINITIONS
void LiteScript::StateExecutor::I_DEFINE_VARIABLE(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() == 0 || instr.comp_type != Instruction::CompType::COMP_TYPE_STRING)
        return;
    Variable v = state.GetVariable(instr.comp_value.v_string);
    if (v->GetType() != Type::UNDEFINED)
        return;
    state.nsp_lifo.back().Declare(instr.comp_value.v_string, state.op_lifo.back());
    PopValue(state);
}
void LiteScript::StateExecutor::I_DEFINE_ARG(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() == 0 || state.args.size() == 0 || instr.comp_type != Instruction::CompType::COMP_TYPE_INTEGER)
        return;
    std::vector<Variable>& args = state.args.back();
    while (args.size() < instr.comp_value.v_integer)
        args.push_back(state.memory.Create(Type::UNDEFINED));
    args.emplace(args.begin() + instr.comp_value.v_integer, state.op_lifo.back());
    PopValue(state);
}
void LiteScript::StateExecutor::I_DEFINE_RETURN(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() == 0)
        return;
    state.rets.back() = Nullable<Variable>(state.op_lifo.back());
    PopValue(state);
}

// VALUE CREATIONS
void LiteScript::StateExecutor::I_VALUE_POP(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() > 0)
        PopValue(state);
}
void LiteScript::StateExecutor::I_VALUE_COPY(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() > 0)
        state.op_lifo.push_back(Variable(state.op_lifo.back()));
}
void LiteScript::StateExecutor::I_VALUE_UNDEFINED(State& state, Instruction& instr) {
    state.line_num++;
    state.op_lifo.push_back(state.memory.Create(Type::UNDEFINED));
}
void LiteScript::StateExecutor::I_VALUE_NULL(State& state, Instruction& instr) {
    state.line_num++;
    state.op_lifo.push_back(state.memory.Create(Type::NIL));
}
void LiteScript::StateExecutor::I_VALUE_BOOLEAN(State& state, Instruction& instr) {
    state.line_num++;
    if (instr.comp_type != Instruction::CompType::COMP_TYPE_BOOLEAN)
        return;
    state.op_lifo.push_back(state.memory.Create(Type::BOOLEAN));
    state.op_lifo.back()->GetData<bool>() = instr.comp_value.v_boolean;
}
void LiteScript::StateExecutor::I_VALUE_NUMBER(State& state, Instruction& instr) {
    state.line_num++;
    Number number;
    if (instr.comp_type == Instruction::CompType::COMP_TYPE_INTEGER)
        number = Number(instr.comp_value.v_integer);
    else if (instr.comp_type == Instruction::CompType::COMP_TYPE_FLOAT)
        number = Number(instr.comp_value.v_float);
    else
        return;
    state.op_lifo.push_back(state.memory.Create(Type::NUMBER));
    state.op_lifo.back()->GetData<Number>() = number;
}
void LiteScript::StateExecutor::I_VALUE_STRING(State& state, Instruction& instr) {
    state.line_num++;
    if (instr.comp_type != Instruction::CompType::COMP_TYPE_STRING)
        return;
    state.op_lifo.push_back(state.memory.Create(Type::STRING));
    state.op_lifo.back()->GetData<String>() = String(instr.comp_value.v_string);
}
void LiteScript::StateExecutor::I_VALUE_CALLBACK(State& state, Instruction& instr) {
    state.line_num++;
    if (instr.comp_type != Instruction::CompType::COMP_TYPE_INTEGER)
        return;
    state.op_lifo.push_back(state.memory.Create(Type::CALLBACK));
    state.op_lifo.back()->GetData<Callback>() = Callback(state, state.instr_index, (unsigned int)instr.comp_value.v_integer);
}
void LiteScript::StateExecutor::I_VALUE_ARRAY(State& state, Instruction& instr) {
    state.line_num++;
    state.op_lifo.push_back(state.memory.Create(Type::ARRAY));
}
void LiteScript::StateExecutor::I_VALUE_CLASS(State& state, Instruction& instr) {
    state.line_num++;
    state.op_lifo.push_back(state.memory.Create(Type::CLASS));
    state.nsp_lifo.back().Push(state.op_lifo.back());
}
void LiteScript::StateExecutor::I_VALUE_OBJECT(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() == 0 || state.args.size() == 0)
        return;
    Variable oc(state.op_lifo.back());
    PopValue(state);
    if (oc->GetType() != Type::CLASS)
        return;
    std::vector<Variable>& args = state.args.back();
    state.op_lifo.push_back(oc->GetData<Class>().CreateElement(args));
}
void LiteScript::StateExecutor::I_VALUE_ARG(State& state, Instruction& instr) {
    state.line_num++;
    if (instr.comp_type != Instruction::CompType::COMP_TYPE_INTEGER)
        return;
    state.op_lifo.push_back(state.GetArg((unsigned int)instr.comp_value.v_integer));
}
void LiteScript::StateExecutor::I_VALUE_ARGS(State& state, Instruction& instr) {
    state.line_num++;
    if (instr.comp_type != Instruction::CompType::COMP_TYPE_INTEGER)
        return;
    state.op_lifo.push_back(state.memory.Create(Type::ARRAY));
    Array& a = state.op_lifo.back()->GetData<Array>();
    for (unsigned int i = (unsigned int)instr.comp_value.v_integer, sz = state.GetArgsCount(); i < sz; i++)
        a[i] = state.GetArg(i);
}
void LiteScript::StateExecutor::I_VALUE_THIS(State& state, Instruction& instr) {
    state.line_num++;
    state.op_lifo.push_back(Variable(*state.ths.back()));
}
void LiteScript::StateExecutor::I_VALUE_VARIABLE(State& state, Instruction& instr) {
    state.line_num++;
    if (instr.comp_type != Instruction::CompType::COMP_TYPE_STRING)
        return;
    state.op_lifo.push_back(state.GetVariable(instr.comp_value.v_string));
}

// PILES MANAGEMENT
void LiteScript::StateExecutor::I_PUSH_NSP(State& state, Instruction& instr) {
    state.line_num++;
    state.nsp_lifo.push_back(Namer(state.nsp_lifo.back()));
    state.ths.push_back(state.memory.Create(Type::UNDEFINED));
    state.rets.push_back(Nullable<Variable>(state.memory.Create(Type::UNDEFINED)));
}
void LiteScript::StateExecutor::I_PUSH_ARGS(State& state, Instruction& instr) {
    state.line_num++;
    state.args.push_back(std::vector<Variable>());
}
void LiteScript::StateExecutor::I_POP_NSP(State& state, Instruction& instr) {
    state.line_num++;
    if (state.rets.size() > 0) {
        state.op_lifo.push_back(Variable(*state.rets.back()));
        state.rets.pop_back();
    }
    if (state.ths.size() > 0)
        state.ths.pop_back();
    if (state.nsp_lifo.size() > 1)
        state.nsp_lifo.pop_back();
}
void LiteScript::StateExecutor::I_POP_ARGS(State& state, Instruction& instr) {
    state.line_num++;
    if (state.args.size() > 0)
        state.args.pop_back();
}
void LiteScript::StateExecutor::I_RETURN(State& state, Instruction& instr) {
    state.line_num++;
    Nullable<Variable> obj;
    if (state.op_lifo.size() > 0) {
        if (state.op_lifo.back()->GetType() != Type::UNDEFINED)
            obj = state.op_lifo.back();
        else
            PopValue(state);
    }
    state.ExecuteSingle(Instruction(InstrCode::INSTR_POP_NSP));
    if (!obj.isNull)
        PopValue(state);
    state.PopCall();
}

// OPERATIONS
// Assignation et unary operations
void LiteScript::StateExecutor::I_OP_ASSIGN(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (v1 = v2);
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_UNARY_PLUS(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 1)
        return;
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = +v1;
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_UNARY_MINUS(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 1)
        return;
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = -v1;
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_PRE_INCR(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 1)
        return;
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = ++v1;
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_POST_INCR(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 1)
        return;
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = v1++;
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_PRE_DECR(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 1)
        return;
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = --v1;
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_POST_DECR(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 1)
        return;
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = v1--;
    state.op_lifo.push_back(result);
}

// Arithmetic operations
void LiteScript::StateExecutor::I_OP_ADD(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = v1 + v2;
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_SUB(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = v1 - v2;
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_MUL(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = v1 * v2;
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_DIV(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = v1 / v2;
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_MOD(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = v1 % v2;
    state.op_lifo.push_back(result);
}

// Comparison
void LiteScript::StateExecutor::I_OP_EQU(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (v1 == v2);
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_DIF(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (v1 != v2);
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_GREAT(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (v1 > v2);
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_LESS(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (v1 < v2);
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_GREAT_EQU(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (v1 >= v2);
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_LESS_EQU(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (v1 <= v2);
    state.op_lifo.push_back(result);
}

// Logical operation
void LiteScript::StateExecutor::I_OP_LOG_NOT(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 1)
        return;
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (!v1);
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_LOG_AND(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (v1 && v2);
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_LOG_OR(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (v1 || v2);
    state.op_lifo.push_back(result);
}

// Binary operations
void LiteScript::StateExecutor::I_OP_BIT_NOT(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 1)
        return;
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (~v1);
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_BIT_AND(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (v1 & v2);
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_BIT_OR(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (v1 | v2);
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_BIT_XOR(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (v1 ^ v2);
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_LSHIFT(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (v1 << v2);
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_RSHIFT(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (v1 >> v2);
    state.op_lifo.push_back(result);
}

// Arithmetic and assignation
void LiteScript::StateExecutor::I_OP_ADD_ASSIGN(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (v1 += v2);
    state.op_lifo.push_back(result);}
void LiteScript::StateExecutor::I_OP_SUB_ASSIGN(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (v1 -= v2);
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_MUL_ASSIGN(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (v1 *= v2);
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_DIV_ASSIGN(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = (v1 /= v2);
    state.op_lifo.push_back(result);
}

// Special operations
void LiteScript::StateExecutor::I_OP_ARRAY(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = v1[v2];
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_MEMBER(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 1 || instr.comp_type != Instruction::CompType::COMP_TYPE_STRING)
        return;
    Variable v1(state.op_lifo.back());
    PopValue(state);
    Variable result = v1.GetMember(instr.comp_value.v_string);
    state.op_lifo.push_back(result);
}
void LiteScript::StateExecutor::I_OP_CALL(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 1)
        return;
    Variable v1(state.op_lifo.back());
    PopValue(state);
    state.op_lifo.push_back(v1(state.args.back()));
}

// CONTROL INSTRUCTIONS
void LiteScript::StateExecutor::I_JUMP_TO(State& state, Instruction& instr) {
    state.line_num++;
    if (instr.comp_type != Instruction::CompType::COMP_TYPE_INTEGER)
        return;
    state.line_num = (unsigned int)instr.comp_value.v_integer;
}
void LiteScript::StateExecutor::I_JUMP_IF(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 1 || instr.comp_type != Instruction::CompType::COMP_TYPE_INTEGER)
        return;
    Variable v1(state.op_lifo.back().Convert(Type::BOOLEAN));
    PopValue(state);
    if (v1->GetType() == Type::BOOLEAN && v1->GetData<bool>())
        state.line_num = (unsigned int) instr.comp_value.v_integer;
}
void LiteScript::StateExecutor::I_JUMP_ELSE(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 1 || instr.comp_type != Instruction::CompType::COMP_TYPE_INTEGER)
        return;
    Variable v1(state.op_lifo.back().Convert(Type::BOOLEAN));
    PopValue(state);
    if (v1->GetType() == Type::BOOLEAN && !v1->GetData<bool>())
        state.line_num = (unsigned int) instr.comp_value.v_integer;
}

// COMPLEX VALUES COMPLETION
// Array
void LiteScript::StateExecutor::I_ARRAY_PUSH_NUMERIC(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2 || instr.comp_type != Instruction::CompType::COMP_TYPE_INTEGER)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    if (v1->GetType() != Type::ARRAY)
        return;
    v1->GetData<Array>()[(unsigned int)instr.comp_value.v_integer] = v2;
}
void LiteScript::StateExecutor::I_ARRAY_PUSH_LITERAL(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2 || instr.comp_type != Instruction::CompType::COMP_TYPE_STRING)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    if (v1->GetType() != Type::ARRAY)
        return;
    v1->GetData<Array>()[instr.comp_value.v_string] = v2;
}

// Class
void LiteScript::StateExecutor::I_CLASS_PUSH_STATIC(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2 || instr.comp_type != Instruction::CompType::COMP_TYPE_STRING)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    if (v1->GetType() != Type::CLASS)
        return;
    v1->GetData<Class>().AddStatic(instr.comp_value.v_string, v2);
}
void LiteScript::StateExecutor::I_CLASS_PUSH_USTATIC(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2 || instr.comp_type != Instruction::CompType::COMP_TYPE_STRING)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    if (v1->GetType() != Type::CLASS)
        return;
    v1->GetData<Class>().AddUnstatic(instr.comp_value.v_string, v2);
}
void LiteScript::StateExecutor::I_CLASS_PUSH_OPERATOR(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2 || instr.comp_type != Instruction::CompType::COMP_TYPE_INTEGER)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    if (v1->GetType() != Type::CLASS)
        return;
    v1->GetData<Class>().AddOperator((unsigned int)instr.comp_value.v_integer, v2);
}
void LiteScript::StateExecutor::I_CLASS_INHERIT(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 2)
        return;
    Variable v2(state.op_lifo.back());
    PopValue(state);
    Variable v1(state.op_lifo.back());
    if (v1->GetType() != Type::CLASS)
        return;
    v1->GetData<Class>().Inherit(v2);
}
void LiteScript::StateExecutor::I_CLASS_CONSTRUCTOR(State& state, Instruction& instr) {
    state.line_num++;
    if (state.op_lifo.size() < 1 || instr.comp_type != Instruction::CompType::COMP_TYPE_STRING)
        return;
    Variable v1(state.op_lifo.back());
    if (v1->GetType() != Type::CLASS)
        return;
    v1->GetData<Class>().DefineConstructor(instr.comp_value.v_string);
}

// NAMESPACES
void LiteScript::StateExecutor::I_NAMESPACE_USE(State& state, Instruction& instr) {
    state.line_num++;
    if (instr.comp_type != Instruction::CompType::COMP_TYPE_STRING)
        return;
    state.UseNamespace(instr.comp_value.v_string);
}