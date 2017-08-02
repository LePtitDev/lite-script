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

#include <cstdlib>

#include "syntax.hpp"

std::array<unsigned char, LiteScript::Syntax::Operators::OP_NUMBER> LiteScript::Syntax::OP_Priority({
    1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2,
    3, 3, 3,
    4, 4,
    5, 5,
    6, 6, 6, 6,
    7, 7,
    8,
    9,
    10,
    11,
    12,
    13, 13, 13, 13, 13
});

unsigned int LiteScript::Syntax::ReadUndefined(const char *text) {
    std::string str;
    return (ReadName(text, str) > 0 && str == "undefined") ? 9 : 0;
}

unsigned int LiteScript::Syntax::ReadNull(const char *text) {
    std::string str;
    return (ReadName(text, str) > 0 && str == "null") ? 4 : 0;
}

unsigned int LiteScript::Syntax::ReadBoolean(const char *text, bool &res) {
    std::string str;
    if (ReadName(text, str) > 0) {
        if (str == "true") {
            res = true;
            return 4;
        }
        else if (str == "false") {
            res = false;
            return 5;
        }
    }
    return 0;
}

unsigned int LiteScript::Syntax::ReadInteger(const char *text, int &res) {
    unsigned int tmp, nb;
    if (text[0] == '-') {
        nb = ReadUInteger(text + 1, tmp);
        if (nb == 0)
            return 0;
        res = -(int)tmp;
        return nb + 1;
    }
    else if (text[0] == '+') {
        nb = ReadUInteger(text + 1, tmp);
        if (nb == 0)
            return 0;
        res = (int)tmp;
        return nb + 1;
    }
    else {
        nb = ReadUInteger(text, tmp);
        res = (int)tmp;
        return nb;
    }
}

unsigned int LiteScript::Syntax::ReadUInteger(const char *text, unsigned int &res) {
    if (text[0] < '0' || text[0] > '9')
        return 0;
    res = 0;
    for (unsigned int i = 0; true; i++) {
        res += (unsigned int)(text[i] - '0');
        if (text[i + 1] < '0' || text[i + 1] > '9')
            return i + 1;
        else
            res *= 10;
    }
}

unsigned int LiteScript::Syntax::ReadNumber(const char *text, float &res) {
    unsigned int nb;
    if (text[0] == '-') {
        nb = ReadUNumber(text + 1, res);
        if (nb == 0)
            return 0;
        res = -res;
        return nb + 1;
    }
    else if (text[0] == '+') {
        nb = ReadUNumber(text + 1, res);
        if (nb == 0)
            return 0;
        return nb + 1;
    }
    else {
        return ReadUNumber(text, res);
    }
}

unsigned int LiteScript::Syntax::ReadUNumber(const char *text, float &res) {
    unsigned int i = 0;
    if (text[0] >= '0' && text[0] <= '9')
        goto q1;
    if (text[0] == '.' && text[1] >= '0' && text[1] <= '9')
        goto q2;
    else
        return 0;
q1:
    for (; text[i] >= '0' && text[i] <= '9'; i++);
    if (text[i] == '.')
        goto q2;
    if (text[i] == 'e')
        goto q3;
    else
        goto r;
q2:
    for (i++; text[i] >= '0' && text[i] <= '9'; i++);
    if (text[i] == 'e')
        goto q3;
    else
        goto r;
q3:
    i++;
    if (text[i] == '+' || text[i] == '-')
        i++;
    if (text[i] < '0' || text[i] > '9')
        return 0;
    for (; text[i] >= '0' && text[i] <= '9'; i++);
r:
    res = (float)atof(text);
    return i;
}

unsigned int LiteScript::Syntax::ReadName(const char *text, std::string &name) {
    if ((text[0] < 'a' || text[0] > 'z') && (text[0] < 'A' || text[0] > 'Z') && text[0] != '$' && text[0] != '_')
        return 0;
    name.clear();
    name += text[0];
    unsigned int i;
    for (i = 1;
         (text[i] >= 'a' && text[i] <= 'z') ||
         (text[i] >= 'A' && text[i] <= 'Z') ||
         (text[i] >= '0' && text[i] <= '9') ||
                 text[i] == '$' ||
                 text[i] == '_'; i++)
        name += text[i];
    return i;
}

int LiteScript::Syntax::ReadString(const char *text, std::string &string) {
    if (text[0] != '"' && text[0] != '\'')
        return 0;
    string.clear();
    char end_char = text[0];
    int i;
    for (i = 1; text[i] != end_char; i++) {
        if (text[i] == '\0')
            return -1;
        if (text[i] != '\\') {
            string += text[i];
        }
        else {
            switch (text[++i]) {
                case 'n':
                    string += '\n';
                    break;
                case 't':
                    string += '\t';
                    break;
                default:
                    string += text[i];
            }
        }
    }
    return i + 1;
}

unsigned int LiteScript::Syntax::ReadComment(const char *text) {
    if (text[0] != '/')
        return 0;
    unsigned int i;
    if (text[1] == '/') {
        for (i = 2; text[i] != '\n' && text[i] != '\0'; i++);
        return i;
    }
    else if (text[1] == '*') {
        for (i = 2; text[i] != '\0'; i++) {
            if (text[i] == '*' && text[i + 1] == '/') {
                i += 2;
                break;
            }
        }
        return i;
    }
    else {
        return 0;
    }
}

unsigned int LiteScript::Syntax::ReadWhitespace(const char *text) {
    unsigned int i = 0, tmp;
    while (true) {
        if (text[i] == ' ' || text[i] == '\t' || text[i] == '\n')
            i++;
        else if ((tmp = ReadComment(text)) > 0)
            i += tmp;
        else
            break;
    }
    return i;
}

