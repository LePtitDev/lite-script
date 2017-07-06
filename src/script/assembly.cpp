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

#include "assembly.hpp"
#include "syntax.hpp"

std::array<const char *, LiteScript::Assembly::ErrorType::ASSM_ERROR_NUMBER> LiteScript::Assembly::ErrorMsg({
    "No error",
    "Unknow assembly command",
    "Boolean argument expected",
    "Unsigned integer argument expected",
    "Number argument expected",
    "String argument expected"
});

LiteScript::Assembly::Assembly() :
    _mem_a(true), memory(*(LiteScript::Memory *)&_mem_d), state(*(LiteScript::State *)&_sta_d),
    error(Assembly::ErrorType::ASSM_ERROR_NO), line_error(0)
{
    std::allocator<Memory> mem_allocator;
    mem_allocator.construct(&this->memory);
    std::allocator<State> sta_allocator;
    sta_allocator.construct(&this->state, this->memory);
}

LiteScript::Assembly::Assembly(Memory &mem) :
    _mem_a(false), memory(mem), state(*(LiteScript::State *)&_sta_d),
    error(Assembly::ErrorType::ASSM_ERROR_NO), line_error(0)
{
    std::allocator<State> sta_allocator;
    sta_allocator.construct(&this->state, this->memory);
}

LiteScript::Assembly::~Assembly() {
    std::allocator<State> sta_allocator;
    sta_allocator.destroy(&this->state);
    if (this->_mem_a) {
        std::allocator<Memory> mem_allocator;
        mem_allocator.destroy(&this->memory);
    }
}

LiteScript::Variable LiteScript::Assembly::Execute(const char *code) {
    this->error = Assembly::ErrorType::ASSM_ERROR_NO;
    std::vector<Instruction> list = Assembly::GetInstructionList(code, this->error, this->line_error);
    if (this->error != Assembly::ErrorType::ASSM_ERROR_NO)
        return this->memory.Create(Type::UNDEFINED);
    else {
        this->state.AddInstructions(list);
        return this->state.Execute();
    }
}

LiteScript::Variable LiteScript::Assembly::ExecuteSingle(const char *code) {
    this->error = Assembly::ErrorType::ASSM_ERROR_NO;
    Instruction in = Assembly::GetInstructionSingle(code, this->error);
    if (this->error != Assembly::ErrorType::ASSM_ERROR_NO)
        return this->memory.Create(Type::UNDEFINED);
    else {
        this->state.AddInstruction(in);
        return this->state.Execute();
    }
}

const char* LiteScript::Assembly::GetError() const {
    return Assembly::GetError(this->error);
}

LiteScript::Instruction LiteScript::Assembly::GetInstructionSingle(const char *code) {
    Assembly::ErrorType errorType;
    return Assembly::GetInstructionSingle(code, errorType);
}

