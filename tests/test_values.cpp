#include "helpers.hpp"

#include <cmath>
#include <limits>

using namespace LiteScript;

TEST_SUITE("number") {

    TEST_CASE("integral values are stored as integers") {
        CHECK(Number(5).IsInteger());
        CHECK(Number(5.0).IsInteger());
        CHECK_FALSE(Number(5.5).IsInteger());
        CHECK_FALSE(Number(1e30).IsInteger());
        CHECK_FALSE(Number(std::numeric_limits<double>::quiet_NaN()).IsInteger());
        CHECK_FALSE(Number(std::numeric_limits<double>::infinity()).IsInteger());
    }

    TEST_CASE("arithmetic") {
        CHECK(Number(2) + Number(3) == Number(5));
        CHECK((Number(2) + Number(0.5)).ToFloat() == 2.5);
        CHECK(Number(7) - Number(10) == Number(-3));
        CHECK(Number(6) * Number(7) == Number(42));
        CHECK(Number(6) / Number(2) == Number(3));
        CHECK((Number(6) / Number(2)).IsInteger());
        CHECK((Number(7) / Number(2)).ToFloat() == 3.5);
        CHECK((Number(7.0) / Number(2.5)).ToFloat() == doctest::Approx(2.8));
        CHECK(Number(7) % Number(3) == Number(1));
        CHECK(Number(-7) % Number(3) == Number(-1));
        CHECK((Number(7.5) % Number(2)).ToFloat() == doctest::Approx(1.5));
        CHECK(-Number(4) == Number(-4));
    }

    TEST_CASE("division by zero") {
        CHECK(std::isinf((Number(1) / Number(0)).ToFloat()));
        CHECK(std::isnan((Number(0) / Number(0)).ToFloat()));
        CHECK(std::isnan((Number(5) % Number(0)).ToFloat()));
    }

    TEST_CASE("integer overflows become floating numbers") {
        Number max(std::numeric_limits<std::int64_t>::max());
        Number min(std::numeric_limits<std::int64_t>::min());
        CHECK_FALSE((max + Number(1)).IsInteger());
        CHECK_FALSE((min - Number(10000)).IsInteger());
        CHECK_FALSE((max * Number(2)).IsInteger());
        CHECK_FALSE((min / Number(-1)).IsInteger());
        CHECK_FALSE((-min).IsInteger());
        CHECK(min % Number(-1) == Number(0));
        Number n = max;
        ++n;
        CHECK_FALSE(n.IsInteger());
    }

    TEST_CASE("increments keep the fractional part") {
        Number n(1.5);
        ++n;
        CHECK(n.ToFloat() == 2.5);
        CHECK((n--).ToFloat() == 2.5);
        CHECK(n.ToFloat() == 1.5);
    }

    TEST_CASE("comparisons") {
        CHECK(Number(1) < Number(2));
        CHECK(Number(1) < Number(1.5));
        CHECK(Number(2) == Number(2.0));
        CHECK(Number(2) >= Number(2));
        CHECK(Number(3) > Number(2.5));
        Number nan(std::numeric_limits<double>::quiet_NaN());
        CHECK_FALSE(nan == nan);
        CHECK_FALSE(nan < Number(1));
        CHECK_FALSE(nan >= Number(1));
    }

    TEST_CASE("binary operations use the integer value") {
        CHECK((Number(6) & Number(3)) == Number(2));
        CHECK((Number(6) | Number(3)) == Number(7));
        CHECK((Number(6) ^ Number(3)) == Number(5));
        CHECK(~Number(0) == Number(-1));
        CHECK((Number(1) << Number(4)) == Number(16));
        CHECK((Number(-16) >> Number(2)) == Number(-4));
        CHECK((Number(1) << Number(64)) == Number(1));
        CHECK((Number(5.7) & Number(7)) == Number(5));
    }

    TEST_CASE("conversion to integer saturates") {
        CHECK(Number(1e300).ToInteger() == std::numeric_limits<std::int64_t>::max());
        CHECK(Number(-1e300).ToInteger() == std::numeric_limits<std::int64_t>::min());
        CHECK(Number(std::numeric_limits<double>::quiet_NaN()).ToInteger() == 0);
        CHECK(Number(-2.9).ToInteger() == -2);
    }

    TEST_CASE("text") {
        CHECK(Number(42).ToString() == "42");
        CHECK(Number(-3).ToString() == "-3");
        CHECK(Number(2.5).ToString() == "2.5");
        CHECK(Number(0.1).ToString() == "0.1");
        CHECK(Number(std::numeric_limits<double>::quiet_NaN()).ToString() == "NaN");
        CHECK(Number(-std::numeric_limits<double>::infinity()).ToString() == "-Infinity");
    }

}

TEST_SUITE("string") {

    TEST_CASE("UTF-8 conversions") {
        std::string text = "h\xC3\xA9 \xE3\x81\x93 \xF0\x9F\x98\x80";
        String s(text);
        CHECK(s.GetLength() == 6);
        CHECK(s[1] == U'é');
        CHECK(s[3] == U'こ');
        CHECK(s[5] == U'\U0001F600');
        CHECK(s.ToUTF8() == text);
    }

    TEST_CASE("invalid UTF-8 sequences are replaced") {
        CHECK(String("a\xFF" "b").GetData() == U"a�b");
        CHECK(String("a\xC3").GetData() == U"a�");
        CHECK(String("\xE3\x41\x42").GetData() == U"�AB");
        CHECK(String::ConvertToUTF8(U"\xD800") == "\xEF\xBF\xBD");
    }

    TEST_CASE("replace") {
        String s("hello");
        s.Replace(1, 1, String("EE"));
        CHECK(s.ToUTF8() == "hEEllo");
        s.Replace(100, 1, String("!"));
        CHECK(s.ToUTF8() == "hEEllo!");
        s.Replace(0, 0, String(">"));
        CHECK(s.ToUTF8() == ">hEEllo!");
    }

    TEST_CASE("repeat") {
        CHECK(String("ab").Repeat(3).ToUTF8() == "ababab");
        CHECK(String("ab").Repeat(0).GetLength() == 0);
        CHECK(String("ab").Repeat(-2).GetLength() == 0);
        CHECK_THROWS_AS(String("ab").Repeat(std::int64_t(1) << 40), std::length_error);
    }

    TEST_CASE("comparisons") {
        CHECK(String("abc") == String("abc"));
        CHECK(String("abc") < String("abd"));
        CHECK(String("ab") < String("abc"));
        CHECK(String("b") > String("abc"));
    }

}
