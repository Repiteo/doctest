#ifndef DOCTEST_PARTS_PRIVATE_TIMER
#define DOCTEST_PARTS_PRIVATE_TIMER

#include "doctest/parts/public/config.h"

DOCTEST_SUPPRESS_PRIVATE_WARNINGS_PUSH

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
#include <cstdint>
DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

#ifndef DOCTEST_CONFIG_DISABLE

namespace doctest {
namespace detail {

using ticks_t = std::uint64_t;

ticks_t getCurrentTicks();

struct Timer {
    void start();
    unsigned int getElapsedMicroseconds() const;
    double getElapsedSeconds() const;

private:
    ticks_t m_ticks = 0;
};

} // namespace detail
} // namespace doctest

#endif // DOCTEST_CONFIG_DISABLE

DOCTEST_SUPPRESS_PRIVATE_WARNINGS_POP

#endif // DOCTEST_PARTS_PRIVATE_TIMER