int LiteScript::Syntax::ReadArray(const char *text, std::vector<Instruction> &instrl, Script::ErrorType &errorType) {
    union {
        int i;
        unsigned int ui;
    } tmp;
    int i = 1;
    if (text[0] == '[') {
        // \[ ({expr}(,{expr})*)? \]
        instrl.push_back(Instruction(InstrCode::INSTR_VALUE_ARRAY));
        unsigned int index = 0;
        while (text[i] != ']') {
            if ((tmp.ui = ReadWhitespace(text + i)) > 0)
                i += (int)tmp.ui;
            else if (index == 0 && (tmp.i = ReadExpression(text + i, instrl, errorType)) > 0) {
                i += tmp.i;
                instrl.push_back(Instruction(InstrCode::INSTR_ARRAY_PUSH_NUMERIC, (int)0));
                index++;
            }
            else if (index > 0 && text[i] == ',' && (tmp.i = ReadExpression(text + i + 1, instrl, errorType)) > 0) {
                i += tmp.i + 1;
                instrl.push_back(Instruction(InstrCode::INSTR_ARRAY_PUSH_NUMERIC, (int)index++));
            }
            else {
                if (tmp.i != 0)
                    return tmp.i - i;
                else {
                    errorType = Script::ErrorType::SCRPT_ERROR_SIMPLEARRAY_END;
                    return -i;
                }
            }
        }
        return i + 1;
    }
    else if (text[0] == '{') {
        // \{ ({name}:{expr}(,{name}:{expr})*)? \}
        instrl.push_back(Instruction(InstrCode::INSTR_VALUE_ARRAY));
        bool need_name = true;
        std::string tmp_str;
        while (text[i] != '}') {
            if ((tmp.ui = ReadWhitespace(text + i)) > 0)
                i += (int)tmp.ui;
            else if (need_name && (tmp.ui = ReadName(text + i, tmp_str)) > 0) {
                i += (int)tmp.ui;
                i += (int)ReadWhitespace(text + i);
                if (text[i] != ':') {
                    errorType = Script::ErrorType::SCRPT_ERROR_NAMEDARRAY_COLON;
                    return -i;
                }
                ++i;
                if ((tmp.i = ReadExpression(text + i, instrl, errorType)) <= 0) {
                    if (tmp.i != 0)
                        return tmp.i - i;
                    else {
                        errorType = Script::ErrorType::SCRPT_ERROR_NAMEDARRAY_EXPRESSION;
                        return -i;
                    }
                }
                i += tmp.i;
                instrl.push_back(Instruction(InstrCode::INSTR_ARRAY_PUSH_LITERAL, tmp_str.c_str()));
                need_name = false;
            }
            else if (!need_name && text[i] == ',') {
                i++;
                need_name = true;
            }
            else {
                if (tmp.i != 0)
                    return tmp.i - i;
                else {
                    errorType = Script::ErrorType::SCRPT_ERROR_NAMEDARRAY_END;
                    return -i;
                }
            }
        }
        return i + 1;
    }
    else {
        return 0;
    }
}

int LiteScript::Syntax::ReadCallbackValue(const char *text, std::vector<Instruction> &instrl,
                                     Script::ErrorType &errorType) {
    std::string tmp_s;
    if (ReadName(text, tmp_s) == 0 || tmp_s != "function")
        return 0;
    int i = 8, tmp, jt = instrl.size(), line = instrl.size() + 1;
    instrl.push_back(Instruction(InstrCode::INSTR_JUMP_TO));
    i += ReadWhitespace(text + i);
    // function\(({name}(,{name})*(...)?)?\){instructionBlock}
    if ((tmp = ReadCallbackArguments(text + i, instrl, errorType)) <= 0) {
        if (tmp != 0)
            return tmp - i;
        else {
            errorType = Script::ErrorType::SCRPT_ERROR_CALLBACK_ARGUMENTS;
            return -i;
        }
    }
    i += tmp;
    i += ReadWhitespace(text + i);
    if ((tmp = ReadInstructionBlock(text + i, instrl, errorType)) <= 0) {
        if (tmp != 0)
            return tmp - i;
        else {
            errorType = Script::ErrorType::SCRPT_ERROR_CALLBACK_INSTRUCTIONS;
            return -i;
        }
    }
    i += tmp;
    instrl.push_back(Instruction(InstrCode::INSTR_RETURN));
    instrl[jt] = Instruction(InstrCode::INSTR_JUMP_TO, (int)instrl.size());
    instrl.push_back(Instruction(InstrCode::INSTR_VALUE_CALLBACK, line));
    return i;
}

int LiteScript::Syntax::ReadCallbackArguments(const char *text, std::vector<Instruction> &instrl,
                                              Script::ErrorType &errorType) {
    if (text[0] != '(')
        return 0;
    bool need_name = true;
    std::string name;
    int i = 1;
    union {
        int i;
        unsigned int ui;
    } tmp;
    int index = 0;
    while (text[i] != ')') {
        if ((tmp.ui = ReadWhitespace(text + i)) > 0)
            i += (int)tmp.ui;
        else if (need_name && (tmp.i = ReadName(text + i, name)) > 0) {
            i += tmp.i;
            instrl.push_back(Instruction(InstrCode::INSTR_VALUE_ARG, index));
            instrl.push_back(Instruction(InstrCode::INSTR_DEFINE_VARIABLE, name.c_str()));
            need_name = false;
        }
        else if (!need_name && text[i] == ',') {
            i++;
            index++;
            need_name = true;
        }
        else if (!need_name && text[i] == '.' && text[i + 1] == '.' && text[i + 2] == '.') {
            instrl[instrl.size() - 2].code = InstrCode::INSTR_VALUE_ARGS;
            i += 3;
            i += ReadWhitespace(text + i);
            if (text[i] != ')') {
                errorType = Script::ErrorType::SCRPT_ERROR_CALLBACK_ARGUMENT_END;
                return -i;
            }
        }
        else {
            errorType = Script::ErrorType::SCRPT_ERROR_CALLBACK_ARGUMENT_END;
            return -i;
        }
    }
    if (need_name && index > 0) {
        errorType = Script::ErrorType::SCRPT_ERROR_CALLBACK_ARGUMENT_NAME;
        return -i;
    }
    return i + 1;
}

int LiteScript::Syntax::ReadClassValue(const char *text, std::vector<Instruction> &instrl,
                                       Script::ErrorType &errorType) {
    std::string tmp_s;
    if (ReadName(text, tmp_s) == 0 || tmp_s != "class")
        return 0;
    instrl.push_back(Instruction(InstrCode::INSTR_VALUE_CLASS));
    int i = 5;
    union {
        int i;
        unsigned int ui;
    } tmp;
    i += ReadWhitespace(text + i);
    if ((tmp.i = ReadClassInherits(text + i, instrl, errorType)) >= 0)
        i += tmp.i;
    else
        return tmp.i - i;
    i += ReadWhitespace(text + i);
    if (text[i] != '{') {
        errorType = Script::ErrorType::SCRPT_ERROR_CLASS_BEGIN;
        return -i;
    }
    ++i;
    while (text[i] != '}') {
        if ((tmp.ui = ReadWhitespace(text + i)) > 0)
            i += (int)tmp.ui;
        else if ((tmp.i  = ReadClassMember(text + i, instrl, errorType)) > 0) {
            i += tmp.i;
        }
        else {
            if (tmp.i != 0)
                return tmp.i - i;
            else {
                errorType = Script::ErrorType::SCRPT_ERROR_CLASS_END;
                return -i;
            }
        }
    }
    return i + 1;
}

