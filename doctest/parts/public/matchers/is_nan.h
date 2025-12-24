#pragma once

#include "doctest/parts/public/string.h"

DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wc++98-compat-pedantic")

namespace doctest {

template <typename F>
struct DOCTEST_INTERFACE_DECL IsNaN
{
    F value; bool flipped;
    IsNaN(F f, bool flip = false) : value(f), flipped(flip) { }
    IsNaN<F> operator!() const { return { value, !flipped }; }
    operator bool() const;
};

#ifndef __MINGW32__
extern template struct DOCTEST_INTERFACE_DECL IsNaN<float>;
extern template struct DOCTEST_INTERFACE_DECL IsNaN<double>;
extern template struct DOCTEST_INTERFACE_DECL IsNaN<long double>;
#endif

DOCTEST_INTERFACE String toString(IsNaN<float> in);
DOCTEST_INTERFACE String toString(IsNaN<double> in);
DOCTEST_INTERFACE String toString(IsNaN<double long> in);

} // namespace doctest

DOCTEST_CLANG_SUPPRESS_WARNING_POP
