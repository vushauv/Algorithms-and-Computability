#include "check.hpp"
#include "value.hpp"
#include "tests.hpp"

#include <string>

void test_number() {
    // Parsing of valid numbers.
    CHECK(parse_value<Value>("0").status == ParseStatus::Ok);
    CHECK(parse_value<Value>("0").value == 0);
    CHECK(parse_value<Value>("42").value == 42);
    CHECK(parse_value<Value>("0007").value == 7);
    CHECK(parse_value<Value>("000000000000000000000000000000000000000000001").value == 1);  // 45 characters

    const std::string max38(38, '9');  // 10^38 - 1
    CHECK(parse_value<Value>(max38).status == ParseStatus::Ok);
    CHECK(parse_value<Value>(max38).value == kNativeLimit - 1);

    // Rejected input.
    CHECK(parse_value<Value>("").status == ParseStatus::Empty);
    CHECK(parse_value<Value>("-5").status == ParseStatus::NotDigit);
    CHECK(parse_value<Value>("1a").status == ParseStatus::NotDigit);
    CHECK(parse_value<Value>("+1").status == ParseStatus::NotDigit);
    CHECK(parse_value<Value>("1 ").status == ParseStatus::NotDigit);
    CHECK(parse_value<Value>("1" + std::string(38, '0')).status == ParseStatus::TooLarge);  // 10^38
    CHECK(parse_value<Value>(std::string(60, '9')).status == ParseStatus::TooLarge);
    CHECK(parse_value<Value>(std::string(60, '9') + "x").status == ParseStatus::NotDigit);

    // Printing, and parsing back.
    CHECK(to_string(0) == "0");
    CHECK(to_string(1234567890) == "1234567890");
    CHECK(to_string(kNativeLimit - 1) == max38);
    const std::string big = "3141592653589793238462643";  // from AaC_example_2.txt
    CHECK(to_string(parse_value<Value>(big).value) == big);
    const Value all_ones = ~Value{0};  // 2^128 - 1
    CHECK(to_string(all_ones) == "340282366920938463463374607431768211455");

    // Checked addition.
    CHECK(checked_add(2, 3) == Value{5});
    CHECK(checked_add(all_ones, 0) == all_ones);
    CHECK(!checked_add(all_ones, 1).has_value());
    CHECK(!checked_add(all_ones / 2 + 1, all_ones / 2 + 1).has_value());
}
