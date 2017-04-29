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

#ifndef LITESCRIPT_LITESCRIPT_HPP
#include "../litescript.hpp"

#else

#ifndef LITESCRIPT_TYPES_INTERNAL_HPP
#define LITESCRIPT_TYPES_INTERNAL_HPP

namespace LiteScript {


    ///////////////////////////////
    ///////// TYPE NUMBER /////////
    ///////////////////////////////


    // Content of a number
    class Number {

        ///////////////////////////////
        ////// OBJECT ATTRIBUTES //////
        ///////////////////////////////

        // Indicate if the number is represented by an integer (0) or a float (1)
        char numeric_type;

        // Representations union of the number
        union {
            // Integer representation
            int integer;
            // Floating representation
            float flotting;
        } value;

    public:

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor of a number
         * - Integer representation
         * - Value equal to zero
         */
        Number();

        /**
         * Constructor of a number by an integer
         *
         * @param value The integer value to assign
         */
        Number(int value);

        /**
         * Constructor of a number by a float
         *
         * @param value The float value to assign
         */
        Number(float value);

        /**
         * Constructor by copy
         *
         * @param number The number to be copied
         */
        Number(const Number& number);

        ///////////////////////////////////
        ////// OPERATORS OVERLOADING //////
        ///////////////////////////////////

        /**
         * Get an integer representation of the number
         */
        operator int() const;

        /**
         * Get a floating representation of the number
         */
        operator float() const;

        /**
         * Get a string representation of the number
         */
        operator std::string() const;

        /**
         * Assign the number by an other number
         *
         * @param number The other number
         * @return This number
         */
        Number& operator=(const Number& number);

        ////// ARITHMETIC OPERATIONS //////

        /**
         * Apply unary plus operation
         *
         * @return The result of operation
         */
        Number operator+() const;

        /**
         * Apply unary minus operation
         *
         * @return The result of operation
         */
        Number operator-() const;

        /**
         * Apply pre-increment operation
         *
         * @return This number
         */
        Number& operator++();

        /**
         * Apply post-increment operation
         *
         * @return The result of operation
         */
        Number operator++(int);

        /**
         * Apply pre-decrement operation
         *
         * @return This number
         */
        Number& operator--();

        /**
         * Apply post-decrement operation
         *
         * @return The result of operation
         */
        Number operator--(int);


        /**
         * Apply addition operation
         *
         * @param number The second operand number
         * @return The result of operation
         */
        Number operator+(const Number& number) const;

        /**
         * Apply substraction operation
         *
         * @param number The second operand number
         * @return The result of operation
         */
        Number operator-(const Number& number) const;

        /**
         * Apply multiplication operation
         *
         * @param number The second operand number
         * @return The result of operation
         */
        Number operator*(const Number& number) const;

        /**
         * Apply division operation
         *
         * @param number The second operand number
         * @return The result of operation
         */
        Number operator/(const Number& number) const;

        /**
         * Apply modulo operation
         *
         * @param number The second operand number
         * @return The result of operation
         */
        Number operator%(const Number& number) const;

        ////// COMPARISON OPERATIONS //////

        /**
         * Apply equality comparison
         *
         * @param number The second operand number
         * @return The result of operation
         */
        bool operator==(const Number& number) const;

        /**
         * Apply inequality comparison
         *
         * @param number The second operand number
         * @return The result of operation
         */
        bool operator!=(const Number& number) const;

        /**
         * Apply superiority comparison
         *
         * @param number The second operand number
         * @return The result of operation
         */
        bool operator>(const Number& number) const;

        /**
         * Apply inferiority comparison
         *
         * @param number The second operand number
         * @return The result of operation
         */
        bool operator<(const Number& number) const;

        /**
         * Apply superiority or equality comparison
         *
         * @param number The second operand number
         * @return The result of operation
         */
        bool operator>=(const Number& number) const;

        /**
         * Apply inferiority or equality comparison
         *
         * @param number The second operand number
         * @return The result of operation
         */
        bool operator<=(const Number& number) const;

        ////// BINARY OPERATIONS //////

        /**
         * Apply binary not operation
         *
         * @return The result of operation
         */
        Number operator~() const;

        /**
         * Apply binary and operation
         *
         * @param number The second operand number
         * @return The result of operation
         */
        Number operator&(const Number& number) const;

        /**
         * Apply binary or operation
         *
         * @param number The second operand number
         * @return The result of operation
         */
        Number operator|(const Number& number) const;

        /**
         * Apply binary xor operation
         *
         * @param number The second operand number
         * @return The result of operation
         */
        Number operator^(const Number& number) const;

        /**
         * Apply left shift operation
         *
         * @param number The second operand number
         * @return The result of operation
         */
        Number operator<<(const Number& number) const;

