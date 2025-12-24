#pragma once

#include "doctest/parts/private/prelude.h"

DOCTEST_SUPPRESS_PRIVATE_WARNINGS_PUSH

#ifndef DOCTEST_CONFIG_DISABLE

namespace doctest {
namespace detail {

    // all the registered tests
    std::set<TestCase>& getRegisteredTests() {
        static std::set<TestCase> data;
        return data;
    }

} // namespace detail
} // namespace doctest

#endif // DOCTEST_CONFIG_DISABLE

DOCTEST_SUPPRESS_PRIVATE_WARNINGS_POP
