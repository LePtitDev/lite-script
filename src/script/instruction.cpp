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

#include <cstring>

#include "instruction.hpp"
#include "assembly.hpp"

#include "../streamer.hpp"

LiteScript::Instruction::Instruction() {}
LiteScript::Instruction::Instruction(InstrCode code) :
    code(code), comp_type(CompType::COMP_TYPE_NONE) {}
LiteScript::Instruction::Instruction(InstrCode code, bool val) :
    code(code), comp_type(CompType::COMP_TYPE_BOOLEAN) { this->comp_value.v_boolean = val; }
LiteScript::Instruction::Instruction(InstrCode code, int val) :
    code(code), comp_type(CompType::COMP_TYPE_INTEGER) { this->comp_value.v_integer = val; }
LiteScript::Instruction::Instruction(InstrCode code, float val) :
    code(code), comp_type(CompType::COMP_TYPE_FLOAT) { this->comp_value.v_float = val; }
LiteScript::Instruction::Instruction(InstrCode code, const char * val) :
    code(code), comp_type(CompType::COMP_TYPE_STRING)
{
    unsigned int len = strlen(val) + 1;
    this->comp_value.v_string = new char[len];
    memcpy(this->comp_value.v_string, val, len);
}
LiteScript::Instruction::Instruction(const Instruction &i) :
    code(i.code), comp_type(i.comp_type)
{
    unsigned int len;
    switch (this->comp_type) {
        case CompType::COMP_TYPE_BOOLEAN:
            this->comp_value.v_boolean = i.comp_value.v_boolean;
            break;
        case CompType::COMP_TYPE_INTEGER:
            this->comp_value.v_integer = i.comp_value.v_integer;
            break;
        case CompType::COMP_TYPE_FLOAT:
            this->comp_value.v_float = i.comp_value.v_float;
            break;
        case CompType::COMP_TYPE_STRING:
            len = strlen(i.comp_value.v_string) + 1;
            this->comp_value.v_string = new char[len];
            memcpy(this->comp_value.v_string, i.comp_value.v_string, len);
            break;
        default:
            break;
    }
}

LiteScript::Instruction::~Instruction() {
    if (this->comp_type == CompType::COMP_TYPE_STRING)
        delete this->comp_value.v_string;
}

LiteScript::Instruction & LiteScript::Instruction::operator=(const Instruction &i) {
    if (this->comp_type == CompType::COMP_TYPE_STRING)
        delete this->comp_value.v_string;
    unsigned int len;
    this->code = i.code;
    this->comp_type = i.comp_type;
    switch (this->comp_type) {
        case CompType::COMP_TYPE_BOOLEAN:
            this->comp_value.v_boolean = i.comp_value.v_boolean;
            break;
        case CompType::COMP_TYPE_INTEGER:
            this->comp_value.v_integer = i.comp_value.v_integer;
            break;
        case CompType::COMP_TYPE_FLOAT:
            this->comp_value.v_float = i.comp_value.v_float;
            break;
        case CompType::COMP_TYPE_STRING:
            len = strlen(i.comp_value.v_string) + 1;
            this->comp_value.v_string = new char[len];
            memcpy(this->comp_value.v_string, i.comp_value.v_string, len);
            break;
        default:
            break;
    }
    return *this;
}

