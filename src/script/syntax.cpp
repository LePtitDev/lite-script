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

unsigned int LiteScript::Syntax::ReadUndefined(const char *text) {
    return (text[0] == 'u' &&
            text[1] == 'n' &&
            text[2] == 'd' &&
            text[3] == 'e' &&
            text[4] == 'f' &&
            text[5] == 'i' &&
            text[6] == 'n' &&
            text[7] == 'e' &&
            text[8] == 'd') ? 9 : 0;
}

unsigned int LiteScript::Syntax::ReadNull(const char *text) {
    return (text[0] == 'n' &&
            text[1] == 'u' &&
            text[2] == 'l' &&
            text[3] == 'l') ? 4 : 0;
}

unsigned int LiteScript::Syntax::ReadBoolean(const char *text, bool &res) {
    if (text[0] == 't' && text[1] == 'r' && text[2] == 'u' && text[3] == 'e') {
        res = true;
        return 4;
    }
    else if (text[0] == 'f' && text[1] == 'a' && text[2] == 'l' && text[3] == 's' && text[4] == 'e') {
        res = false;
        return 5;
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
    if (text[0] < 'a' && text[0] > 'z' && text[0] < 'A' && text[0] > 'Z' && text[0] != '$' && text[0] != '_')
        return 0;
    name.clear();
    name += text[0];
    unsigned int i;
    for (i = 1;
         (text[i] >= 'a' && text[i] <= 'z') ||
         (text[i] >= 'A' && text[i] <= 'Z') ||
         (text[i] >= '0' && text[i] <= '9') ||
                 text[i] == '$' ||
                 text[i] == '_'; i++);
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
                instrl.push_back(Instruction(InstrCode::INSTR_ARRAY_PUSH_NUMERIC, (int)0));
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
    if (text[0] != 'f' ||
        text[1] != 'u' ||
        text[2] != 'n' ||
        text[3] != 'c' ||
        text[4] != 't' ||
        text[5] != 'i' ||
        text[6] != 'o' ||
        text[7] != 'n')
        return 0;
    int i = 8, tmp, line = instrl.size() - 1;;
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
            instrl.push_back(Instruction(InstrCode::INSTR_VALUE_VARIABLE, name.c_str()));
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
    if (text[0] != 'c' ||
        text[1] != 'l' ||
        text[2] != 'a' ||
        text[3] != 's' ||
        text[4] != 's')
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
    return i + 1;
}

int LiteScript::Syntax::ReadClassMember(const char *text, std::vector<Instruction> &instrl,
                                        Script::ErrorType &errorType) {
    int i = 0, tmp, curr;
    if ((tmp = ReadClassOperator(text, instrl, errorType)) != 0)
        return tmp;

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