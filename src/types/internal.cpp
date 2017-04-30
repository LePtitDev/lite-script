#include "internal.hpp"

#include "character.hpp"

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

unsigned int LiteScript::String::GetLength() const { return (unsigned int)this->str.size(); }
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
    for (unsigned int i = 0, sz = (unsigned int)str.size(); i < sz; i++) {
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
    for (unsigned int i = 0, sz = (unsigned int)str.size(); i < sz; i++) {
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
    for (unsigned int i = 0, sz = (unsigned int)string.str.size(); i < sz; i++)
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

LiteScript::Object& LiteScript::String::GetChar(unsigned int i) {
    this->tmp_obj.Reassign(_type_character, sizeof(Character));
    ObjectAllocator.construct(&this->tmp_obj.GetData<Character>(), *this, i);
    return this->tmp_obj;
}

LiteScript::Object& LiteScript::String::GetMember(const char * name) {
    if (strcmp(name, "length") == 0) {
        this->tmp_obj.Reassign(Type::NUMBER, sizeof(Number));
        ObjectAllocator.construct(&this->tmp_obj.GetData<Number>(), (int)this->str.size());
        return this->tmp_obj;
    }
    else {
        return Object::UNDEFINED;
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