int LiteScript::Syntax::ReadClassInherits(const char *text, std::vector<Instruction> &instrl,
                                          Script::ErrorType &errorType) {
    if (text[0] != ':')
        return 0;
    int i = 1;
    i += ReadWhitespace(text + i);
    union {
        int i;
        unsigned int ui;
    } tmp;
    bool need_expr = true;
    while (text[i] != '{') {
        if ((tmp.ui = ReadWhitespace(text + i)) > 0)
            i += (int)tmp.ui;
        else if (!need_expr && text[i] == ',') {
            i++;
            need_expr = true;
        }
        else if (need_expr && (tmp.i = ReadExpression(text + i, instrl, errorType)) > 0) {
            i += tmp.i;
            instrl.push_back(Instruction(InstrCode::INSTR_CLASS_INHERIT));
            need_expr = false;
        }
        else {
            if (tmp.i != 0)
                return tmp.i - i;
            else {
                errorType = Script::ErrorType::SCRPT_ERROR_CLASS_INHERIT;
                return -i;
            }
        }
    }
    if (need_expr) {
        errorType = Script::ErrorType::SCRPT_ERROR_CLASS_INHERIT;
        return -i;
    }
    return i;
}

int LiteScript::Syntax::ReadClassMember(const char *text, std::vector<Instruction> &instrl,
                                        Script::ErrorType &errorType) {
    int i = 0, tmp, curr;
    if ((tmp = ReadClassOperator(text, instrl, errorType)) != 0)
        return tmp;
    if ((tmp = ReadClass(text, instrl, errorType)) != 0) {
        if (tmp > 0)
            instrl.back().code = InstrCode::INSTR_CLASS_PUSH_STATIC;
        return tmp;
    }

    bool is_static = false;
    if (text[0] == 's' &&
        text[1] == 't' &&
        text[2] == 'a' &&
        text[3] == 't' &&
        text[4] == 'i' &&
        text[5] == 'c') {
        is_static = true;
        i = 6;
        i += ReadWhitespace(text + i);
    }

    if (text[i] == 'v') {
        curr = instrl.size();
        if ((tmp = ReadVariableDefinition(text + i, instrl, errorType)) <= 0) {
            if (tmp != 0)
                return tmp - i;
            else {
                errorType = Script::ErrorType::SCRPT_ERROR_CLASS_INVALID;
                return -i;
            }
        }
        i += tmp;
        for (int sz = instrl.size(); curr < sz; curr++) {
            if (instrl[curr].code == InstrCode::INSTR_DEFINE_VARIABLE)
                instrl[curr].code = is_static ? InstrCode::INSTR_CLASS_PUSH_STATIC : InstrCode::INSTR_CLASS_PUSH_USTATIC;
        }
        i += ReadWhitespace(text + i);
        if (text[i] != ';') {
            errorType = Script::ErrorType::SCRPT_ERROR_SEMICOLON;
            return -i;
        }
        ++i;
    }
    else if (text[i] == 'f') {
        if ((tmp = ReadCallback(text + i, instrl, errorType)) <= 0) {
            if (tmp != 0)
                return tmp - i;
            else {
                errorType = Script::ErrorType::SCRPT_ERROR_CLASS_INVALID;
                return -i;
            }
        }
        i += tmp;
        instrl.back().code = is_static ? InstrCode::INSTR_CLASS_PUSH_STATIC : InstrCode::INSTR_CLASS_PUSH_USTATIC;
    }
    else {
        errorType = Script::ErrorType::SCRPT_ERROR_CLASS_INVALID;
        return -i;
    }
    return i;
}

