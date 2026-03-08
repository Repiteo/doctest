#ifndef DOCTEST_PARTS_PRIVATE_FILTERS
#define DOCTEST_PARTS_PRIVATE_FILTERS

#include "doctest/parts/public/string.h"

DOCTEST_SUPPRESS_PRIVATE_WARNINGS_PUSH

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
#include <vector>
DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

#ifndef DOCTEST_CONFIG_DISABLE

namespace doctest {
namespace detail {

// matching of a string against a wildcard mask (case sensitivity configurable) taken from
// https://www.codeproject.com/Articles/1088/Wildcard-string-compare-globbing
int wildcmp(const char *str, const char *wild, bool caseSensitive);

// checks if the name matches any of the filters (and can be configured what to do when empty)
bool matchesAny(const char *name, const std::vector<String> &filters, bool matchEmpty, bool caseSensitive);

} // namespace detail
} // namespace doctest

#endif // DOCTEST_CONFIG_DISABLE

DOCTEST_SUPPRESS_PRIVATE_WARNINGS_POP

#endif // DOCTEST_PARTS_PRIVATE_FILTERS