void LiteScript::Instruction::Save(std::ostream &stream, const std::vector<Instruction> &instr) {
    for (unsigned int i = 0, sz = instr.size(); i < sz; i++) {
        bool line_num = false;
        switch (instr[i].code) {
            // DEFINITIONS
            case InstrCode::INSTR_DEFINE_VARIABLE:
                stream << "define.variable";
                break;
            case InstrCode::INSTR_DEFINE_ARG:
                stream << "define.arg";
                break;
            case InstrCode::INSTR_DEFINE_RETURN:
                stream << "define.return";
                break;
            // VALUE CREATIONS
            case InstrCode::INSTR_VALUE_POP:
                stream << "value.pop";
                break;
            case InstrCode::INSTR_VALUE_COPY:
                stream << "value.copy";
                break;
            case InstrCode::INSTR_VALUE_ASSIGN:
                stream << "value.assign";
                break;
            case InstrCode::INSTR_VALUE_UNDEFINED:
                stream << "value.undefined";
                break;
            case InstrCode::INSTR_VALUE_NULL:
                stream << "value.null";
                break;
            case InstrCode::INSTR_VALUE_BOOLEAN:
                stream << "value.boolean";
                break;
            case InstrCode::INSTR_VALUE_NUMBER:
                stream << "value.number";
                break;
            case InstrCode::INSTR_VALUE_STRING:
                stream << "value.string";
                break;
            case InstrCode::INSTR_VALUE_CALLBACK:
                stream << "value.callback";
                line_num = true;
                break;
            case InstrCode::INSTR_VALUE_ARRAY:
                stream << "value.array";
                break;
            case InstrCode::INSTR_VALUE_CLASS:
                stream << "value.class";
                break;
            case InstrCode::INSTR_VALUE_OBJECT:
                stream << "value.object";
                break;
            case InstrCode::INSTR_VALUE_ARG:
                stream << "value.arg";
                break;
            case InstrCode::INSTR_VALUE_ARGS:
                stream << "value.args";
                break;
            case InstrCode::INSTR_VALUE_THIS:
                stream << "value.this";
                break;
            case InstrCode::INSTR_VALUE_VARIABLE:
                stream << "value.variable";
                break;
            // PILES MANAGEMENT
            case InstrCode::INSTR_PUSH_NSP:
                stream << "push.nsp";
                break;
            case InstrCode::INSTR_PUSH_ARGS:
                stream << "push.args";
                break;
            case InstrCode::INSTR_POP_NSP:
                stream << "pop.nsp";
                break;
            case InstrCode::INSTR_POP_ARGS:
                stream << "pop.args";
                break;
            case InstrCode::INSTR_RETURN:
                stream << "return";
                break;
            // OPERATIONS
            // Assignation et unary operations
            case InstrCode::INSTR_OP_ASSIGN:
                stream << "operator.assign";
                break;
            case InstrCode::INSTR_OP_UNARY_PLUS:
                stream << "operator.unary-plus";
                break;
            case InstrCode::INSTR_OP_UNARY_MINUS:
                stream << "operator.unary-minus";
                break;
            case InstrCode::INSTR_OP_PRE_INCR:
                stream << "operator.pre-incr";
                break;
            case InstrCode::INSTR_OP_POST_INCR:
                stream << "operator.post-incr";
                break;
            case InstrCode::INSTR_OP_PRE_DECR:
                stream << "operator.pre-decr";
                break;
            case InstrCode::INSTR_OP_POST_DECR:
                stream << "operator.post-decr";
                break;
            // Arithmetic operations
            case InstrCode::INSTR_OP_ADD:
                stream << "operator.add";
                break;
            case InstrCode::INSTR_OP_SUB:
                stream << "operator.sub";
                break;
            case InstrCode::INSTR_OP_MUL:
                stream << "operator.mul";
                break;
            case InstrCode::INSTR_OP_DIV:
                stream << "operator.div";
                break;
            case InstrCode::INSTR_OP_MOD:
                stream << "operator.mod";
                break;
            // Comparison
            case InstrCode::INSTR_OP_EQU:
                stream << "operator.equ";
                break;
            case InstrCode::INSTR_OP_DIF:
                stream << "operator.dif";
                break;
            case InstrCode::INSTR_OP_GREAT:
                stream << "operator.great";
                break;
            case InstrCode::INSTR_OP_LESS:
                stream << "operator.less";
                break;
            case InstrCode::INSTR_OP_GREAT_EQU:
                stream << "operator.great-equ";
                break;
            case InstrCode::INSTR_OP_LESS_EQU:
                stream << "operator.less-equ";
                break;
            // Logical operation
            case InstrCode::INSTR_OP_LOG_NOT:
                stream << "operator.log-not";
                break;
            case InstrCode::INSTR_OP_LOG_AND:
                stream << "operator.log-and";
                break;
            case InstrCode::INSTR_OP_LOG_OR:
                stream << "operator.log-or";
                break;
            // Binary operations
            case InstrCode::INSTR_OP_BIT_NOT:
                stream << "operator.bit-not";
                break;
            case InstrCode::INSTR_OP_BIT_AND:
                stream << "operator.bit-and";
                break;
            case InstrCode::INSTR_OP_BIT_OR:
                stream << "operator.bit-or";
                break;
            case InstrCode::INSTR_OP_BIT_XOR:
                stream << "operator.bit-xor";
                break;
            case InstrCode::INSTR_OP_LSHIFT:
                stream << "operator.lshift";
                break;
            case InstrCode::INSTR_OP_RSHIFT:
                stream << "operator.rshift";
                break;
            // Arithmetic and assignation
            case InstrCode::INSTR_OP_ADD_ASSIGN:
                stream << "operator.add-assign";
                break;
            case InstrCode::INSTR_OP_SUB_ASSIGN:
                stream << "operator.sub-assign";
                break;
            case InstrCode::INSTR_OP_MUL_ASSIGN:
                stream << "operator.mul-assign";
                break;
            case InstrCode::INSTR_OP_DIV_ASSIGN:
                stream << "operator.div-assign";
                break;
            // Special operations
            case InstrCode::INSTR_OP_ARRAY:
                stream << "operator.array";
                break;
            case InstrCode::INSTR_OP_MEMBER:
                stream << "operator.member";
                break;
            case InstrCode::INSTR_OP_CALL:
                stream << "operator.call";
                break;
            // CONTROL INSTRUCTIONS
            case InstrCode::INSTR_JUMP_TO:
                stream << "jump-to";
                line_num = true;
                break;
            case InstrCode::INSTR_JUMP_IF:
                stream << "jump-if";
                line_num = true;
                break;
            case InstrCode::INSTR_JUMP_ELSE:
                stream << "jump-else";
                line_num = true;
                break;
            // COMPLEX VALUES COMPLETION
            // Array
            case InstrCode::INSTR_ARRAY_PUSH_NUMERIC:
                stream << "array.push-numeric";
                break;
            case InstrCode::INSTR_ARRAY_PUSH_LITERAL:
                stream << "array.push-literal";
                break;
            // Class
            case InstrCode::INSTR_CLASS_PUSH_STATIC:
                stream << "class.push-static";
                break;
            case InstrCode::INSTR_CLASS_PUSH_USTATIC:
                stream << "class.push-unstatic";
                break;
            case InstrCode::INSTR_CLASS_PUSH_OPERATOR:
                stream << "class.push-operator";
                break;
            case InstrCode::INSTR_CLASS_INHERIT:
                stream << "class.inherit";
                break;
            case InstrCode::INSTR_CLASS_CONSTRUCTOR:
                stream << "class.constructor";
                break;
            // NAMESPACES
            case InstrCode::INSTR_NAMESPACE_USE:
                stream << "namespace";
                break;
            default:
                continue;
        }
        switch (instr[i].comp_type) {
            case Instruction::CompType::COMP_TYPE_BOOLEAN:
                stream << " " << (instr[i].comp_value.v_boolean ? "true" : "false");
                break;
            case Instruction::CompType::COMP_TYPE_INTEGER:
                if (line_num)
                    stream << " " << (instr[i].comp_value.v_integer + 1);
                else
                    stream << " " << instr[i].comp_value.v_integer;
                break;
            case Instruction::CompType::COMP_TYPE_FLOAT:
                stream << " " << instr[i].comp_value.v_float;
                break;
            case Instruction::CompType::COMP_TYPE_STRING:
                stream << " " << instr[i].comp_value.v_string;
                break;
            default:
                break;
        }
        stream << std::endl;
    }
}

