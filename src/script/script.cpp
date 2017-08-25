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

#include "../types/internal.hpp"
#include "script.hpp"
#include "syntax.hpp"

std::array<const char *, LiteScript::Script::ErrorType::SCRPT_ERROR_NUMBER> LiteScript::Script::ErrorMsg({
    // SCRPT_ERROR_NO
    "No error",

    // SCRPT_ERROR_SIMPLEARRAY_END
    "Closing bracket \"]\" expected for array",
    // SCRPT_ERROR_NAMEDARRAY_COLON
    "Colon \":\" character expected after array member name",
    // SCRPT_ERROR_NAMEDARRAY_EXPRESSION
    "Expression expected after array member name",
    // SCRPT_ERROR_NAMEDARRAY_END
	"Closing brace \"}\" expected for array closing",

    // SCRPT_ERROR_CALLBACK_ARGUMENTS
    "Arguments expected for callback definition",
    // SCRPT_ERROR_CALLBACK_ARGUMENT_NAME
    "Variable name expected for arguments",
    // SCRPT_ERROR_CALLBACK_ARGUMENT_END
    "Closing parenthesis \")\" expected for arguments",
    // SCRPT_ERROR_CALLBACK_INSTRUCTIONS
    "Instructions expected for callback definition",

    // SCRPT_ERROR_CLASS_INHERIT
    "Class to inherit expected for class definition",
    // SCRPT_ERROR_CLASS_BEGIN
    "Opening brace \"{\" expected for class definition",
    // SCRPT_ERROR_CLASS_INVALID
    "Invalid member inside class definition",
    // SCRPT_ERROR_CLASS_OPERATOR_NAME
    "Operator name expected for class definition",
    // SCRPT_ERROR_CLASS_OPERATOR_INVALID
    "Invalid operator name for class definition",
    // SCRPT_ERROR_CLASS_END
    "Closing brace \"}\" expected for class definition",

    // SCRPT_ERROR_FOR_INITIALISATION
    "Initialisation expected in for control statement",
    // SCRPT_ERROR_FOR_CONDITION
    "Condition expected in for control statement",
    // SCRPT_ERROR_SWITCH_KEYWORDS
    "\"case\" or \"default\" keywords expected in switch control statement",
    // SCRPT_ERROR_SWITCH_END
    "Closing brace \"}\" expected in switch control statement",

    // SCRPT_ERROR_NAME
    "Variable name expected",
    // SCRPT_ERROR_VALUE
    "Value expected",
    // SCRPT_ERROR_OPERATOR
    "Invalid operator",
    // SCRPT_ERROR_EXPRESSION
    "Expression expected",
    // SCRPT_ERROR_PARENTHESIS_OPEN
    "Opening parenthesis \"(\" expected",
    // SCRPT_ERROR_PARENTHESIS_CLOSE
    "Closing parenthesis \")\" expected",
    // SCRPT_ERROR_BRACKET_CLOSE
    "Closing bracket \"]\" expected",
    // SCRPT_ERROR_BRACE_OPEN
    "Opening brace \"{\" expected",
    // SCRPT_ERROR_BRACE_CLOSE
    "Closing brace \"}\" expected",
    // SCRPT_ERROR_COLON
    "Colon \":\" expected",
    // SCRPT_ERROR_SEMICOLON
    "Semi-colon \";\" expected",
    // SCRPT_ERROR_INSTRUCTION
    "Instruction expected",
    // SCRPT_ERROR_RETURN
    "Expression or semi-colon expected for return",
    // SCRPT_ERROR_WHILE
    "\"while\" keyword expected in do..while control statement",
    // SCRPT_ERROR_NAMESPACE
    "Invalid namespace",

    // SCRPT_ERROR_UNKNOW
    "Unknow element"
});

LiteScript::Script::Script(State &state) :
        memory(state.memory), state(state),
        error(Script::ErrorType::SCRPT_ERROR_NO), line_error(0), col_error(0) {}

LiteScript::Script::Script(const Script &sc) :
        memory(sc.memory), state(sc.state),
        error(sc.error), line_error(sc.line_error), col_error(sc.col_error) {}

void LiteScript::Script::Execute(const char *code) {
    this->error = Script::ErrorType::SCRPT_ERROR_NO;
    std::vector<Instruction> list = Script::GetInstructionList(code, this->error, this->line_error, this->col_error);
    if (this->error == Script::ErrorType::SCRPT_ERROR_NO) {
        this->state.AddInstructions(list);
        this->state.Execute();
    }
}

const char* LiteScript::Script::GetError() const {
    return Script::GetError(this->error);
}

std::vector<LiteScript::Instruction> LiteScript::Script::GetInstructionList(const char *code) {
    Script::ErrorType errorType;
    unsigned int line_err, col_error;
    return Script::GetInstructionList(code, errorType, line_err, col_error);
}

std::vector<LiteScript::Instruction> LiteScript::Script::GetInstructionList(const char *code, Script::ErrorType &err,
                                                                            unsigned int &line_err,
                                                                            unsigned int &col_error) {
    std::vector<Instruction> list;
    int tmp;
    if ((tmp = LiteScript::Syntax::ReadScript(code, list, err)) < 0) {
        line_err = 0;
        tmp = -tmp;
        int sv = 0;
        for (int i = 0; i < tmp; i++) {
            if (code[i] == '\n') {
                line_err++;
                sv = i + 1;
            }
        }
        int len = tmp - sv;
        char * tmp_s = new char[len + 1];
        memcpy(tmp_s, code + sv, (size_t)len);
        tmp_s[len] = '\0';
        String str(tmp_s);
        col_error = str.GetLength();
    }
    return list;
}

const char* LiteScript::Script::GetError(Script::ErrorType type) {
    return Script::ErrorMsg[type];
}