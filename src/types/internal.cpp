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

#include "internal.hpp"

#include "character.hpp"

#include "../types/undefined.hpp"

/**************************/
/****** CLASS NUMBER ******/
/**************************/

LiteScript::Number::Number() :
    numeric_type(0)
{
    value.integer = 0;
}
LiteScript::Number::Number(int v) :
    numeric_type(0)
{
    value.integer = v;
}
LiteScript::Number::Number(float v) {
    if (roundf(v) == v) {
        this->numeric_type = 0;
        this->value.integer = (int)v;
    }
    else {
        this->numeric_type = 1;
        this->value.flotting = v;
    }
}
LiteScript::Number::Number(const Number& number) :
    numeric_type(number.numeric_type)
{
    if (this->numeric_type == 0)
        this->value.integer = number.value.integer;
    else
        this->value.flotting = number.value.flotting;
}

LiteScript::Number::operator int() const {
    return ((this->numeric_type == 0) ? this->value.integer : (int)this->value.flotting);
}
LiteScript::Number::operator float() const {
    return ((this->numeric_type == 0) ? (float)this->value.integer : this->value.flotting);
}

LiteScript::Number::operator std::string() const {
    std::stringstream ss;
    if (this->numeric_type == 0)
        ss << this->value.integer;
    else
        ss << this->value.flotting;
    return ss.str();
}

LiteScript::Number& LiteScript::Number::operator=(const LiteScript::Number& number) {
    this->numeric_type = number.numeric_type;
    if (this->numeric_type == 0)
        this->value.integer = number.value.integer;
    else
        this->value.flotting = number.value.flotting;
    return *this;
}

LiteScript::Number LiteScript::Number::operator+() const {
    if (this->numeric_type == 0)
        return Number(this->value.integer);
    else
        return Number(this->value.flotting);
}
LiteScript::Number LiteScript::Number::operator-() const {
    if (this->numeric_type == 0)
        return Number(-this->value.integer);
    else
        return Number(-this->value.flotting);
}
LiteScript::Number& LiteScript::Number::operator++() {
    this->value.integer = (int)*this + 1;
    this->numeric_type = 0;
    return *this;
}
LiteScript::Number LiteScript::Number::operator++(int) {
    Number res((int)*this);
    this->value.integer = res.value.integer + 1;
    this->numeric_type = 0;
    return res;
}
LiteScript::Number& LiteScript::Number::operator--() {
    this->value.integer = (int)*this - 1;
    this->numeric_type = 0;
    return *this;
}
LiteScript::Number LiteScript::Number::operator--(int) {
    Number res((int)*this);
    this->value.integer = res.value.integer - 1;
    this->numeric_type = 0;
    return res;
}

LiteScript::Number LiteScript::Number::operator+(const LiteScript::Number& number) const {
    if (this->numeric_type == 0 && number.numeric_type == 0)
        return Number(this->value.integer + number.value.integer);
    else
        return Number((float)*this + (float)number);
}
LiteScript::Number LiteScript::Number::operator-(const LiteScript::Number& number) const {
    if (this->numeric_type == 0 && number.numeric_type == 0)
        return Number(this->value.integer - number.value.integer);
    else
        return Number((float)*this - (float)number);
}
LiteScript::Number LiteScript::Number::operator*(const LiteScript::Number& number) const {
    if (this->numeric_type == 0 && number.numeric_type == 0)
        return Number(this->value.integer * number.value.integer);
    else
        return Number((float)*this * (float)number);
}
LiteScript::Number LiteScript::Number::operator/(const LiteScript::Number& number) const {
    if (this->numeric_type == 0 && number.numeric_type == 0) {
        if (number.value.integer == 0)
            return Number(nanf(""));
        else
            return Number(this->value.integer / number.value.integer);
    }
    else
        return Number((float)*this / (float)number);
}
LiteScript::Number LiteScript::Number::operator%(const LiteScript::Number& number) const {
    if (this->numeric_type == 0 && number.numeric_type == 0) {
        if (number.value.integer == 0)
            return Number(nanf(""));
        else
            return Number(this->value.integer % number.value.integer);
    }
    else {
        float tmp = (float) *this / (float) number;
        return Number((tmp - truncf(tmp)) * (float) number);
    }
}

