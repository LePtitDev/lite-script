#ifndef LITESCRIPT_NUMBER_HPP
#define LITESCRIPT_NUMBER_HPP

#include <cstdint>
#include <string>
#include <variant>

namespace LiteScript {

    // Content of a number: a 64 bits integer or a double precision float
    // Integer operations stay integer when the result is an integer (6 / 2 == 3, 5 / 2 == 2.5),
    // the other operations are done in floating point
    class Number {

        // The representation of the number
        std::variant<std::int64_t, double> value;

    public:

        /**
         * Zero constructor
         */
        constexpr Number() noexcept : value(std::int64_t{ 0 }) {}

        /**
         * Integer constructor
         */
        constexpr Number(int v) noexcept : value(std::int64_t{ v }) {}

        /**
         * Integer constructor
         */
        constexpr Number(std::int64_t v) noexcept : value(v) {}

        /**
         * Floating constructor (an integral value is stored as an integer)
         */
        Number(double v) noexcept;

        /**
         * Indicate if the number is represented by an integer
         */
        bool IsInteger() const noexcept { return std::holds_alternative<std::int64_t>(this->value); }

        /**
         * Return the number as an integer (truncated, saturated, NaN gives 0)
         */
        std::int64_t ToInteger() const noexcept;

        /**
         * Return the number as a floating number
         */
        double ToFloat() const noexcept;

        /**
         * Return the shortest text that represents the number
         */
        std::string ToString() const;

        ////// ARITHMETIC OPERATIONS //////

        Number operator+() const noexcept { return *this; }
        Number operator-() const noexcept;
        Number& operator++() noexcept;
        Number operator++(int) noexcept;
        Number& operator--() noexcept;
        Number operator--(int) noexcept;
        Number operator+(const Number& number) const noexcept;
        Number operator-(const Number& number) const noexcept;
        Number operator*(const Number& number) const noexcept;
        Number operator/(const Number& number) const noexcept;
        Number operator%(const Number& number) const noexcept;
        Number& operator+=(const Number& number) noexcept { return *this = *this + number; }
        Number& operator-=(const Number& number) noexcept { return *this = *this - number; }
        Number& operator*=(const Number& number) noexcept { return *this = *this * number; }
        Number& operator/=(const Number& number) noexcept { return *this = *this / number; }

        ////// COMPARISON OPERATIONS //////

        bool operator==(const Number& number) const noexcept;
        bool operator<(const Number& number) const noexcept;
        bool operator!=(const Number& number) const noexcept { return !(*this == number); }
        bool operator>(const Number& number) const noexcept { return number < *this; }
        bool operator<=(const Number& number) const noexcept { return *this < number || *this == number; }
        bool operator>=(const Number& number) const noexcept { return number < *this || *this == number; }

        ////// BINARY OPERATIONS (on the integer representation) //////

        Number operator~() const noexcept;
        Number operator&(const Number& number) const noexcept;
        Number operator|(const Number& number) const noexcept;
        Number operator^(const Number& number) const noexcept;
        Number operator<<(const Number& number) const noexcept;
        Number operator>>(const Number& number) const noexcept;

    };

}

#endif //LITESCRIPT_NUMBER_HPP