int LiteScript::Syntax::ReadClassOperator(const char *text, std::vector<Instruction> &instrl,
                                          Script::ErrorType &errorType) {
    if (text[0] != 'o' ||
        text[1] != 'p' ||
        text[2] != 'e' ||
        text[3] != 'r' ||
        text[4] != 'a' ||
        text[5] != 't' ||
        text[6] != 'o' ||
        text[7] != 'r')
        return 0;
    int i = 8;
    union {
        int i;
        unsigned int ui;
    } tmp;
    i += ReadWhitespace(text + i);
    std::string name;
    if ((tmp.ui = ReadName(text + i, name)) <= 0) {
        errorType = Script::ErrorType::SCRPT_ERROR_CLASS_OPERATOR_NAME;
        return -i;
    }

    Class::OperatorType op;
    if (name == "assign")
        op = Class::OperatorType::OP_TYPE_ASSIGN;
    else if (name == "unary_plus")
        op = Class::OperatorType::OP_TYPE_UNARY_PLUS;
    else if (name == "unary_minus")
        op = Class::OperatorType::OP_TYPE_UNARY_MINUS;
    else if (name == "pre_increment")
        op = Class::OperatorType::OP_TYPE_PRE_INCR;
    else if (name == "pre_decrement")
        op = Class::OperatorType::OP_TYPE_PRE_DECR;
    else if (name == "post_increment")
        op = Class::OperatorType::OP_TYPE_POST_INCR;
    else if (name == "post_decrement")
        op = Class::OperatorType::OP_TYPE_POST_DECR;
    else if (name == "add")
        op = Class::OperatorType::OP_TYPE_ADD;
    else if (name == "subtract")
        op = Class::OperatorType::OP_TYPE_SUB;
    else if (name == "multiply")
        op = Class::OperatorType::OP_TYPE_MUL;
    else if (name == "divide")
        op = Class::OperatorType::OP_TYPE_DIV;
    else if (name == "modulo")
        op = Class::OperatorType::OP_TYPE_MOD;
    else if (name == "equal")
        op = Class::OperatorType::OP_TYPE_EQU;
    else if (name == "different")
        op = Class::OperatorType::OP_TYPE_ADD;
    else if (name == "greater")
        op = Class::OperatorType::OP_TYPE_GREAT;
    else if (name == "lesser")
        op = Class::OperatorType::OP_TYPE_LESS;
    else if (name == "greater_equal")
        op = Class::OperatorType::OP_TYPE_GREAT_EQU;
    else if (name == "lesser_equal")
        op = Class::OperatorType::OP_TYPE_LESS_EQU;
    else if (name == "not")
        op = Class::OperatorType::OP_TYPE_LOG_NOT;
    else if (name == "and")
        op = Class::OperatorType::OP_TYPE_LOG_AND;
    else if (name == "or")
        op = Class::OperatorType::OP_TYPE_LOG_OR;
    else if (name == "bitwise_not")
        op = Class::OperatorType::OP_TYPE_BIT_NOT;
    else if (name == "bitwise_and")
        op = Class::OperatorType::OP_TYPE_BIT_AND;
    else if (name == "bitwise_or")
        op = Class::OperatorType::OP_TYPE_BIT_OR;
    else if (name == "bitwise_xor")
        op = Class::OperatorType::OP_TYPE_BIT_XOR;
    else if (name == "left_shift")
        op = Class::OperatorType::OP_TYPE_LSHIFT;
    else if (name == "right_shift")
        op = Class::OperatorType::OP_TYPE_RSHIFT;
    else if (name == "add_assign")
        op = Class::OperatorType::OP_TYPE_ADD_ASSIGN;
    else if (name == "subtract_assign")
        op = Class::OperatorType::OP_TYPE_SUB_ASSIGN;
    else if (name == "multiply_assign")
        op = Class::OperatorType::OP_TYPE_MUL_ASSIGN;
    else if (name == "divide_assign")
        op = Class::OperatorType::OP_TYPE_DIV_ASSIGN;
    else if (name == "get")
        op = Class::OperatorType::OP_TYPE_ARRAY;
    else if (name == "call")
        op = Class::OperatorType::OP_TYPE_CALL;
    else {
        errorType = Script::ErrorType::SCRPT_ERROR_CLASS_OPERATOR_INVALID;
        return -i;
    }

    i += (int)tmp.ui;
    i += ReadWhitespace(text + i);
    if ((tmp.i = ReadCallbackArguments(text + i, instrl, errorType)) <= 0) {
        if (tmp.i != 0)
            return tmp.i - i;
        else {
            errorType = Script::ErrorType::SCRPT_ERROR_CALLBACK_ARGUMENTS;
            return -i;
        }
    }
    i += tmp.i;
    i += ReadWhitespace(text + i);
    if ((tmp.i = ReadInstructionBlock(text + i, instrl, errorType)) <= 0) {
        if (tmp.i != 0)
            return tmp.i - i;
        else {
            errorType = Script::ErrorType::SCRPT_ERROR_CALLBACK_INSTRUCTIONS;
            return -i;
        }
    }
    i += tmp.i;
    instrl.push_back(Instruction(InstrCode::INSTR_CLASS_PUSH_OPERATOR, (int)op));
    return i;
}

int LiteScript::Syntax::ReadValue(const char *text, std::vector<Instruction> &instrl, Script::ErrorType &errorType) {
    union {
        int i;
        unsigned int ui;
    } tmp;
    union {
        bool b;
        float f;
    } res;
    std::string tmp_s;
    if ((tmp.ui = ReadUndefined(text)) > 0) {
        instrl.push_back(Instruction(InstrCode::INSTR_VALUE_UNDEFINED));
        return (int) tmp.ui;
    }
    else if ((tmp.ui = ReadNull(text)) > 0) {
        instrl.push_back(Instruction(InstrCode::INSTR_VALUE_NULL));
        return (int) tmp.ui;
    }
    else if ((tmp.ui = ReadBoolean(text, res.b)) > 0) {
        instrl.push_back(Instruction(InstrCode::INSTR_VALUE_BOOLEAN, res.b));
        return (int) tmp.ui;
    }
    else if ((tmp.ui = ReadNumber(text, res.f)) > 0) {
        instrl.push_back(Instruction(InstrCode::INSTR_VALUE_NUMBER, res.f));
        return (int) tmp.ui;
    }
    else if ((tmp.i = ReadCallbackValue(text, instrl, errorType)) > 0) {
        return tmp.i;
    }
    else if (tmp.i != 0)
        return tmp.i;
    else if ((tmp.i = ReadClassValue(text, instrl, errorType)) > 0) {
        return tmp.i;
    }
    else if (tmp.i != 0)
        return tmp.i;
    else if ((tmp.ui = ReadName(text, tmp_s)) > 0) {
        instrl.push_back(Instruction(InstrCode::INSTR_VALUE_VARIABLE, tmp_s.c_str()));
        return (int) tmp.ui;
    }
    else if ((tmp.i = ReadString(text, tmp_s)) > 0) {
        instrl.push_back(Instruction(InstrCode::INSTR_VALUE_STRING, tmp_s.c_str()));
        return tmp.i;
    }
    else if (tmp.i != 0)
        return tmp.i;
    else if ((tmp.i = ReadArray(text, instrl, errorType)) > 0) {
        return tmp.i;
    }
    else if (tmp.i != 0)
        return tmp.i;
    else if (text[0] == '(') {
        int i = 1;
        i += ReadWhitespace(text + i);
        if ((tmp.i = ReadExpression(text + i, instrl, errorType)) <= 0) {
            if (tmp.i != 0)
                return tmp.i - i;
            else {
                errorType = Script::ErrorType::SCRPT_ERROR_EXPRESSION;
                return -i;
            }
        }
        i += tmp.i;
        i += ReadWhitespace(text + i);
        if (text[i] != ')') {
            errorType = Script::ErrorType::SCRPT_ERROR_PARENTHESIS_CLOSE;
            return -i;
        }
        return i + 1;
    }
    else
        return 0;
}

