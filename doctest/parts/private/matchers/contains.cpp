#include "doctest/parts/public/matchers/contains.h"

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
#include <cstring>
DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

DOCTEST_SUPPRESS_PRIVATE_WARNINGS_PUSH

namespace doctest {

Contains::Contains(const String &str)
    : string(str) {}

bool Contains::checkWith(const String &other) const {
    return std::strstr(other.c_str(), string.c_str()) != nullptr;
}

String toString(const Contains &in) {
    return "Contains( " + in.string + " )";
}

bool operator==(const String &lhs, const Contains &rhs) {
    return rhs.checkWith(lhs);
}

bool operator==(const Contains &lhs, const String &rhs) {
    return lhs.checkWith(rhs);
}

bool operator!=(const String &lhs, const Contains &rhs) {
    return !rhs.checkWith(lhs);
}

bool operator!=(const Contains &lhs, const String &rhs) {
    return !lhs.checkWith(rhs);
}

} // namespace doctest

DOCTEST_SUPPRESS_PRIVATE_WARNINGS_POP
