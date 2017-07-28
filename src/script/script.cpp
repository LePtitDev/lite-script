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

#include "script.hpp"

std::array<const char *, LiteScript::Script::ErrorType::SCRPT_ERROR_NUMBER> LiteScript::Script::ErrorMsg({
    // SCRPT_ERROR_NO
    "No error",

    // SCRPT_ERROR_SIMPLEARRAY_END
    "Closing bracket expected for array",
    // SCRPT_ERROR_NAMEDARRAY_COLON
    "Colon character expected after array member name",
    // SCRPT_ERROR_NAMEDARRAY_EXPRESSION
    "Expression expected after array member name",
    // SCRPT_ERROR_NAMEDARRAY_END
    "Closing brace expected for array closing",

    // SCRPT_ERROR_CALLBACK_ARGUMENTS
    "Arguments expected for callback definition",
    // SCRPT_ERROR_CALLBACK_ARGUMENT_NAME
    "Variable name expected for arguments",
    // SCRPT_ERROR_CALLBACK_ARGUMENT_END
    "Closing parenthesis expected for arguments",
    // SCRPT_ERROR_CALLBACK_INSTRUCTIONS
    "Instructions expected for callback definition",

    // SCRPT_ERROR_CLASS_INHERIT
    "Class to inherit expected for class definition",
    // SCRPT_ERROR_CLASS_BEGIN
    "Opening brace expected for class definition",
    // SCRPT_ERROR_CLASS_INVALID
    "Invalid member inside class definition",
    // SCRPT_ERROR_CLASS_OPERATOR_NAME
    "Operator name expected for class definition",
    // SCRPT_ERROR_CLASS_OPERATOR_INVALID
    "Invalid operator name for class definition",
    // SCRPT_ERROR_CLASS_END
    "Closing brace expected for class definition"
});

LiteScript::Script::Script() :
        _mem_a(true), memory(*(LiteScript::Memory *)&_mem_d), state(*(LiteScript::State *)&_sta_d),
        error(Script::ErrorType::SCRPT_ERROR_NO), line_error(0)
{
    std::allocator<Memory> mem_allocator;
    mem_allocator.construct(&this->memory);
    std::allocator<State> sta_allocator;
    sta_allocator.construct(&this->state, this->memory);
}

LiteScript::Script::Script(Memory &mem) :
        _mem_a(false), memory(mem), state(*(LiteScript::State *)&_sta_d),
        error(Script::ErrorType::SCRPT_ERROR_NO), line_error(0)
{
    std::allocator<State> sta_allocator;
    sta_allocator.construct(&this->state, this->memory);
}

LiteScript::Script::~Script() {
    std::allocator<State> sta_allocator;
    sta_allocator.destroy(&this->state);
    if (this->_mem_a) {
        std::allocator<Memory> mem_allocator;
        mem_allocator.destroy(&this->memory);
    }
}

LiteScript::Variable LiteScript::Script::Execute(const char *code) {
    this->error = Script::ErrorType::SCRPT_ERROR_NO;
    std::vector<Instruction> list = Script::GetInstructionList(code, this->error, this->line_error);
    if (this->error != Script::ErrorType::SCRPT_ERROR_NO)
        return this->memory.Create(Type::UNDEFINED);
    else {
        this->state.AddInstructions(list);
        return this->state.Execute();
    }
}

const char* LiteScript::Script::GetError() const {
    return Script::GetError(this->error);
}

std::vector<LiteScript::Instruction> LiteScript::Script::GetInstructionList(const char *code) {
    Script::ErrorType errorType;
    unsigned int line_err;
    return Script::GetInstructionList(code, errorType, line_err);
}

std::vector<LiteScript::Instruction> LiteScript::Script::GetInstructionList(const char *code, Script::ErrorType &err,
                                                                              unsigned int &line_err) {
    std::vector<Instruction> list;
    ////// A COMPLETER //////
    return list;
}

const char* LiteScript::Script::GetError(Script::ErrorType type) {
    return Script::ErrorMsg[type];
}