void LiteScript::Syntax::PopOperator(std::vector<Operators> &op, std::vector<Instruction>& instrl) {
    switch (op.back()) {
        // LEVEL 1
        case Operators::OP_POST_INCR:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_POST_INCR));
            break;
        case Operators::OP_POST_DECR:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_POST_DECR));
            break;
        case Operators::OP_CALL:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_CALL));
            break;
        case Operators::OP_GET:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_ARRAY));
            break;
        case Operators::OP_MEMBER:
            instrl.back().code = InstrCode::INSTR_OP_MEMBER;
            break;
        // LEVEL 2
        case Operators::OP_PRE_INCR:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_PRE_INCR));
            break;
        case Operators::OP_PRE_DECR:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_PRE_DECR));
            break;
        case Operators::OP_UNARY_PLUS:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_UNARY_PLUS));
            break;
        case Operators::OP_UNARY_MINUS:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_UNARY_MINUS));
            break;
        case Operators::OP_NOT:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_LOG_NOT));
            break;
        case Operators::OP_BIT_NOT:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_BIT_NOT));
            break;
        case Operators::OP_NEW:
            instrl.push_back(Instruction(InstrCode::INSTR_VALUE_OBJECT));
            break;
        // LEVEL 3
        case Operators::OP_MUL:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_MUL));
            break;
        case Operators::OP_DIV:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_DIV));
            break;
        case Operators::OP_MOD:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_MOD));
            break;
        // LEVEL 4
        case Operators::OP_ADD:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_ADD));
            break;
        case Operators::OP_SUB:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_SUB));
            break;
        // LEVEL 5
        case Operators::OP_LSHIFT:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_LSHIFT));
            break;
        case Operators::OP_RSHIFT:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_RSHIFT));
            break;
        // LEVEL 6
        case Operators::OP_LESS:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_LESS));
            break;
        case Operators::OP_LESS_EQU:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_LESS_EQU));
            break;
        case Operators::OP_GREAT:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_GREAT));
            break;
        case Operators::OP_GREAT_EQU:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_GREAT_EQU));
            break;
        // LEVEL 7
        case Operators::OP_EQU:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_EQU));
            break;
        case Operators::OP_DIF:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_DIF));
            break;
        // LEVEL 8
        case Operators::OP_BIT_AND:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_BIT_AND));
            break;
        // LEVEL 9
        case Operators::OP_BIT_XOR:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_BIT_XOR));
            break;
        // LEVEL 10
        case Operators::OP_BIT_OR:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_BIT_OR));
            break;
        // LEVEL 11
        case Operators::OP_AND:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_LOG_AND));
            break;
        // LEVEL 12
        case Operators::OP_OR:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_LOG_OR));
            break;
        // LEVEL 13
        case Operators::OP_ASSIGN:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_ASSIGN));
            break;
        case Operators::OP_ADD_ASSIGN:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_ADD_ASSIGN));
            break;
        case Operators::OP_SUB_ASSIGN:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_SUB_ASSIGN));
            break;
        case Operators::OP_MUL_ASSIGN:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_MUL_ASSIGN));
            break;
        case Operators::OP_DIV_ASSIGN:
            instrl.push_back(Instruction(InstrCode::INSTR_OP_DIV_ASSIGN));
            break;
        default:
            break;
    }
    op.pop_back();
}

void LiteScript::Syntax::PushOperator(std::vector<Operators> &opl, Operators opc, std::vector<Instruction>& instrl) {
    while (opl.size() > 0 && OP_Priority[opl.back()] <= OP_Priority[opc])
        PopOperator(opl, instrl);
    opl.push_back(opc);
}

int LiteScript::Syntax::ReadPrefixOperator(const char *text, std::vector<Operators> &op,
                                           std::vector<Instruction> &instrl, Script::ErrorType &errorType) {
    if (text[0] == 'n' &&
        text[1] == 'e' &&
        text[2] == 'w') {
        PushOperator(op, Operators::OP_NEW, instrl);
        return 3;
    }
    else if (text[0] == '+') {
        if (text[1] == '+') {
            PushOperator(op, Operators::OP_PRE_INCR, instrl);
            return 2;
        }
        else {
            PushOperator(op, Operators::OP_UNARY_PLUS, instrl);
            return 1;
        }
    }
    else if (text[0] == '-') {
        if (text[1] == '-') {
            PushOperator(op, Operators::OP_PRE_DECR, instrl);
            return 2;
        }
        else {
            PushOperator(op, Operators::OP_UNARY_MINUS, instrl);
            return 1;
        }
    }
    else if (text[0] == '!') {
        PushOperator(op, Operators::OP_NOT, instrl);
        return 1;
    }
    else if (text[0] == '~') {
        PushOperator(op, Operators::OP_BIT_NOT, instrl);
        return 1;
    }
    else
        return 0;
}

int LiteScript::Syntax::ReadSuffixOperator(const char *text, std::vector<Operators> &op,
                                           std::vector<Instruction> &instrl, Script::ErrorType &errorType) {
    if (text[0] == '+' && text[1] == '+') {
        PushOperator(op, Operators::OP_POST_INCR, instrl);
        return 2;
    }
    else if (text[0] == '-' && text[1] == '-') {
        PushOperator(op, Operators::OP_POST_DECR, instrl);
        return 2;
    }
    else
        return 0;
}

