#include <litescript/number.hpp>

#include "charconv.hpp"

#include <charconv>
#include <cmath>
#include <limits>

namespace {

    using Int = std::int64_t;

    constexpr Int INT_MIN_VALUE = std::numeric_limits<Int>::min();
    constexpr Int INT_MAX_VALUE = std::numeric_limits<Int>::max();

    // 2^63 as a double (first value that doesn't fit in an int64)
    constexpr double INT_LIMIT = 9223372036854775808.0;

    bool AddOverflow(Int a, Int b, Int& result) {
        if ((b > 0 && a > INT_MAX_VALUE - b) || (b < 0 && a < INT_MIN_VALUE - b))
            return true;
        result = a + b;
        return false;
    }

    bool SubOverflow(Int a, Int b, Int& result) {
        if ((b < 0 && a > INT_MAX_VALUE + b) || (b > 0 && a < INT_MIN_VALUE + b))
            return true;
        result = a - b;
        return false;
    }

    bool MulOverflow(Int a, Int b, Int& result) {
        if (a == 0 || b == 0) {
            result = 0;
            return false;
        }
        if ((a == -1 && b == INT_MIN_VALUE) || (b == -1 && a == INT_MIN_VALUE))
            return true;
        Int r = static_cast<Int>(static_cast<std::uint64_t>(a) * static_cast<std::uint64_t>(b));
        if (r / b != a)
            return true;
        result = r;
        return false;
    }

}

LiteScript::Number::Number(double v) noexcept {
    if (std::isfinite(v) && std::trunc(v) == v && v >= -INT_LIMIT && v < INT_LIMIT)
        this->value = static_cast<Int>(v);
    else
        this->value = v;
}

std::int64_t LiteScript::Number::ToInteger() const noexcept {
    if (const Int* i = std::get_if<Int>(&this->value))
        return *i;
    double d = std::get<double>(this->value);
    if (std::isnan(d))
        return 0;
    if (d >= INT_LIMIT)
        return INT_MAX_VALUE;
    if (d < -INT_LIMIT)
        return INT_MIN_VALUE;
    return static_cast<Int>(d);
}

double LiteScript::Number::ToFloat() const noexcept {
    if (const Int* i = std::get_if<Int>(&this->value))
        return static_cast<double>(*i);
    return std::get<double>(this->value);
}

std::string LiteScript::Number::ToString() const {
    if (const Int* i = std::get_if<Int>(&this->value))
        return std::to_string(*i);
    double d = std::get<double>(this->value);
    if (std::isnan(d))
        return "NaN";
    if (std::isinf(d))
        return d > 0 ? "Infinity" : "-Infinity";
    return Internal::FormatDouble(d);
}

LiteScript::Number LiteScript::Number::operator-() const noexcept {
    if (const Int* i = std::get_if<Int>(&this->value)) {
        if (*i == INT_MIN_VALUE)
            return Number(-static_cast<double>(*i));
        return Number(-*i);
    }
    return Number(-std::get<double>(this->value));
}

LiteScript::Number& LiteScript::Number::operator++() noexcept {
    return *this = *this + Number(1);
}

LiteScript::Number LiteScript::Number::operator++(int) noexcept {
    Number previous(*this);
    ++*this;
    return previous;
}

LiteScript::Number& LiteScript::Number::operator--() noexcept {
    return *this = *this - Number(1);
}

LiteScript::Number LiteScript::Number::operator--(int) noexcept {
    Number previous(*this);
    --*this;
    return previous;
}

LiteScript::Number LiteScript::Number::operator+(const Number& number) const noexcept {
    Int result;
    if (this->IsInteger() && number.IsInteger() &&
        !AddOverflow(std::get<Int>(this->value), std::get<Int>(number.value), result))
        return Number(result);
    return Number(this->ToFloat() + number.ToFloat());
}

LiteScript::Number LiteScript::Number::operator-(const Number& number) const noexcept {
    Int result;
    if (this->IsInteger() && number.IsInteger() &&
        !SubOverflow(std::get<Int>(this->value), std::get<Int>(number.value), result))
        return Number(result);
    return Number(this->ToFloat() - number.ToFloat());
}

LiteScript::Number LiteScript::Number::operator*(const Number& number) const noexcept {
    Int result;
    if (this->IsInteger() && number.IsInteger() &&
        !MulOverflow(std::get<Int>(this->value), std::get<Int>(number.value), result))
        return Number(result);
    return Number(this->ToFloat() * number.ToFloat());
}

LiteScript::Number LiteScript::Number::operator/(const Number& number) const noexcept {
    if (this->IsInteger() && number.IsInteger()) {
        Int a = std::get<Int>(this->value), b = std::get<Int>(number.value);
        // The result stays an integer when the division is exact
        if (b != 0 && !(a == INT_MIN_VALUE && b == -1) && a % b == 0)
            return Number(a / b);
    }
    return Number(this->ToFloat() / number.ToFloat());
}

LiteScript::Number LiteScript::Number::operator%(const Number& number) const noexcept {
    if (this->IsInteger() && number.IsInteger()) {
        Int a = std::get<Int>(this->value), b = std::get<Int>(number.value);
        if (b == 0)
            return Number(std::numeric_limits<double>::quiet_NaN());
        if (b == -1)
            return Number(0);
        return Number(a % b);
    }
    return Number(std::fmod(this->ToFloat(), number.ToFloat()));
}

bool LiteScript::Number::operator==(const Number& number) const noexcept {
    if (this->IsInteger() && number.IsInteger())
        return std::get<Int>(this->value) == std::get<Int>(number.value);
    return this->ToFloat() == number.ToFloat();
}

bool LiteScript::Number::operator<(const Number& number) const noexcept {
    if (this->IsInteger() && number.IsInteger())
        return std::get<Int>(this->value) < std::get<Int>(number.value);
    return this->ToFloat() < number.ToFloat();
}

LiteScript::Number LiteScript::Number::operator~() const noexcept {
    return Number(~this->ToInteger());
}

LiteScript::Number LiteScript::Number::operator&(const Number& number) const noexcept {
    return Number(this->ToInteger() & number.ToInteger());
}

LiteScript::Number LiteScript::Number::operator|(const Number& number) const noexcept {
    return Number(this->ToInteger() | number.ToInteger());
}

LiteScript::Number LiteScript::Number::operator^(const Number& number) const noexcept {
    return Number(this->ToInteger() ^ number.ToInteger());
}

LiteScript::Number LiteScript::Number::operator<<(const Number& number) const noexcept {
    auto shift = static_cast<unsigned int>(number.ToInteger() & 63);
    return Number(static_cast<Int>(static_cast<std::uint64_t>(this->ToInteger()) << shift));
}

LiteScript::Number LiteScript::Number::operator>>(const Number& number) const noexcept {
    auto shift = static_cast<unsigned int>(number.ToInteger() & 63);
    return Number(this->ToInteger() >> shift);
}