bool LiteScript::Number::operator==(const LiteScript::Number& number) const {
    if (this->numeric_type == 0 && number.numeric_type == 0)
        return (this->value.integer == number.value.integer);
    else
        return ((float)*this == (float)number);
}
bool LiteScript::Number::operator!=(const LiteScript::Number& number) const {
    if (this->numeric_type == 0 && number.numeric_type == 0)
        return (this->value.integer != number.value.integer);
    else
        return ((float)*this != (float)number);
}
bool LiteScript::Number::operator>(const LiteScript::Number& number) const {
    if (this->numeric_type == 0 && number.numeric_type == 0)
        return (this->value.integer > number.value.integer);
    else
        return ((float)*this > (float)number);
}
bool LiteScript::Number::operator<(const LiteScript::Number& number) const {
    if (this->numeric_type == 0 && number.numeric_type == 0)
        return (this->value.integer < number.value.integer);
    else
        return ((float)*this < (float)number);
}
bool LiteScript::Number::operator>=(const LiteScript::Number& number) const {
    if (this->numeric_type == 0 && number.numeric_type == 0)
        return (this->value.integer >= number.value.integer);
    else
        return ((float)*this >= (float)number);
}
bool LiteScript::Number::operator<=(const LiteScript::Number& number) const {
    if (this->numeric_type == 0 && number.numeric_type == 0)
        return (this->value.integer <= number.value.integer);
    else
        return ((float)*this <= (float)number);
}

LiteScript::Number LiteScript::Number::operator~() const {
    return Number(~(int)*this);
}
LiteScript::Number LiteScript::Number::operator&(const LiteScript::Number& number) const {
    return Number((int)*this & (int)number);
}
LiteScript::Number LiteScript::Number::operator|(const LiteScript::Number& number) const {
    return Number((int)*this | (int)number);
}
LiteScript::Number LiteScript::Number::operator^(const LiteScript::Number& number) const {
    return Number((int)*this ^ (int)number);
}
LiteScript::Number LiteScript::Number::operator<<(const LiteScript::Number& number) const {
    return Number((int)*this << (int)number);
}
LiteScript::Number LiteScript::Number::operator>>(const LiteScript::Number& number) const {
    return Number((int)*this >> (int)number);
}

LiteScript::Number& LiteScript::Number::operator+=(const LiteScript::Number& number) {
    if (this->numeric_type == 0 && number.numeric_type == 0)
        this->value.integer += number.value.integer;
    else
        *this = (float)*this + (float)number;
    return *this;
}
LiteScript::Number& LiteScript::Number::operator-=(const LiteScript::Number& number) {
    if (this->numeric_type == 0 && number.numeric_type == 0)
        this->value.integer -= number.value.integer;
    else
        *this = (float)*this - (float)number;
    return *this;
}
LiteScript::Number& LiteScript::Number::operator*=(const LiteScript::Number& number) {
    if (this->numeric_type == 0 && number.numeric_type == 0)
        this->value.integer *= number.value.integer;
    else
        *this = (float)*this * (float)number;
    return *this;
}
LiteScript::Number& LiteScript::Number::operator/=(const LiteScript::Number& number) {
    if (this->numeric_type == 0 && number.numeric_type == 0)
        this->value.integer /= number.value.integer;
    else
        *this = (float)*this / (float)number;
    return *this;
}


/**************************/
/****** CLASS STRING ******/
/**************************/

LiteScript::String::String() {}
LiteScript::String::String(char c) { this->str += (char32_t)c; }
LiteScript::String::String(char32_t c) { this->str += c; }
LiteScript::String::String(const char * data) : str(String::ConvertToUnicode(std::string(data))) {}
LiteScript::String::String(const char32_t * data) : str(data) {}
LiteScript::String::String(const std::string& data) : str(String::ConvertToUnicode(data)) {}
LiteScript::String::String(const std::u32string& data) : str(data) {}
LiteScript::String::String(const String& data) : str(data.str) {}

unsigned int LiteScript::String::GetLength() const { return this->str.size(); }
std::u32string& LiteScript::String::GetData() { return this->str; }
const std::u32string& LiteScript::String::GetData() const { return this->str; }