LiteScript::Instruction LiteScript::Assembly::GetInstructionSingle(const char *code, Assembly::ErrorType &err) {
    union {
        bool b;
        int i;
        unsigned int ui;
        float f;
    } tmp;
    if (code[0] == 'd') {
        //define.
        if (strncmp(code, "define.", 7)) {
            err = Assembly::ErrorType::ASSM_ERROR_UNKNOW_CMD;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //define.variable [string]
        if (strncmp(code + 7, "variable", 8) == 0) {
            if (code[15] == ' ' && code[16] != '\0')
                return Instruction(InstrCode::INSTR_DEFINE_VARIABLE, code + 16);
            err = Assembly::ErrorType::ASSM_ERROR_EXPECTED_STRING;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //define.arg [int]
        if (strncmp(code + 7, "arg", 3) == 0) {
            if (code[10] == ' ' && Syntax::ReadUInteger(code + 11, tmp.ui) > 0)
                return Instruction(InstrCode::INSTR_DEFINE_ARG, (int)tmp.ui);
            err = Assembly::ErrorType::ASSM_ERROR_EXPECTED_INTEGER;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //define.return
        if (strncmp(code + 7, "return", 7) == 0)
            return Instruction(InstrCode::INSTR_DEFINE_RETURN);
        err = Assembly::ErrorType::ASSM_ERROR_UNKNOW_CMD;
        return Instruction(InstrCode::INSTR_INVALID);
    }
    else if (code[0] == 'v') {
        //value.
        if (strncmp(code, "value.", 6)) {
            err = Assembly::ErrorType::ASSM_ERROR_UNKNOW_CMD;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //value.pop
        if (strncmp(code + 6, "pop", 4) == 0)
            return Instruction(InstrCode::INSTR_VALUE_POP);
        //value.undefined
        if (strncmp(code + 6, "undefined", 10) == 0)
            return Instruction(InstrCode::INSTR_VALUE_UNDEFINED);
        //value.null
        if (strncmp(code + 6, "null", 5) == 0)
            return Instruction(InstrCode::INSTR_VALUE_NULL);
        //value.boolean [bool]
        if (strncmp(code + 6, "boolean", 7) == 0) {
            if (code[13] == ' ' && Syntax::ReadBoolean(code + 14, tmp.b) > 0)
                return Instruction(InstrCode::INSTR_VALUE_BOOLEAN, tmp.b);
            err = Assembly::ErrorType::ASSM_ERROR_EXPECTED_BOOLEAN;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //value.number [float]
        if (strncmp(code + 6, "number", 6) == 0) {
            if (code[12] == ' ' && Syntax::ReadNumber(code + 13, tmp.f) > 0)
                return Instruction(InstrCode::INSTR_VALUE_NUMBER, tmp.f);
            err = Assembly::ErrorType::ASSM_ERROR_EXPECTED_NUMBER;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //value.string [string]
        if (strncmp(code + 6, "string", 6) == 0) {
            if (code[12] == ' ' && code[13] != '\0')
                return Instruction(InstrCode::INSTR_VALUE_STRING, code + 13);
            err = Assembly::ErrorType::ASSM_ERROR_EXPECTED_STRING;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //value.array
        if (strncmp(code + 6, "array", 6) == 0)
            return Instruction(InstrCode::INSTR_VALUE_ARRAY);
        //value.class
        if (strncmp(code + 6, "class", 5) == 0)
            return Instruction(InstrCode::INSTR_VALUE_CLASS);
        //value.callback [int]
        if (strncmp(code + 6, "callback", 8) == 0) {
            if (code[14] == ' ' && Syntax::ReadUInteger(code + 15, tmp.ui) > 0)
                return Instruction(InstrCode::INSTR_VALUE_CALLBACK, (int)tmp.ui);
            err = Assembly::ErrorType::ASSM_ERROR_EXPECTED_INTEGER;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //value.args [int]
        if (strncmp(code + 6, "args", 4) == 0) {
            if (code[10] == ' ' && Syntax::ReadUInteger(code + 11, tmp.ui) > 0)
                return Instruction(InstrCode::INSTR_VALUE_ARGS, (int)tmp.ui);
            err = Assembly::ErrorType::ASSM_ERROR_EXPECTED_INTEGER;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //value.variable [string]
        if (strncmp(code + 6, "variable", 8) == 0) {
            if (code[14] == ' ' && code[15] != '\0')
                return Instruction(InstrCode::INSTR_VALUE_VARIABLE, code + 15);
            err = Assembly::ErrorType::ASSM_ERROR_EXPECTED_STRING;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //value.this
        if (strncmp(code + 6, "this", 5) == 0)
            return Instruction(InstrCode::INSTR_VALUE_THIS);
        err = Assembly::ErrorType::ASSM_ERROR_UNKNOW_CMD;
        return Instruction(InstrCode::INSTR_INVALID);
    }
    else if (code[0] == 'p') {
        //push.
        if (strncmp(code, "push.", 5) == 0) {
            //push.nsp
            if (strncmp(code + 5, "nsp", 4) == 0)
                return Instruction(InstrCode::INSTR_PUSH_NSP);
            //push.args
            if (strncmp(code + 5, "args", 5) == 0)
                return Instruction(InstrCode::INSTR_PUSH_ARGS);
            err = Assembly::ErrorType::ASSM_ERROR_UNKNOW_CMD;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        if (strncmp(code, "pop.", 4) == 0) {
            //pop.nsp
            if (strncmp(code + 4, "nsp", 4) == 0)
                return Instruction(InstrCode::INSTR_POP_NSP);
            //pop.args
            if (strncmp(code + 4, "args", 5) == 0)
                return Instruction(InstrCode::INSTR_POP_ARGS);
            err = Assembly::ErrorType::ASSM_ERROR_UNKNOW_CMD;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        err = Assembly::ErrorType::ASSM_ERROR_UNKNOW_CMD;
        return Instruction(InstrCode::INSTR_INVALID);
    }
    else if (code[0] == 'r') {
        //return
        if (strncmp(code, "return", 7) == 0)
            return Instruction(InstrCode::INSTR_RETURN);
        err = Assembly::ErrorType::ASSM_ERROR_UNKNOW_CMD;
        return Instruction(InstrCode::INSTR_INVALID);
    }
    else if (code[0] == 'o') {
        //operator.
        if (strncmp(code, "operator.", 9)) {
            err = Assembly::ErrorType::ASSM_ERROR_UNKNOW_CMD;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //operator.assign
        if (strncmp(code + 9, "assign", 7) == 0)
            return Instruction(InstrCode::INSTR_OP_ASSIGN);
        //operator.unary-plus
        if (strncmp(code + 9, "unary-plus", 11) == 0)
            return Instruction(InstrCode::INSTR_OP_UNARY_PLUS);
        //operator.unary-minus
        if (strncmp(code + 9, "unary-minus", 12) == 0)
            return Instruction(InstrCode::INSTR_OP_UNARY_MINUS);
        //operator.pre-incr
        if (strncmp(code + 9, "pre-incr", 9) == 0)
            return Instruction(InstrCode::INSTR_OP_PRE_INCR);
        //operator.pre-decr
        if (strncmp(code + 9, "pre-decr", 9) == 0)
            return Instruction(InstrCode::INSTR_OP_PRE_DECR);
        //operator.post-incr
        if (strncmp(code + 9, "post-incr", 10) == 0)
            return Instruction(InstrCode::INSTR_OP_POST_INCR);
        //operator.post-decr
        if (strncmp(code + 9, "post-decr", 10) == 0)
            return Instruction(InstrCode::INSTR_OP_POST_DECR);
        //operator.add-assign
        if (strncmp(code + 9, "add-assign", 11) == 0)
            return Instruction(InstrCode::INSTR_OP_ADD_ASSIGN);
        //operator.sub-assign
        if (strncmp(code + 9, "sub-assign", 11) == 0)
            return Instruction(InstrCode::INSTR_OP_SUB_ASSIGN);
        //operator.mul-assign
        if (strncmp(code + 9, "mul-assign", 11) == 0)
            return Instruction(InstrCode::INSTR_OP_MUL_ASSIGN);
        //operator.div-assign
        if (strncmp(code + 9, "div-assign", 11) == 0)
            return Instruction(InstrCode::INSTR_OP_DIV_ASSIGN);
        //operator.add
        if (strncmp(code + 9, "add", 4) == 0)
            return Instruction(InstrCode::INSTR_OP_ADD);
        //operator.sub
        if (strncmp(code + 9, "sub", 4) == 0)
            return Instruction(InstrCode::INSTR_OP_SUB);
        //operator.mul
        if (strncmp(code + 9, "mul", 4) == 0)
            return Instruction(InstrCode::INSTR_OP_MUL);
        //operator.div
        if (strncmp(code + 9, "div", 4) == 0)
            return Instruction(InstrCode::INSTR_OP_DIV);
        //operator.mod
        if (strncmp(code + 9, "mod", 4) == 0)
            return Instruction(InstrCode::INSTR_OP_MOD);
        //operator.equ
        if (strncmp(code + 9, "equ", 4) == 0)
            return Instruction(InstrCode::INSTR_OP_EQU);
        //operator.dif
        if (strncmp(code + 9, "dif", 4) == 0)
            return Instruction(InstrCode::INSTR_OP_DIF);
        //operator.great-equ
        if (strncmp(code + 9, "great-equ", 10) == 0)
            return Instruction(InstrCode::INSTR_OP_GREAT_EQU);
        //operator.great
        if (strncmp(code + 9, "great", 6) == 0)
            return Instruction(InstrCode::INSTR_OP_GREAT);
        //operator.less-equ
        if (strncmp(code + 9, "less-equ", 9) == 0)
            return Instruction(InstrCode::INSTR_OP_LESS_EQU);
        //operator.less
        if (strncmp(code + 9, "less", 5) == 0)
            return Instruction(InstrCode::INSTR_OP_LESS);
        //operator.log-not
        if (strncmp(code + 9, "log-not", 8) == 0)
            return Instruction(InstrCode::INSTR_OP_LOG_NOT);
        //operator.log-and
        if (strncmp(code + 9, "log-and", 8) == 0)
            return Instruction(InstrCode::INSTR_OP_LOG_AND);
        //operator.log-or
        if (strncmp(code + 9, "log-or", 7) == 0)
            return Instruction(InstrCode::INSTR_OP_LOG_OR);
        //operator.bit-not
        if (strncmp(code + 9, "bit-not", 8) == 0)
            return Instruction(InstrCode::INSTR_OP_BIT_NOT);
        //operator.bit-and
        if (strncmp(code + 9, "bit-and", 8) == 0)
            return Instruction(InstrCode::INSTR_OP_BIT_AND);
        //operator.bit-or
        if (strncmp(code + 9, "bit-or", 7) == 0)
            return Instruction(InstrCode::INSTR_OP_BIT_OR);
        //operator.bit-xor
        if (strncmp(code + 9, "bit-xor", 8) == 0)
            return Instruction(InstrCode::INSTR_OP_BIT_XOR);
        //operator.lshift
        if (strncmp(code + 9, "lshift", 7) == 0)
            return Instruction(InstrCode::INSTR_OP_LSHIFT);
        //operator.rshift
        if (strncmp(code + 9, "rshift", 7) == 0)
            return Instruction(InstrCode::INSTR_OP_RSHIFT);
        //operator.array
        if (strncmp(code + 9, "array", 6) == 0)
            return Instruction(InstrCode::INSTR_OP_ARRAY);
        //operator.member [string]
        if (strncmp(code + 9, "member", 6) == 0) {
            if (code[15] == ' ' && code[16] != '\0')
                return Instruction(InstrCode::INSTR_OP_MEMBER, code + 16);
            err = Assembly::ErrorType::ASSM_ERROR_EXPECTED_STRING;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //operator.call
        if (strncmp(code + 9, "call", 5) == 0)
            return Instruction(InstrCode::INSTR_OP_CALL);
        err = Assembly::ErrorType::ASSM_ERROR_UNKNOW_CMD;
        return Instruction(InstrCode::INSTR_INVALID);
    }
    else if (code[0] == 'j') {
        //jump-
        if (strncmp(code, "jump-", 5)) {
            err = Assembly::ErrorType::ASSM_ERROR_UNKNOW_CMD;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //jump-to [int]
        if (strncmp(code + 5, "to", 2) == 0) {
            if (code[7] == ' ' && Syntax::ReadUInteger(code + 8, tmp.ui) > 0)
                return Instruction(InstrCode::INSTR_JUMP_TO, (int)tmp.ui);
            err = Assembly::ErrorType::ASSM_ERROR_EXPECTED_INTEGER;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //jump-if [int]
        if (strncmp(code + 5, "if", 2) == 0) {
            if (code[7] == ' ' && Syntax::ReadUInteger(code + 8, tmp.ui) > 0)
                return Instruction(InstrCode::INSTR_JUMP_IF, (int)tmp.ui);
            err = Assembly::ErrorType::ASSM_ERROR_EXPECTED_INTEGER;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //jump-else [int]
        if (strncmp(code + 5, "else", 4) == 0) {
            if (code[9] == ' ' && Syntax::ReadUInteger(code + 10, tmp.ui) > 0)
                return Instruction(InstrCode::INSTR_JUMP_ELSE, (int)tmp.ui);
            err = Assembly::ErrorType::ASSM_ERROR_EXPECTED_INTEGER;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        err = Assembly::ErrorType::ASSM_ERROR_UNKNOW_CMD;
        return Instruction(InstrCode::INSTR_INVALID);
    }
    else if (code[0] == 'a') {
        //array.push-
        if (strncmp(code, "array.push-", 11)) {
            err = Assembly::ErrorType::ASSM_ERROR_UNKNOW_CMD;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //array.push-numeric [int]
        if (strncmp(code + 11, "numeric", 7) == 0) {
            if (code[18] == ' ' && Syntax::ReadUInteger(code + 19, tmp.ui) > 0)
                return Instruction(InstrCode::INSTR_ARRAY_PUSH_NUMERIC, (int)tmp.ui);
            err = Assembly::ErrorType::ASSM_ERROR_EXPECTED_INTEGER;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //array.push-literal [string]
        if (strncmp(code + 11, "literal", 7) == 0) {
            if (code[18] == ' ' && code[19] != '\0')
                return Instruction(InstrCode::INSTR_ARRAY_PUSH_LITERAL, code + 19);
            err = Assembly::ErrorType::ASSM_ERROR_EXPECTED_STRING;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        err = Assembly::ErrorType::ASSM_ERROR_UNKNOW_CMD;
        return Instruction(InstrCode::INSTR_INVALID);
    }
    else if (code[0] == 'c') {
        //class.
        if (strncmp(code, "class.", 6)) {
            err = Assembly::ErrorType::ASSM_ERROR_UNKNOW_CMD;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //class.push-static [string]
        if (strncmp(code + 6, "push-static", 11) == 0) {
            if (code[17] == ' ' && code[18] != '\0')
                return Instruction(InstrCode::INSTR_CLASS_PUSH_STATIC, code + 18);
            err = Assembly::ErrorType::ASSM_ERROR_EXPECTED_STRING;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //class.push-unstatic [string]
        if (strncmp(code + 6, "push-unstatic", 13) == 0) {
            if (code[19] == ' ' && code[20] != '\0')
                return Instruction(InstrCode::INSTR_CLASS_PUSH_USTATIC, code + 20);
            err = Assembly::ErrorType::ASSM_ERROR_EXPECTED_STRING;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //class.push-operator [int]
        if (strncmp(code + 6, "push-operator", 13) == 0) {
            if (code[19] == ' ' && Syntax::ReadUInteger(code + 20, tmp.ui) > 0)
                return Instruction(InstrCode::INSTR_CLASS_PUSH_OPERATOR, (int)tmp.ui);
            err = Assembly::ErrorType::ASSM_ERROR_EXPECTED_INTEGER;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        //class.inherit
        if (strncmp(code + 6, "inherit", 8) == 0)
            return Instruction(InstrCode::INSTR_CLASS_INHERIT);
        err = Assembly::ErrorType::ASSM_ERROR_NO;
        return Instruction(InstrCode::INSTR_INVALID);
    }
    else if (code[0] == 'n') {
        //namespace [string]
        if (strncmp(code, "namespace", 9) == 0) {
            if (code[9] == ' ' && code[10] != '\0')
                return Instruction(InstrCode::INSTR_NAMESPACE_USE, code + 10);
            err = Assembly::ErrorType::ASSM_ERROR_EXPECTED_STRING;
            return Instruction(InstrCode::INSTR_INVALID);
        }
        err = Assembly::ErrorType::ASSM_ERROR_NO;
        return Instruction(InstrCode::INSTR_INVALID);
    }

    err = Assembly::ErrorType::ASSM_ERROR_UNKNOW_CMD;
    return Instruction(InstrCode::INSTR_INVALID);
}

std::vector<LiteScript::Instruction> LiteScript::Assembly::GetInstructionList(const char *code) {
    Assembly::ErrorType errorType;
    unsigned int line_err;
    return Assembly::GetInstructionList(code, errorType, line_err);
}

std::vector<LiteScript::Instruction> LiteScript::Assembly::GetInstructionList(const char *code, Assembly::ErrorType &err,
                                                                  unsigned int &line_err) {
    std::vector<Instruction> list;
    std::string line;
    unsigned int line_num = 0;
    for (unsigned int i = 0; code[i] != '\0'; i++) {
        if (code[i] == '\n') {
            line_num++;
            list.push_back(Assembly::GetInstructionSingle(line.c_str(), err));
            if (err != Assembly::ErrorType::ASSM_ERROR_NO) {
                list.clear();
                line_err = line_num;
                return list;
            }
            line.clear();
        }
        else {
            line += code[i];
        }
    }
    return list;
}

const char* LiteScript::Assembly::GetError(Assembly::ErrorType type) {
    return Assembly::ErrorMsg[type];
}