std::vector<LiteScript::Instruction> LiteScript::Instruction::Load(std::istream &stream) {
    std::string code;
    while (!stream.eof())
        code += (char)stream.get();
    return Assembly::GetInstructionList(code.c_str());
}

void LiteScript::Instruction::SaveBinary(std::ostream &stream, const std::vector<Instruction> &instr) {
    OStreamer streamer(stream);
    for (unsigned int i = 0, sz = instr.size(); i < sz; i++) {
        stream << instr[i].code;
        switch (instr[i].comp_type) {
            case CompType::COMP_TYPE_BOOLEAN:
                streamer << (unsigned char)1 << (unsigned char)instr[i].comp_value.v_boolean;
                break;
            case CompType::COMP_TYPE_INTEGER:
                streamer << (unsigned char)2 << instr[i].comp_value.v_integer;
                break;
            case CompType::COMP_TYPE_FLOAT:
                streamer << (unsigned char)3 << (*(unsigned int *)&instr[i].comp_value.v_float);
                break;
            case CompType::COMP_TYPE_STRING:
                stream << (unsigned char)4 << instr[i].comp_value.v_string;
            default:
                stream << (unsigned char)0;
        }
    }
}

std::vector<LiteScript::Instruction> LiteScript::Instruction::LoadBinary(std::istream &stream) {
    std::vector<Instruction> res;
    IStreamer streamer(stream);
    std::string s_tmp;
    unsigned char c_tmp;
    while (!stream.eof()) {
        Instruction instr;
        stream >> instr.code;
        stream >> instr.comp_type;
        switch (instr.comp_type) {
            case CompType::COMP_TYPE_BOOLEAN:
                stream >> instr.comp_value.v_boolean;
                break;
            case CompType::COMP_TYPE_INTEGER:
                streamer >> instr.comp_value.v_integer;
                break;
            case CompType::COMP_TYPE_FLOAT:
                streamer >> (*(unsigned int *)&instr.comp_value.v_float);
                break;
            case CompType::COMP_TYPE_STRING:
                s_tmp.clear();
                while (!stream.eof() && (c_tmp = (unsigned char)stream.get()) != 0)
                    s_tmp += c_tmp;
                instr.comp_value.v_string = new char[s_tmp.size() + 1];
                memcpy(instr.comp_value.v_string, s_tmp.c_str(), s_tmp.size() + 1);
                break;
            default:
                instr.comp_type = 0;
        }
        res.push_back(instr);
    }
    return res;
}