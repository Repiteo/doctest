#include "doctest/parts/public/matchers/is_nan.h"
#include "doctest/parts/public/string.h"

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
#include <cmath>
DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

DOCTEST_SUPPRESS_PRIVATE_WARNINGS_PUSH

namespace doctest {

DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(4738)
template <typename F>
IsNaN<F>::operator bool() const {
    return std::isnan(value) ^ flipped;
}
DOCTEST_MSVC_SUPPRESS_WARNING_POP
template struct DOCTEST_INTERFACE_DEF IsNaN<float>;
template struct DOCTEST_INTERFACE_DEF IsNaN<double>;
template struct DOCTEST_INTERFACE_DEF IsNaN<long double>;

template <typename F>
String toString(IsNaN<F> in) {
    return String(in.flipped ? "! " : "") + "IsNaN( " + doctest::toString(in.value) + " )";
}

String toString(IsNaN<float> in) {
    return toString<float>(in);
}

String toString(IsNaN<double> in) {
    return toString<double>(in);
}

String toString(IsNaN<double long> in) {
    return toString<double long>(in);
}

} // namespace doctest

DOCTEST_SUPPRESS_PRIVATE_WARNINGS_POP