int LiteScript::Syntax::ReadOperator(const char *text, std::vector<Operators> &op, bool& need_value, std::vector<Instruction> &instrl,
                                     Script::ErrorType &errorType) {
    need_value = true;
    if (text[0] == '(') {
        while (op.size() > 0 && OP_Priority[op.back()] == 1)
            PopOperator(op, instrl);
        instrl.push_back(Instruction(InstrCode::INSTR_PUSH_ARGS));
        int i = 1, index = 0;
        union {
            int i;
            unsigned int ui;
        } tmp;
        bool need_arg = true;
        while (text[i] != ')') {
            if ((tmp.ui = ReadWhitespace(text + i)) > 0)
                i += (int)tmp.ui;
            else if (need_arg && (tmp.i = ReadExpression(text + i, instrl, errorType)) > 0) {
                i += tmp.i;
                need_arg = false;
                instrl.push_back(Instruction(InstrCode::INSTR_DEFINE_ARG, index++));
            }
            else if (tmp.i < 0)
                return tmp.i - i;
            else if (!need_arg && text[i] == ',') {
                i++;
                need_arg = true;
            }
            else {
                errorType = Script::ErrorType::SCRPT_ERROR_PARENTHESIS_CLOSE;
                return -i;
            }
        }
        if (need_arg && index > 0) {
            errorType = Script::ErrorType::SCRPT_ERROR_EXPRESSION;
            return -i;
        }
        if (op.size() > 0 && op.back() == Operators::OP_NEW)
            instrl.push_back(Instruction(InstrCode::INSTR_VALUE_OBJECT));
        else
            instrl.push_back(Instruction(InstrCode::INSTR_OP_CALL));
        instrl.push_back(Instruction(InstrCode::INSTR_POP_ARGS));
        need_value = false;
        return i + 1;
    }
    else if (text[0] == '[') {
        while (op.size() > 0 && OP_Priority[op.back()] == 1)
            PopOperator(op, instrl);
        int i = 1, tmp;
        i += (int)ReadWhitespace(text + i);
        if ((tmp = ReadExpression(text + i, instrl, errorType)) <= 0) {
            if (tmp != 0)
                return tmp - i;
            else {
                errorType = Script::ErrorType::SCRPT_ERROR_EXPRESSION;
                return -i;
            }
        }
        i += tmp;
        i += (int)ReadWhitespace(text + i);
        if (text[i] != ']') {
            errorType = Script::ErrorType::SCRPT_ERROR_BRACKET_CLOSE;
            return -i;
        }
        instrl.push_back(Instruction(InstrCode::INSTR_OP_ARRAY));
        need_value = false;
        return i + 1;
    }
    else if (text[0] == '.') {
        while (op.size() > 0 && OP_Priority[op.back()] == 1)
            PopOperator(op, instrl);
        int i = 1;
        i += (int)ReadWhitespace(text + i);
        unsigned int tmp;
        std::string name;
        if ((tmp = ReadName(text + i, name)) == 0) {
            errorType = Script::ErrorType::SCRPT_ERROR_NAME;
            return -i;
        }
        i += (int)tmp;
        instrl.push_back(Instruction(InstrCode::INSTR_OP_MEMBER, name.c_str()));
        return i;
    }
    else if (text[0] == '?') {
        while (op.size() > 0 && OP_Priority[op.back()] == OP_Priority[Operators::OP_ASSIGN])
            PopOperator(op, instrl);
        int i = 1, tmp, line_je = instrl.size();
        i += (int)ReadWhitespace(text + i);
        instrl.push_back(Instruction(InstrCode::INSTR_JUMP_ELSE));
        if ((tmp = ReadExpression(text + i, instrl, errorType)) <= 0) {
            if (tmp != 0)
                return tmp - i;
            else {
                errorType = Script::ErrorType::SCRPT_ERROR_EXPRESSION;
                return -i;
            }
        }
        i += tmp;
        i += (int)ReadWhitespace(text + i);
        int line_jt = instrl.size();
        instrl.push_back(Instruction(InstrCode::INSTR_JUMP_TO));
        instrl[line_je] = Instruction(InstrCode::INSTR_JUMP_ELSE, (int)instrl.size());
        if (text[i] != ':') {
            errorType = Script::ErrorType::SCRPT_ERROR_COLON;
            return -i;
        }
        i += (int)ReadWhitespace(text + i);
        if ((tmp = ReadExpression(text + i, instrl, errorType)) <= 0) {
            if (tmp != 0)
                return tmp - i;
            else {
                errorType = Script::ErrorType::SCRPT_ERROR_EXPRESSION;
                return -i;
            }
        }
        instrl[line_jt] = Instruction(InstrCode::INSTR_JUMP_TO, (int)instrl.size());
        i += tmp;
        return i;
    }
    else {
        switch (text[0]) {
            case '+':
                if (text[1] == '=') {
                    PushOperator(op, Operators::OP_ADD_ASSIGN, instrl);
                    return 2;
                }
                else {
                    PushOperator(op, Operators::OP_ADD, instrl);
                    return 1;
                }
            case '-':
                if (text[1] == '=') {
                    PushOperator(op, Operators::OP_SUB_ASSIGN, instrl);
                    return 2;
                }
                else {
                    PushOperator(op, Operators::OP_SUB, instrl);
                    return 1;
                }
            case '*':
                if (text[1] == '=') {
                    PushOperator(op, Operators::OP_MUL_ASSIGN, instrl);
                    return 2;
                }
                else {
                    PushOperator(op, Operators::OP_MUL, instrl);
                    return 1;
                }
            case '/':
                if (text[1] == '=') {
                    PushOperator(op, Operators::OP_DIV_ASSIGN, instrl);
                    return 2;
                }
                else {
                    PushOperator(op, Operators::OP_DIV, instrl);
                    return 1;
                }
            case '%':
                PushOperator(op, Operators::OP_MOD, instrl);
                return 1;
            case '=':
                if (text[1] == '=') {
                    PushOperator(op, Operators::OP_EQU, instrl);
                    return 2;
                }
                else {
                    PushOperator(op, Operators::OP_ASSIGN, instrl);
                    return 1;
                }
            case '!':
                if (text[1] == '=') {
                    PushOperator(op, Operators::OP_DIF, instrl);
                    return 2;
                }
                else {
                    errorType = Script::ErrorType::SCRPT_ERROR_OPERATOR;
                    return -1;
                }
            case '<':
                if (text[1] == '=') {
                    PushOperator(op, Operators::OP_LESS_EQU, instrl);
                    return 2;
                }
                else if (text[1] == '<') {
                    PushOperator(op, Operators::OP_LSHIFT, instrl);
                    return 2;
                }
                else {
                    PushOperator(op, Operators::OP_LESS, instrl);
                    return 1;
                }
            case '>':
                if (text[1] == '=') {
                    PushOperator(op, Operators::OP_GREAT_EQU, instrl);
                    return 2;
                }
                else if (text[1] == '>') {
                    PushOperator(op, Operators::OP_RSHIFT, instrl);
                    return 2;
                }
                else {
                    PushOperator(op, Operators::OP_GREAT, instrl);
                    return 1;
                }
            case '&':
                if (text[1] == '&') {
                    PushOperator(op, Operators::OP_AND, instrl);
                    return 2;
                }
                else {
                    PushOperator(op, Operators::OP_BIT_AND, instrl);
                    return 1;
                }
            case '|':
                if (text[1] == '|') {
                    PushOperator(op, Operators::OP_OR, instrl);
                    return 2;
                }
                else {
                    PushOperator(op, Operators::OP_BIT_OR, instrl);
                    return 1;
                }
            case '^':
                PushOperator(op, Operators::OP_BIT_XOR, instrl);
                return 1;
            default:
                need_value = false;
                return 0;
        }
    }
}