void LiteScript::String::Clear() { this->str.clear(); }
void LiteScript::String::Insert(unsigned int i, const LiteScript::String& data) { this->str.insert(i, data.str); }
void LiteScript::String::Erase(unsigned int i, unsigned int len) { this->str.erase(i, len); }
void LiteScript::String::Replace(unsigned int i, const LiteScript::String& data) { this->Replace(i, 1, data); }
void LiteScript::String::Replace(unsigned int i, unsigned int len, const LiteScript::String& data) {
    this->Erase(i, len);
    this->Insert(i, data);
}
LiteScript::String LiteScript::String::SubString(unsigned int i, unsigned int len) const { return String(this->str.substr(i, len)); }

LiteScript::String::operator std::string() const { return String::ConvertToUTF8(this->str); }
LiteScript::String::operator std::u32string() const { return this->str; }

std::string LiteScript::String::ConvertToUTF8(const std::u32string& str) {
    std::string res;
    for (unsigned int i = 0, sz = str.size(); i < sz; i++) {
        if (str[i] < 0x80) {
            //1 byte
            res.push_back((unsigned char)(str[i]));
        }
        else {
            if (str[i] < 0x800) {
                //2 bytes
                res.push_back((unsigned char) (0b11000000 | (str[i] >> 6)));
            }
            else {
                if (str[i] < 0x10000) {
                    //3 bytes
                    res.push_back((unsigned char) (0b11100000 | (str[i] >> 12)));
                } else {
                    //4 bytes
                    res.push_back((unsigned char) (0b11110000 | (str[i] >> 18)));
                    res.push_back((unsigned char) (0b10000000 | ((str[i] >> 12) & 0b00111111)));
                }
                res.push_back((unsigned char) (0b10000000 | ((str[i] >> 6) & 0b00111111)));
            }
            res.push_back((unsigned char) (0b10000000 | (str[i] & 0b00111111)));
        }
    }
    return res;
}
std::u32string LiteScript::String::ConvertToUnicode(const std::string& str) {
    std::u32string res;
    for (unsigned int i = 0, sz = str.size(); i < sz; i++) {
        std::bitset<4> header(((unsigned char)str[i] & (unsigned char)0b11110000) >> 4);
        if (header[3] == 0) {
            //1 byte
            res.push_back((unsigned char)str[i]);
        }
        else if (header[1] == 0) {
            //2 bytes
            if (i >= sz - 1) return res;
            res.push_back(((char32_t) ((unsigned char) str[i] & 0b00011111) << 6) |
                          (char32_t) ((unsigned char) str[i + 1] & 0b00111111));
            i += 1;
        }
        else if (header[0] == 0) {
            //3 bytes
            if (i >= sz - 2) return res;
            res.push_back(((char32_t) ((unsigned char) str[i] & 0b00001111) << 12) |
                          ((char32_t) ((unsigned char) str[i + 1] & 0b00111111) << 6) |
                          (char32_t) ((unsigned char) str[i + 2] & 0b00111111));
            i += 2;
        }
        else {
            //4 bytes
            if (i >= sz - 3) return res;
            res.push_back(((char32_t) ((unsigned char) str[i] & 0b00000111) << 18) |
                          ((char32_t) ((unsigned char) str[i + 1] & 0b00111111) << 12) |
                          ((char32_t) ((unsigned char) str[i + 2] & 0b00111111) << 6) |
                          (char32_t) ((unsigned char) str[i + 3] & 0b00111111));
            i += 3;
        }
    }
    return res;
}

LiteScript::String& LiteScript::String::operator=(const LiteScript::String& string) {
    this->str = string.str;
    return *this;
}

LiteScript::String LiteScript::String::operator+(const LiteScript::String& string) const {
    String res(*this);
    for (unsigned int i = 0, sz = string.str.size(); i < sz; i++)
        res.str += string.str[i];
    return res;
}
LiteScript::String LiteScript::String::operator*(unsigned int nb) const {
    String res;
    for (unsigned int i = 0; i < nb; i++)
        res.str += this->str;
    return res;
}

