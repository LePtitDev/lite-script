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

#include "instruction.hpp"

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