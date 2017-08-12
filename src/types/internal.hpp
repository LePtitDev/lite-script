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

#ifndef LITESCRIPT_TYPES_INTERNAL_HPP
#define LITESCRIPT_TYPES_INTERNAL_HPP

#include "../memory/variable.hpp"
#include "../memory/memory.hpp"

#include "../script/state.hpp"

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
         * @return The character
         */
        char32_t& operator[](unsigned int index);

        /**
         * Get the character at the position indicated (constant)
         *
         * @param index The index of the character
         * @return The character (constant)
         */
        const char32_t& operator[](unsigned int index) const;

        /**
         * Get the object character at the position indicated
         *
         * @param memory The main memory
         * @param index The index of the character
         * @return The variable that contain the character
         */
        Variable GetChar(Memory& memory, unsigned int index);

        /**
         * Get the object member with the specified name
         *
         * @param memory The main memory
         * @param name The member's name
         * @return The member variable if exist and an undefined object otherwise
         */
        Variable GetMember(Memory& memory, const char * name);

    };


    //////////////////////////////////
    ///////// TYPE CHARACTER /////////
    //////////////////////////////////


    // Content of a character
    class Character {

        ////////////////////////
        ////// ATTRIBUTES //////
        ////////////////////////

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
        Character(String& str, unsigned int index);

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
        String& operator+=(const String& str);

        /**
         * Apply multiplication and assignation operation
         *
         * @param str The second operand integer
         * @return The string of the character
         */
        String& operator*=(unsigned int x);

    };

    // Content of callback
    class Callback {

        ////////////////////////
        ////// ATTRIBUTES //////
        ////////////////////////

        // The instructions list index
        unsigned int intrl_idx;

        // The line number of the callback beginning
        unsigned int line_num;

        // The internal callback
        Variable (* call_ptr)(State&, std::vector<Variable>&);

        // The callback namespace
        Nullable<Namer> nsp;

    public:

        // The main memory
        Memory& memory;

        // The current this
        Nullable<Variable> This;

        ///////////////////////
        ////// ACCESSORS //////
        ///////////////////////

        // Public getter on the instructions list index
        const unsigned int& I;

        // Public getter on the instruction line number
        const unsigned int& L;

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor
         *
         * @param mem The main memory
         */
        Callback(Memory& mem);

        /**
         * Constructor for script callback
         *
         * @param s The state of the script
         * @param i The instructions list index in the state
         * @param l The line number of the callback beginning
         */
        Callback(State& s, unsigned int i, unsigned int l);

        /**
         * Constructor for internal callback
         *
         * @param mem The main memory
         * @param cptr The internal fonction pointer
         */
        Callback(Memory& mem, Variable (* cptr)(State&, std::vector<Variable>&));

        /**
         * Copy constructor
         */
        Callback(const Callback& c);

        /////////////////////
        ////// METHODS //////
        /////////////////////

        /**
         * Indicate if the callback is internal
         */
        bool isInternal() const;

        // The assign operator
        Callback& operator=(const Callback& c);

        // The equal comparison
        bool operator==(const Callback& c) const;

        // The not equal comparison
        bool operator!=(const Callback& c) const;

        // The calling operator
        Variable operator()(State& state, std::vector<Variable>& args);

    };

    // Content of an array value
    class Array {

        ////////////////////////
        ////// ATTRIBUTES //////
        ////////////////////////

        // Named members
        std::vector<std::pair<std::string, Variable>> named;

        // Unamed members
        std::vector<Variable> unamed;

    public:

        // The main memory
        Memory& memory;

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor of a value object
         *
         * @param mem The main memory
         */
        Array(Memory& mem);

        /**
         * Copy constructor
         */
        Array(const Array& o);

        /////////////////////
        ////// METHODS //////
        /////////////////////

        /**
         * Get the count of named members
         */
        unsigned int NamedCount() const;

        /**
         * Get the count of unamed members
         */
        unsigned int UnamedCount() const;

        /**
         * Get the name/key of a named member
         *
         * @param idx The index of the named member in the vector
         */
        const char * GetNamedKey(unsigned int idx) const;

        /**
         * Get the variable of a named member
         *
         * @param idx The index of the names member in the vector
         */
        Variable GetNamedVariable(unsigned int idx) const;

        /**
         * Find the index of the named member exists
         *
         * @param name The name of the member
         * @return The index if exists and -1 otherwise
         */
        int IndexOfNamed(const char * name) const;

        /**
         * Indicate if the named member exists
         *
         * @param name The name of the member
         */
        int ExistNamed(const char * name) const;

        /**
         * Indicate if the unamed member exists
         *
         * @param idx The index of the member
         */
        int ExistUnamed(unsigned int idx) const;

        /**
         * The access operator for unamed members without creation
         *
         * @param idx The index of the member
         */
        Variable ConstantGet(unsigned int idx) const;

        /**
         * The access operator for named members without creation
         *
         * @param name The name of the member
         */
        Variable ConstantGet(const char * name) const;

        // The assign operator
        Array& operator=(const Array& obj);

        // The access operator for unamed members (create it if not exist)
        Variable operator[](unsigned int idx);

        // The access operator for named members (create it if not exist)
        Variable operator[](const char * name);

    };

    // Content of a class object
    class Class {

    public:

        // The operator type
        enum OperatorType {
            OP_TYPE_ASSIGN,

            OP_TYPE_UNARY_PLUS,
            OP_TYPE_UNARY_MINUS,
            OP_TYPE_PRE_INCR,
            OP_TYPE_POST_INCR,
            OP_TYPE_PRE_DECR,
            OP_TYPE_POST_DECR,

            OP_TYPE_ADD,
            OP_TYPE_SUB,
            OP_TYPE_MUL,
            OP_TYPE_DIV,
            OP_TYPE_MOD,

            OP_TYPE_EQU,
            OP_TYPE_DIF,
            OP_TYPE_GREAT,
            OP_TYPE_LESS,
            OP_TYPE_GREAT_EQU,
            OP_TYPE_LESS_EQU,

            OP_TYPE_LOG_NOT,
            OP_TYPE_LOG_AND,
            OP_TYPE_LOG_OR,

            OP_TYPE_BIT_NOT,
            OP_TYPE_BIT_AND,
            OP_TYPE_BIT_OR,
            OP_TYPE_BIT_XOR,
            OP_TYPE_LSHIFT,
            OP_TYPE_RSHIFT,

            OP_TYPE_ADD_ASSIGN,
            OP_TYPE_SUB_ASSIGN,
            OP_TYPE_MUL_ASSIGN,
            OP_TYPE_DIV_ASSIGN,

            OP_TYPE_ARRAY,
            OP_TYPE_CALL,

            OP_TYPE_NUMBER
        };

    private:

        ////////////////////////
        ////// ATTRIBUTES //////
        ////////////////////////

        // Classes inherited
        std::vector<Variable> inherit;

        // Static members
        std::vector<std::pair<std::string, Variable>> s_members;

        // Unstatic members
        std::vector<std::pair<std::string, Variable>> us_members;

        // Operators overloading
        std::array<Nullable<Variable>, OperatorType::OP_TYPE_NUMBER> op_members;

        // The index of constructor callback
        int constructor_index;

    public:

        Memory& memory;

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor of a class
         */
        Class(Memory& memory);

        /**
         * Copy constructor
         */
        Class(const Class& c);

        /////////////////////
        ////// METHODS //////
        /////////////////////

        /**
         * Inherit the class by a parent
         *
         * @param v The parent class
         */
        void Inherit(const Variable& v);

        /**
         * Define the constructor
         *
         * @param name The member function name (need to exists)
         * @return true if success and false otherwise
         */
        bool DefineConstructor(const char * name);

        /**
         * Add a static member
         *
         * @param name The name of the member
         * @param v The variable member
         */
        void AddStatic(const char * name, const Variable& v);

        /**
         * Add an unstatic member
         *
         * @param name The name of the member
         * @param v The variable member
         */
        void AddUnstatic(const char * name, const Variable& v);

        /**
         * Add an unstatic member
         *
         * @param op The operator type
         * @param v The variable member
         */
        void AddOperator(OperatorType op, const Variable& v);

        /**
         * Add an unstatic member
         *
         * @param op The operator type
         * @param v The variable member
         * @return true if success
         */
        void AddOperator(unsigned int op, const Variable& v);

        /**
         * Get the static member
         *
         * @param name The name of the member
         */
        Variable GetStaticMember(const char * name) const;

        /**
         * Get the unstatic member
         *
         * @param name The name of the member
         */
        Variable GetUnstaticMember(const char * name) const;

        /**
         * Get the operator member
         *
         * @param op The operator type
         */
        Variable GetOperator(Class::OperatorType op) const;

        /**
         * Get the operator member
         *
         * @param op The operator type
         */
        Variable GetOperator(unsigned int op) const;

        /**
         * Create an element by this class
         *
         * @param state The script state
         * @param args Constructor arguments
         */
        Variable CreateElement(State& state, std::vector<Variable>& args);

        /**
         * Get the class inherits
         */
        const std::vector<Variable>& GetInherits() const;

        /**
         * Get the static members count
         */
        unsigned int GetStaticCount() const;

        /**
         * Get the unstatic members count
         */
        unsigned int GetUnstaticCount() const;

        /**
         * Get the static member name
         *
         * @param i The index of the member
         */
        const char * GetStaticName(unsigned int i) const;

        /**
         * Get the unstatic member name
         *
         * @param i The index of the member
         */
        const char * GetUnstaticName(unsigned int i) const;

        /**
         * Get the static member
         *
         * @param i The index of the member
         */
        Variable GetStaticMember(unsigned int i) const;

        /**
         * Get the unstatic member
         *
         * @param i The index of the member
         */
        Variable GetUnstaticMember(unsigned int i) const;

        /**
         * Get the index of the constructor
         */
        int GetConstructorIndex() const;

        // The assign operator
        Class& operator=(const Class& c);

        // The equal comparison operator
        bool operator==(const Class& c) const;

        // The not equal comparison operator
        bool operator!=(const Class& c) const;

    };

    // Content of class element
    class ClassObject {

        ////////////////////////
        ////// ATTRIBUTES //////
        ////////////////////////

        // The member list
        std::vector<std::pair<std::string, Variable>> members;

    public:

        // The base class
        Nullable<Class> ClassBase;

        // The script state
        Nullable<State> ScriptState;

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor
         */
        ClassObject();

        /**
         * Copy constructor
         */
        ClassObject(const ClassObject& c);

        /////////////////////
        ////// METHODS //////
        /////////////////////

        /**
         * Add a member in the object
         *
         * @param name The name of member
         * @param v The variable of member
         */
        void AddMember(const char * name, const Variable& v);

        /**
         * Get the members count
         */
        unsigned int GetMemberCount() const;

        /**
         * Get the member name at a specified index
         *
         * @param index The index of member
         */
        const char * GetMemberName(unsigned int index) const;

        /**
         * Get the member variable at a specified index
         *
         * @param index The index of member
         */
        Variable GetMemberVariable(unsigned int index) const;

        /**
         * Get a member by its name
         *
         * @param name The name of the member
         */
        Variable GetMember(const char * name);

    };

    // Content of a namespace object
    class Namespace {

        // The variables list
        std::vector<std::pair<std::string, Variable>> vars;

    public:

        // The main memory
        Memory& memory;

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor of a namespace
         */
        Namespace(Memory& mem);

        /**
         * Copy constructor
         */
        Namespace(const Namespace& nsp);

        /////////////////////
        ////// METHODS //////
        /////////////////////

        /**
         * Define a variable in the namespace
         *
         * @param name The name of the variable
         * @return true if success
         */
        bool DefineVariable(const char * name);

        /**
         * Define a variable in the namespace
         *
         * @param name The name of the variable
         * @param v The pre-exist variable
         * @return true if success
         */
        bool DefineVariable(const char * name, const Variable& v);

        /**
         * Get the variable count in the namespace
         */
        unsigned int Count() const;

        /**
         * Get the name/key of the variable
         *
         * @param idx The index of the variable
         */
        const char * GetKey(unsigned int idx) const;

        /**
         * Get a variable
         *
         * @param idx The index of the variable
         */
        Variable GetVariable(unsigned int idx) const;

        /**
         * Get the index of a variable by its name
         *
         * @param name The name of the variable
         * @return The index if exists and -1 otherwise
         */
        int IndexOf(const char * name) const;;

        // The access operator
        Variable operator[](const char * name) const;

    };

}

#endif //LITESCRIPT_INTERNAL_HPP