#pragma once

#include "doctest/parts/public/config.h"
#include "doctest/parts/public/warnings.h"
#include "doctest/parts/public/assert/type.h"

DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wc++98-compat-pedantic")

#ifndef DOCTEST_CONFIG_DISABLE

namespace doctest {
namespace detail {

  struct DOCTEST_INTERFACE TestFailureException
  {
  };

  DOCTEST_INTERFACE bool checkIfShouldThrow(assertType::Enum at);

#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS
  DOCTEST_NORETURN
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS
  DOCTEST_INTERFACE void throwException();

} // namespace detail
} // namespace doctest

#endif // DOCTEST_CONFIG_DISABLE

DOCTEST_CLANG_SUPPRESS_WARNING_POP