bool LiteScript::String::operator==(const LiteScript::String& string) const { return (this->str == string.str); }
bool LiteScript::String::operator!=(const LiteScript::String& string) const { return (this->str != string.str); }
bool LiteScript::String::operator>(const LiteScript::String& string) const { return (this->str > string.str); }
bool LiteScript::String::operator<(const LiteScript::String& string) const { return (this->str < string.str); }
bool LiteScript::String::operator>=(const LiteScript::String& string) const { return (this->str >= string.str); }
bool LiteScript::String::operator<=(const LiteScript::String& string) const { return (this->str <= string.str); }

LiteScript::String& LiteScript::String::operator+=(const LiteScript::String& string) {
    this->str += string.str;
    return *this;
}
LiteScript::String& LiteScript::String::operator*=(unsigned int nb) {
    std::u32string string = this->str;
    this->str.clear();
    for (unsigned int i = 0; i < nb; i++)
        this->str += string;
    return *this;
}

char32_t & LiteScript::String::operator[](unsigned int index) { return this->str[index]; }
const char32_t & LiteScript::String::operator[](unsigned int index) const { return this->str[index]; }

LiteScript::Variable LiteScript::String::GetChar(LiteScript::Memory& memory, unsigned int i) {
    Variable result = memory.Create(_type_character);
    std::allocator<Character> allocator;
    allocator.construct(&result->GetData<Character>(), *this, i);
    return result;
}

LiteScript::Variable LiteScript::String::GetMember(LiteScript::Memory& memory, const char * name) {
    if (strcmp(name, "length") == 0) {
        Variable result = memory.Create(_type_character);
        std::allocator<Character> allocator;
        allocator.construct(&result->GetData<Number>(), (int)this->str.size());
        return result;
    }
    else {
        return memory.Create(_type_undefined);
    }
}


/*****************************/
/****** CLASS CHARACTER ******/
/*****************************/

LiteScript::Character::Character(String& str, unsigned int i) : str(str), i(i) {}

LiteScript::Character::operator char32_t() const {
    return this->str[i];
}

LiteScript::String LiteScript::Character::operator=(const LiteScript::String& str) {
    this->str.Replace(this->i, str);
    return str;
}

LiteScript::String LiteScript::Character::operator+(const LiteScript::String& str) const {
    String res(this->str[this->i]);
    res += str;
    return res;
}
LiteScript::String LiteScript::Character::operator*(unsigned int nb) const {
    String res, cpy(this->str[this->i]);
    for (unsigned int j = 0; j < nb; j++)
        res += cpy;
    return res;
}

bool LiteScript::Character::operator==(const LiteScript::String& str) const {
    return (String(this->str[i]) == str);
}
bool LiteScript::Character::operator!=(const LiteScript::String& str) const {
    return (String(this->str[i]) != str);
}
bool LiteScript::Character::operator>(const LiteScript::String& str) const {
    return (String(this->str[i]) > str);
}
bool LiteScript::Character::operator<(const LiteScript::String& str) const {
    return (String(this->str[i]) < str);
}
bool LiteScript::Character::operator>=(const LiteScript::String& str) const {
    return (String(this->str[i]) >= str);
}
bool LiteScript::Character::operator<=(const LiteScript::String& str) const {
    return (String(this->str[i]) <= str);
}

LiteScript::String& LiteScript::Character::operator+=(const LiteScript::String& str) {
    this->str.Insert(this->i + 1, str);
    return this->str;
}
LiteScript::String& LiteScript::Character::operator*=(unsigned int nb) {
    String res, cpy(this->str[this->i]);
    for (unsigned int j = 0; j < nb; j++)
        res += cpy;
    this->str.Replace(this->i, res);
    return this->str;
}

/****************************/
/****** CLASS CALLBACK ******/
/****************************/

LiteScript::Callback::Callback() : state(nullptr) {}

LiteScript::Callback::Callback(const Callback &c) :
    state(c.state), intrl_idx(c.intrl_idx), line_num(c.line_num), call_ptr(c.call_ptr)
{

}

LiteScript::Callback::Callback(LiteScript::State &s, unsigned int i, unsigned int l) :
    call_ptr(nullptr), state(&s), intrl_idx(i), line_num(l)
{

}