int LiteScript::Syntax::ReadExpression(const char *text, std::vector<Instruction> &instrl,
                                       Script::ErrorType &errorType) {
    union {
        int i;
        unsigned int ui;
    } tmp;
    int i = 0, index = 0;
    std::vector<Operators> op_heap;
    bool need_value = true;
    while (true) {
        if ((tmp.ui = ReadWhitespace(text + i)) > 0)
            i += (int)tmp.ui;
        else if (need_value) {
            bool terminate = true;
            if ((tmp.i = ReadPrefixOperator(text + i, op_heap, instrl, errorType)) > 0) {
                terminate = false;
                i += tmp.i;
                i += (int)ReadWhitespace(text + i);
            }
            if ((tmp.i = ReadValue(text + i, instrl, errorType)) <= 0) {
                if (tmp.i != 0)
                    return tmp.i;
                if (terminate)
                    break;
                errorType = Script::ErrorType::SCRPT_ERROR_VALUE;
                return -i;
            }
            i += tmp.i;
            if ((tmp.i = ReadSuffixOperator(text + i, op_heap, instrl, errorType)) > 0)
                i += tmp.i;
            need_value = false;
            index++;
        }
        else {
            if ((tmp.i = ReadOperator(text + i, op_heap, need_value, instrl, errorType)) <= 0) {
                if (tmp.i != 0)
                    return tmp.i - i;
                else
                    break;
            }
            i += tmp.i;
        }
    }
    if (need_value) {
        if (index == 0)
            return 0;
        else {
            errorType = Script::ErrorType::SCRPT_ERROR_VALUE;
            return -i;
        }
    }
    while (!op_heap.empty())
        PopOperator(op_heap, instrl);
    return i;
}

int LiteScript::Syntax::ReadVariableDefinition(const char *text, std::vector<Instruction> &instrl,
                                               Script::ErrorType &errorType) {
    if (text[0] != 'v' ||
        text[1] != 'a' ||
        text[2] != 'r' ||
        text[3] != ' ')
        return 0;
    int i = 4, index = 0;
    union {
        int i;
        unsigned int ui;
    } tmp;
    bool need_name = true;
    std::string name;
    while (true) {
        if ((tmp.ui = ReadWhitespace(text + i)) > 0)
            i += (int)tmp.ui;
        else if (need_name && (tmp.ui = ReadName(text + i, name)) > 0) {
            i += (int)tmp.ui;
            i += (int)ReadWhitespace(text + i);
            if (text[i] == '=') {
                i++;
                i += (int)ReadWhitespace(text + i);
                if ((tmp.i = ReadExpression(text + i, instrl, errorType)) <= 0) {
                    if (tmp.i != 0)
                        return tmp.i - i;
                    else {
                        errorType = Script::ErrorType::SCRPT_ERROR_EXPRESSION;
                        return -i;
                    }
                }
                i += tmp.i;
            }
            else
                instrl.push_back(Instruction(InstrCode::INSTR_VALUE_UNDEFINED));
            instrl.push_back(Instruction(InstrCode::INSTR_DEFINE_VARIABLE, name.c_str()));
            need_name = false;
            index++;
        }
        else if (!need_name && text[i] == ',') {
            need_name = true;
            i++;
        }
        else {
            break;
        }
    }
    if (need_name) {
        if (index > 0) {
            errorType = Script::ErrorType::SCRPT_ERROR_NAME;
            return -i;
        }
        else
            return 0;
    }
    return i;
}

int LiteScript::Syntax::ReadControlIf(const char *text, std::vector<Instruction> &instrl,
                                      Script::ErrorType &errorType) {
    if (text[0] != 'i' ||
        text[1] != 'f')
        return 0;
    int i = 2, tmp;
    i += (int)ReadWhitespace(text + i);
    if (text[i] != '(') {
        errorType = Script::ErrorType::SCRPT_ERROR_PARENTHESIS_OPEN;
        return -i;
    }
    i++;
    i += (int)ReadWhitespace(text + i);
    if ((tmp = ReadExpression(text + i, instrl, errorType)) <= 0) {
        if (tmp != 0)
            return tmp - i;
        else {
            errorType = Script::ErrorType::SCRPT_ERROR_EXPRESSION;
            return -i;
        }
    }
    i += tmp;
    i += (int)ReadWhitespace(text + i);
    if (text[i] != ')') {
        errorType = Script::ErrorType::SCRPT_ERROR_PARENTHESIS_CLOSE;
        return -i;
    }
    int je = instrl.size();
    instrl.push_back(Instruction(InstrCode::INSTR_JUMP_ELSE));
    i++;
    i += (int)ReadWhitespace(text + i);
    if ((tmp = ReadInstruction(text + i, instrl, errorType)) <= 0) {
        if (tmp != 0)
            return tmp - i;
        else {
            errorType = Script::ErrorType::SCRPT_ERROR_INSTRUCTION;
            return -i;
        }
    }
    i += tmp;
    i += (int)ReadWhitespace(text + i);
    instrl[je] = Instruction(InstrCode::INSTR_JUMP_ELSE, (int)instrl.size());
    if (text[i+0] != 'e' ||
        text[i+1] != 'l' ||
        text[i+2] != 's' ||
        text[i+3] != 'e')
        return i;
    i += 4;
    int jt = instrl.size();
    instrl.push_back(Instruction(InstrCode::INSTR_JUMP_TO));
    if ((tmp = ReadInstruction(text + i, instrl, errorType)) <= 0) {
        if (tmp != 0)
            return tmp - i;
        else {
            errorType = Script::ErrorType::SCRPT_ERROR_INSTRUCTION;
            return -i;
        }
    }
    instrl[jt] = Instruction(InstrCode::INSTR_JUMP_TO, (int)instrl.size());
    i += tmp;
    return i;
}