        /**
         * Apply right shift operation
         *
         * @param number The second operand number
         * @return The result of operation
         */
        Number operator>>(const Number& number) const;

        ////// DIRECT MODIFICATION OPERATIONS //////

        /**
         * Apply addition and assignation operation
         *
         * @param number The second operand number
         * @return This number
         */
        Number& operator+=(const Number& number);

        /**
         * Apply substraction and assignation operation
         *
         * @param number The second operand number
         * @return This number
         */
        Number& operator-=(const Number& number);

        /**
         * Apply multiplication and assignation operation
         *
         * @param number The second operand number
         * @return This number
         */
        Number& operator*=(const Number& number);

        /**
         * Apply division and assignation operation
         *
         * @param number The second operand number
         * @return This number
         */
        Number& operator/=(const Number& number);

    };


    ///////////////////////////////
    ///////// TYPE STRING /////////
    ///////////////////////////////


    // Content of a string
    class String {

        ///////////////////////////////
        ////// OBJECT ATTRIBUTES //////
        ///////////////////////////////

        // Datas content of the string coded in UNICODE
        std::u32string str;

    public:

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor of a string
         */
        String();

        /**
         * Constructor by assign a character in ASCII
         *
         * @param c The caracter in ASCII to assign
         */
        String(char c);

        /**
         * Constructor by assign a character in UNICODE
         *
         * @param c The caracter in UNICODE to assign
         */
        String(char32_t c);

        /**
         * Constructor by assign a string in UTF-8
         *
         * @param str The string in UTF-8 to assign
         */
        String(const char * str);

        /**
         * Constructor by assign a string in UNICODE
         *
         * @param str The string in UNICODE to assign
         */
        String(const char32_t * str);

        /**
         * Constructor by assign a string in UTF-8
         *
         * @param str The string in UTF-8 to assign
         */
        String(const std::string& str);

        /**
         * Constructor by assign a string in UNICODE
         *
         * @param str The string in UNICODE to assign
         */
        String(const std::u32string& str);

        /**
         * Constructor by copy
         *
         * @param str The string to be copied
         */
        String(const String& str);

        /////////////////////
        ////// METHODS //////
        /////////////////////

        /**
         * Get the length of the string (the '\0' character is not counted)
         * (Warning: it's not the datas size, if you want the length in bytes, you need to multiply by 4 the return)
         *
         * @return The length of the string
         */
        unsigned int GetLength() const;

        /**
         * Get the datas in UNICODE
         *
         * @return The datas in UNICODE
         */
        std::u32string& GetData();

        /**
         * Get the datas in UNICODE (constant)
         *
         * @return The datas in UNICODE (constant)
         */
        const std::u32string& GetData() const;

        /**
         * Remove the content of the string
         */
        void Clear();

        /**
         * Insert the content of the second string inside this string
         *
         * @param index The index where the string need to be placed
         * @param str The second string
         */
        void Insert(unsigned int index, const String& str);

        /**
         * Erase the content of the content of the string from the index indicated
         * and the number of characters indicated
         *
         * @param index The index from which the string should be erased
         * @param length The number of characters to be erased
         */
        void Erase(unsigned int index, unsigned int length = 1);

        /**
         * Erase the character indicated and insert the content of the second string in its place
         *
         * @param index The index of the character
         * @param str The second string
         */
        void Replace(unsigned int index, const String& str);

        /**
         * Erase a sequende indicated and insert the content of the second string in its place
         *
         * @param index The index of the sequence's begin
         * @param length The number of characters to be replaced
         * @param str The second string
         */
        void Replace(unsigned int index, unsigned int length, const String& str);

        /**
         * Get the sequence indicated in the string
         *
         * @param index The index of the sequence's begin
         * @param length The number of characters in the sequence
         * @return The sequence as a string
         */
        String SubString(unsigned int index, unsigned int length) const;

        ///////////////////////////////////
        ////// OPERATORS OVERLOADING //////
        ///////////////////////////////////

        /**
         * Convert the content to a string in UTF-8 format
         */
        operator std::string() const;

        /**
         * Convert the content to a string in UNICODE format
         */
        operator std::u32string() const;

        /**
         * Convert a string in UNICODE format to a string in UTF-8 format
         *
         * @param str The string to be converted
         * @return The string in UTF-8 format
         */
        static std::string ConvertToUTF8(const std::u32string& str);

        /**
         * Convert a string in UTF-8 format to a string in UNICODE format
         *
         * @param str The string to be converted
         * @return The string in UNICODE format
         */
        static std::u32string ConvertToUnicode(const std::string&);

        /**
         * Assign the string by an other string
         *
         * @param str The other string
         * @return This string
         */
        String& operator=(const String& str);

