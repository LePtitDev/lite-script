#ifndef LITESCRIPT_LITESCRIPT_HPP
#include "../litescript.hpp"

#else

#ifndef LITESCRIPT_TYPES_INTERNAL_HPP
#define LITESCRIPT_TYPES_INTERNAL_HPP

namespace LiteScript {

    class Number {

        char numeric_type;
        union {
            int integer;
            float flotting;
        } value;

    public:

        Number();
        Number(int);
        Number(float);
        Number(const Number&);

        operator int() const;
        operator float() const;

        operator std::string() const;

        Number& operator=(const Number&);

        Number operator+() const;
        Number operator-() const;
        Number& operator++();
        Number operator++(int);
        Number& operator--();
        Number operator--(int);

        Number operator+(const Number&) const;
        Number operator-(const Number&) const;
        Number operator*(const Number&) const;
        Number operator/(const Number&) const;
        Number operator%(const Number&) const;

        bool operator==(const Number&) const;
        bool operator!=(const Number&) const;
        bool operator>(const Number&) const;
        bool operator<(const Number&) const;
        bool operator>=(const Number&) const;
        bool operator<=(const Number&) const;

        Number operator~() const;
        Number operator&(const Number&) const;
        Number operator|(const Number&) const;
        Number operator^(const Number&) const;
        Number operator<<(const Number&) const;
        Number operator>>(const Number&) const;

        Number& operator+=(const Number&);
        Number& operator-=(const Number&);
        Number& operator*=(const Number&);
        Number& operator/=(const Number&);

    };

    class String {

        std::u32string str;

    public:

        String();
        String(char);
        String(char32_t);
        String(const char *);
        String(const char32_t *);
        String(const std::string&);
        String(const std::u32string&);
        String(const String&);

        unsigned int GetLength() const;
        std::u32string& GetData();

        void Clear();
        void Insert(unsigned int, const String&);
        void Erase(unsigned int, unsigned int = 1);
        void Replace(unsigned int, const String&);
        void Replace(unsigned int, unsigned int, const String&);
        String SubString(unsigned int, unsigned int) const;

        operator std::string() const;
        operator std::u32string() const;

        static std::string ConvertToUTF8(const std::u32string&);
        static std::u32string ConvertToUnicode(const std::string&);

        String& operator=(const String&);

        String operator+(const String&) const;
        String operator*(unsigned int) const;

        bool operator==(const String&) const;
        bool operator!=(const String&) const;
        bool operator>(const String&) const;
        bool operator<(const String&) const;
        bool operator>=(const String&) const;
        bool operator<=(const String&) const;

        String& operator+=(const String&);
        String& operator*=(unsigned int);

        char32_t& operator[](unsigned int);
        const char32_t& operator[](unsigned int) const;

    };

    class Character {

        Object& obj;
        String& str;
        unsigned int i;

    public:

        Character(Object&, unsigned int);

        operator char32_t() const;

        String operator=(const String&);

        String operator+(const String&) const;
        String operator*(unsigned int) const;

        bool operator==(const String&) const;
        bool operator!=(const String&) const;
        bool operator>(const String&) const;
        bool operator<(const String&) const;
        bool operator>=(const String&) const;
        bool operator<=(const String&) const;

        Object& operator+=(const String&);
        Object& operator*=(unsigned int);

    };

}

#endif //LITESCRIPT_INTERNAL_HPP

#endif
