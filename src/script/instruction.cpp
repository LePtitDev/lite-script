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

LiteScript::Instruction::~Instruction() {
    if (this->comp_type == CompType::COMP_TYPE_STRING)
        delete this->comp_value.v_string;
}

void LiteScript::Instruction::Save(std::ostream &stream) {
    stream << this->code;
    switch (this->comp_type) {
        case CompType::COMP_TYPE_NONE:
            stream << (unsigned char)0x00;
            break;
        case CompType::COMP_TYPE_BOOLEAN:
            stream << (unsigned char)0x01 << (this->comp_value.v_boolean ? "true" : "false") << (unsigned char)0x00;
            break;
        case CompType::COMP_TYPE_INTEGER:
            stream << (unsigned char)0x02 << this->comp_value.v_integer << (unsigned char)0x00;
            break;
        case CompType::COMP_TYPE_FLOAT:
            stream << (unsigned char)0x02 << this->comp_value.v_float << (unsigned char)0x00;
            break;
        case CompType::COMP_TYPE_STRING:
            stream << (unsigned char)0x02 << this->comp_value.v_string << (unsigned char)0x00;
            break;
        default:
            stream << (unsigned char)0x00;
    }
}

void LiteScript::Instruction::Save(std::ostream &stream, Instruction instr) {
    instr.Save(stream);
}

LiteScript::Instruction LiteScript::Instruction::Load(std::istream &stream) {
    Instruction result;
    result.code = (unsigned char)stream.get();
    int type = stream.get();
    if (type == 0) {
        result.comp_type = CompType::COMP_TYPE_NONE;
        return result;
    }
    std::string str;
    char c;
    while (!stream.eof()) {
        c = (char)stream.get();
        if (c == 0x00)
            break;
        str += c;
    }
    switch (type) {
        case 1:
            result.comp_type = CompType::COMP_TYPE_BOOLEAN;
            result.comp_value.v_boolean = (str == "true");
            break;
        case 2:
            result.comp_type = CompType::COMP_TYPE_INTEGER;
            result.comp_value.v_integer = atoi(str.c_str());
            break;
        case 3:
            result.comp_type = CompType::COMP_TYPE_FLOAT;
            result.comp_value.v_float = (float)atof(str.c_str());
            break;
        case 4:
            result.comp_type = CompType::COMP_TYPE_STRING;
            result.comp_value.v_string = new char[str.size() + 1];
            memcpy(result.comp_value.v_string, str.c_str(), str.size());
            break;
        default:
            result.comp_type = CompType::COMP_TYPE_NONE;
            break;
    }
    return result;
}