        ////// ARITHMETIC OPERATIONS //////

        /**
         * Apply addition operation
         *
         * @param str The second operand string
         * @return The result of the operation
         */
        String operator+(const String& str) const;

        /**
         * Apply multiplication operation
         *
         * @param x The second operand integer
         * @return The result of the operation
         */
        String operator*(unsigned int x) const;

        ////// COMPARISON OPERATIONS //////

        /**
         * Apply equality comparison
         *
         * @param str The second operand string
         * @return The result of operation
         */
        bool operator==(const String& str) const;

        /**
         * Apply inequality comparison
         *
         * @param str The second operand string
         * @return The result of operation
         */
        bool operator!=(const String& str) const;

        /**
         * Apply superiority comparison
         *
         * @param str The second operand string
         * @return The result of operation
         */
        bool operator>(const String& str) const;

        /**
         * Apply inferiority comparison
         *
         * @param str The second operand string
         * @return The result of operation
         */
        bool operator<(const String& str) const;

        /**
         * Apply superiority or equality comparison
         *
         * @param str The second operand string
         * @return The result of operation
         */
        bool operator>=(const String& str) const;

        /**
         * Apply inferiority or equality comparison
         *
         * @param str The second operand string
         * @return The result of operation
         */
        bool operator<=(const String& str) const;

        ////// DIRECT MODIFICATION OPERATIONS //////

        /**
         * Apply addition and assignation operation
         *
         * @param str The second operand string
         * @return This string
         */
        String& operator+=(const String& str);

        /**
         * Apply addition and assignation operation
         *
         * @param str The second operand integer
         * @return This string
         */
        String& operator*=(unsigned int x);

        ////// ACCESS OPERATIONS //////

        /**
         * Get the character at the position indicated
         *
         * @param index The index of the character
         * @return The character reference
         */
        char32_t& operator[](unsigned int index);

        /**
         * Get the character at the position indicated (constant)
         *
         * @param index The index of the character
         * @return The character reference (constant)
         */
        const char32_t& operator[](unsigned int index) const;

    };


    //////////////////////////////////
    ///////// TYPE CHARACTER /////////
    //////////////////////////////////


    // Content of a character
    class Character {

        ////////////////////////
        ////// ATTRIBUTES //////
        ////////////////////////

        // The object which contain the string
        Object& obj;

        // The referenced string
        String& str;

        // The index of the character in the string
        unsigned int i;

    public:

        /////////////////////////
        ////// CONSTRUCTOR //////
        /////////////////////////

        /**
         * Basic constructor of a character (only for the string type class)
         *
         * @param object The object that contain the string
         * @param index The index of the character in the string
         */
        Character(Object& object, unsigned int index);

        ///////////////////////////////////
        ////// OPERATORS OVERLOADING //////
        ///////////////////////////////////

        /**
         * Convert the content to a UNICODE character
         */
        operator char32_t() const;

        /**
         * Assign the character by a string
         *
         * @param str The other string
         * @return This string
         */
        String operator=(const String& str);

        ////// ARITHMETIC OPERATIONS //////

        /**
         * Apply addition operation
         *
         * @param str The second operand string
         * @return The result of the operation
         */
        String operator+(const String& str) const;

        /**
         * Apply multiplication operation
         *
         * @param x The second operand integer
         * @return The result of the operation
         */
        String operator*(unsigned int x) const;

        ////// COMPARISON OPERATIONS //////

        /**
         * Apply equality comparison
         *
         * @param str The second operand string
         * @return The result of operation
         */
        bool operator==(const String& str) const;

        /**
         * Apply inequality comparison
         *
         * @param str The second operand string
         * @return The result of operation
         */
        bool operator!=(const String& str) const;

        /**
         * Apply superiority comparison
         *
         * @param str The second operand string
         * @return The result of operation
         */
        bool operator>(const String& str) const;

        /**
         * Apply inferiority comparison
         *
         * @param str The second operand string
         * @return The result of operation
         */
        bool operator<(const String& str) const;

        /**
         * Apply superiority or equality comparison
         *
         * @param str The second operand string
         * @return The result of operation
         */
        bool operator>=(const String& str) const;

        /**
         * Apply inferiority or equality comparison
         *
         * @param str The second operand string
         * @return The result of operation
         */
        bool operator<=(const String& str) const;

        ////// DIRECT MODIFICATION OPERATIONS //////

        /**
         * Apply addition and assignation operation
         *
         * @param str The second operand string
         * @return The string of the character
         */
        Object& operator+=(const String& str);

        /**
         * Apply multiplication and assignation operation
         *
         * @param str The second operand integer
         * @return The string of the character
         */
        Object& operator*=(unsigned int x);

    };

}

#endif //LITESCRIPT_INTERNAL_HPP

#endif
