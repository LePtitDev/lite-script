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