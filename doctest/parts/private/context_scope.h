#pragma once

#include "doctest/parts/private/prelude.h"

DOCTEST_SUPPRESS_PRIVATE_WARNINGS_PUSH


#ifndef DOCTEST_CONFIG_DISABLE

namespace doctest {
namespace detail {
    extern DOCTEST_THREAD_LOCAL std::vector<IContextScope*> g_infoContexts; // for logging with INFO()
}
} // namespace doctest

#endif // DOCTEST_CONFIG_DISABLE

DOCTEST_SUPPRESS_PRIVATE_WARNINGS_POP