LiteScript::Callback::Callback(State &s, Variable (* cptr)(State &, std::vector<Variable>&)) :
    state(&s), call_ptr(cptr)
{

}

bool LiteScript::Callback::isAssigned() const {
    return this->state != nullptr;
}

LiteScript::Callback & LiteScript::Callback::operator=(const Callback &c) {
    this->state = c.state;
    this->intrl_idx = c.intrl_idx;
    this->line_num = c.line_num;
    this->call_ptr = c.call_ptr;
    return *this;
}

bool LiteScript::Callback::operator==(const Callback &c) const {
    if (this->state != c.state)
        return false;
    else if (this->state == nullptr)
        return true;
    else if (this->call_ptr == nullptr)
        return (this->intrl_idx == c.intrl_idx && this->line_num == c.line_num);
    else
        return (this->call_ptr == c.call_ptr);
}

bool LiteScript::Callback::operator!=(const Callback &c) const {
    return !(*this == c);
}

LiteScript::Variable LiteScript::Callback::operator()(std::vector<Variable> &args) {
    // A COMPLETER
    return this->call_ptr(*this->state, args); // A SUPPRIMER
}

/****************************/
/****** CLASS CALLBACK ******/
/****************************/

LiteScript::VObject::VObject(Memory& mem) : memory(mem) {}

LiteScript::VObject::VObject(const VObject &o) :
    memory(o.memory), named(o.named), unamed(o.unamed)
{

}

unsigned int LiteScript::VObject::NamedCount() const {
    return this->named.size();
}

unsigned int LiteScript::VObject::UnamedCount() const {
    return this->unamed.size();
}

const char * LiteScript::VObject::GetNamedKey(unsigned int idx) const {
    if (this->named.size() <= idx)
        return nullptr;
    else
        return this->named[idx].first.c_str();
}

LiteScript::Variable LiteScript::VObject::GetNamedVariable(unsigned int idx) const {
    if (this->named.size() <= idx)
        return this->memory.Create(Type::UNDEFINED);
    else
        return Variable(this->named[idx].second);
}

int LiteScript::VObject::IndexOfNamed(const char *name) const {
    for (unsigned int i = 0, sz = this->named.size(); i < sz; i++) {
        if (this->named[i].first == name)
            return (int)i;
    }
    return -1;
}

int LiteScript::VObject::ExistNamed(const char *name) const {
    for (unsigned int i = 0, sz = this->named.size(); i < sz; i++) {
        if (this->named[i].first == name)
            return (this->named[i].second->GetType() != Type::UNDEFINED);
    }
    return false;
}

int LiteScript::VObject::ExistUnamed(unsigned int idx) const {
    if (this->unamed.size() <= idx)
        return false;
    else
        return (this->unamed[idx]->GetType() != Type::UNDEFINED);
}

LiteScript::Variable LiteScript::VObject::ConstantGet(unsigned int idx) const {
    if (this->unamed.size() <= idx)
        return this->memory.Create(Type::UNDEFINED);
    else
        return Variable(this->unamed[idx]);
}

LiteScript::Variable LiteScript::VObject::ConstantGet(const char * name) const {
    for (unsigned int i = 0, sz = this->named.size(); i < sz; i++) {
        if (this->named[i].first == name)
            return Variable(this->named[i].second);
    }
    return this->memory.Create(Type::UNDEFINED);
}

LiteScript::VObject & LiteScript::VObject::operator=(const VObject &obj) {
    this->named.clear();
    this->unamed.clear();
    this->named = obj.named;
    this->unamed = obj.unamed;
    return *this;
}

LiteScript::Variable LiteScript::VObject::operator[](unsigned int idx) {
    while (this->unamed.size() <= idx)
        this->unamed.push_back(this->memory.Create(Type::UNDEFINED));
    return Variable(this->unamed[idx]);
}

LiteScript::Variable LiteScript::VObject::operator[](const char *name) {
    for (unsigned int i = 0, sz = this->named.size(); i < sz; i++) {
        if (this->named[i].first == name)
            return Variable(this->named[i].second);
    }
    this->named.push_back({ std::string(name), this->memory.Create(Type::UNDEFINED) });
    return Variable(this->named.end()->second);
}
