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

unsigned int LiteScript::Syntax::LineBegin(const char *begin, unsigned int current) {
    int i;
    for (i = (int)current - 1; i >= 0 && begin[i] != '\n'; i--);
    return (unsigned int)i + 1;
}

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

int LiteScript::Syntax::ReadString(const char *text, std::string &string, unsigned int &lb) {
    if (text[0] != '"' && text[0] != '\'')
        return 0;
    string.clear();
    char end_char = text[0];
    int i;
    for (i = 1; text[i] != end_char; i++) {
        if (text[i] == '\n')
            lb++;
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

unsigned int LiteScript::Syntax::ReadComment(const char *text, unsigned int& lb) {
    if (text[0] != '/')
        return 0;
    unsigned int i;
    if (text[1] == '/') {
        for (i = 2; text[i] != '\n' && text[i] != '\0'; i++);
        return i;
    }
    else if (text[1] == '*') {
        for (i = 2; text[i] != '\0'; i++) {
            if (text[i] == '\n')
                lb++;
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

unsigned int LiteScript::Syntax::ReadWhitespace(const char *text, unsigned int &lb) {
    unsigned int i = 0, tmp;
    while (true) {
        if (text[i] == ' ' || text[i] == '\t' || text[i] == '\n')
            i++;
        else if ((tmp = ReadComment(text, lb)) > 0)
            i += tmp;
        else
            break;
    }
    return i;
}

int LiteScript::Syntax::ReadArray(const char *text, std::vector<Instruction> &instrl, unsigned int &lb,
                                  Script::ErrorType &errorType) {
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
            if ((tmp.ui = ReadWhitespace(text + i, lb)) > 0)
                i += (int)tmp.ui;
            else if (index == 0 && (tmp.i = ReadExpression(text + i, instrl, lb, errorType)) > 0) {
                i += tmp.i;
                instrl.push_back(Instruction(InstrCode::INSTR_ARRAY_PUSH_NUMERIC, (int)0));
                index++;
            }
            else if (index > 0 && text[i] == ',' && (tmp.i = ReadExpression(text + i + 1, instrl, lb, errorType)) > 0) {
                i += tmp.i + 1;
                instrl.push_back(Instruction(InstrCode::INSTR_ARRAY_PUSH_NUMERIC, (int)index++));
            }
            else {
                if (tmp.i != 0)
                    return tmp.i - i;
                else {
                    errorType = Script::ErrorType::SCRPT_ERROR_VALUE_SIMPLEARRAY_ENDEXPECTED;
                    return -i;
                }
            }
        }
        return i + 1;
    }
    else if (text[0] == '{') {
        // \{ ({name}:{expr}(,{name}:{expr})*)? \}
        instrl.push_back(Instruction(InstrCode::INSTR_VALUE_ARRAY));
        while (text[i] != '}') {
            if ((tmp.ui = ReadWhitespace(text, lb)) > 0)
                i += (int)tmp.ui;
            ////// A COMPLETER //////
        }
        return i + 1;
    }
    else {
        return 0;
    }
}