int LiteScript::Syntax::ReadCallback(const char *text, std::vector<Instruction> &instrl,
                                     Script::ErrorType &errorType) {
    if (text[0] != 'f' ||
        text[1] != 'u' ||
        text[2] != 'n' ||
        text[3] != 'c' ||
        text[4] != 't' ||
        text[5] != 'i' ||
        text[6] != 'o' ||
        text[7] != 'n' ||
        text[8] != ' ')
        return 0;
    int i = 9, jt = instrl.size(), line = instrl.size() + 1;;
    union {
        int i;
        unsigned int ui;
    } tmp;
    std::string name;
    instrl.push_back(Instruction(InstrCode::INSTR_JUMP_TO));
    i += (int)ReadWhitespace(text + i);
    // function {name}\(({name}(,{name})*(...)?)?\){instructionBlock}
    if ((tmp.ui = ReadName(text + i, name)) == 0) {
        errorType = Script::ErrorType::SCRPT_ERROR_NAME;
        return -i;
    }
    i += (int)tmp.ui;
    i += (int)ReadWhitespace(text + i);
    if ((tmp.i = ReadCallbackArguments(text + i, instrl, errorType)) <= 0) {
        if (tmp.i != 0)
            return tmp.i - i;
        else {
            errorType = Script::ErrorType::SCRPT_ERROR_CALLBACK_ARGUMENTS;
            return -i;
        }
    }
    i += tmp.i;
    i += ReadWhitespace(text + i);
    if ((tmp.i = ReadInstructionBlock(text + i, instrl, errorType)) <= 0) {
        if (tmp.i != 0)
            return tmp.i - i;
        else {
            errorType = Script::ErrorType::SCRPT_ERROR_CALLBACK_INSTRUCTIONS;
            return -i;
        }
    }
    i += tmp.i;
    instrl.push_back(Instruction(InstrCode::INSTR_RETURN));
    instrl[jt] = Instruction(InstrCode::INSTR_JUMP_TO, (int)instrl.size());
    instrl.push_back(Instruction(InstrCode::INSTR_VALUE_CALLBACK, line));
    instrl.push_back(Instruction(InstrCode::INSTR_DEFINE_VARIABLE, name.c_str()));
    return i;
}

int LiteScript::Syntax::ReadClass(const char *text, std::vector<Instruction> &instrl, Script::ErrorType &errorType) {
    if (text[0] != 'c' ||
        text[1] != 'l' ||
        text[2] != 'a' ||
        text[3] != 's' ||
        text[4] != 's' ||
        text[5] != ' ')
        return 0;
    int i = 6;
    union {
        int i;
        unsigned int ui;
    } tmp;
    std::string name;
    i += (int)ReadWhitespace(text + i);
    if ((tmp.ui = ReadName(text + i, name)) == 0) {
        errorType = Script::ErrorType::SCRPT_ERROR_NAME;
        return -i;
    }
    instrl.push_back(Instruction(InstrCode::INSTR_VALUE_CLASS));
    i += (int)tmp.ui;
    i += ReadWhitespace(text + i);
    if ((tmp.i = ReadClassInherits(text + i, instrl, errorType)) >= 0)
        i += tmp.i;
    else
        return tmp.i - i;
    i += ReadWhitespace(text + i);
    if (text[i] != '{') {
        errorType = Script::ErrorType::SCRPT_ERROR_CLASS_BEGIN;
        return -i;
    }
    ++i;
    while (text[i] != '}') {
        if ((tmp.ui = ReadWhitespace(text + i)) > 0)
            i += (int)tmp.ui;
        else if ((tmp.i  = ReadClassMember(text + i, instrl, errorType)) > 0) {
            i += tmp.i;
        }
        else {
            if (tmp.i != 0)
                return tmp.i - i;
            else {
                errorType = Script::ErrorType::SCRPT_ERROR_CLASS_END;
                return -i;
            }
        }
    }
    instrl.push_back(Instruction(InstrCode::INSTR_CLASS_CONSTRUCTOR, name.c_str()));
    instrl.push_back(Instruction(InstrCode::INSTR_DEFINE_VARIABLE, name.c_str()));
    return i + 1;
}

int LiteScript::Syntax::ReadInstructionBlock(const char *text, std::vector<Instruction> &instrl,
                                             Script::ErrorType &errorType) {
    if (text[0] != '{')
        return 0;
    int i = 1;
    union {
        int i;
        unsigned int ui;
    } tmp;
    i += ReadWhitespace(text + i);
    while (text[i] != '}') {
        if ((tmp.ui = ReadWhitespace(text + i)) > 0)
            i += (int)tmp.ui;
        else if ((tmp.i = ReadInstruction(text + i, instrl, errorType)) > 0) {
            i += tmp.i;
        }
        else {
            if (tmp.i != 0)
                return -i;
            else {
                errorType = Script::ErrorType::SCRPT_ERROR_BRACE_CLOSE;
                return -i;
            }
        }
    }
    return i + 1;
}

int LiteScript::Syntax::ReadInstruction(const char *text, std::vector<Instruction> &instrl,
                                        Script::ErrorType &errorType) {
    int i = 0, tmp;
    i += ReadWhitespace(text + i);
    if (text[i] == ';')
        return i + 1;
    if ((tmp = ReadInstructionBlock(text + i, instrl, errorType)) > 0)
        return i + tmp;
    else if (tmp != 0)
        return tmp - i;
    if ((tmp = ReadVariableDefinition(text + i, instrl, errorType)) > 0) {
        i += tmp;
        i += ReadWhitespace(text + i);
        if (text[i] != ';') {
            errorType = Script::ErrorType::SCRPT_ERROR_SEMICOLON;
            return -i;
        }
        return i + 1;
    }
    else if (tmp != 0)
        return tmp - i;
    if ((tmp = ReadCallback(text + i, instrl, errorType)) > 0)
        return tmp + i;
    else if (tmp != 0)
        i = tmp - i;
    if ((tmp = ReadClass(text + i, instrl, errorType)) > 0)
        return tmp + i;
    else if (tmp != 0)
        i = tmp - i;
    if ((tmp = ReadControlIf(text + i, instrl, errorType)) > 0)
        return tmp + i;
    else if (tmp != 0)
        return tmp - i;
    if ((tmp = ReadExpression(text + i, instrl, errorType)) > 0) {
        i += tmp;
        i += ReadWhitespace(text + i);
        if (text[i] != ';') {
            errorType = Script::ErrorType::SCRPT_ERROR_SEMICOLON;
            return -i;
        }
        instrl.push_back(Instruction(InstrCode::INSTR_VALUE_POP));
        return i + 1;
    }
    else if (tmp != 0)
        return tmp - i;
    if (errorType != Script::ErrorType::SCRPT_ERROR_NO)
        return i;
    return 0;
}

int LiteScript::Syntax::ReadScript(const char *text, std::vector<Instruction> &instrl, Script::ErrorType &errorType) {
    int i = 0, tmp;
    while (text[i] != '\0'){
        if ((tmp = (int)ReadWhitespace(text + i)) > 0)
            i += tmp;
        else if ((tmp = ReadInstruction(text + i, instrl, errorType)) > 0)
            i += tmp;
        else {
            if (tmp != 0)
                return tmp - i;
            else {
                errorType = Script::ErrorType::SCRPT_ERROR_UNKNOW;
                return -i;
            }
        }
    }
    return i;
}