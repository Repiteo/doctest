#ifndef DOCTEST_PARTS_PRIVATE_PRELUDE
#define DOCTEST_PARTS_PRIVATE_PRELUDE

#include <doctest/parts/public/version.h>
#include <doctest/parts/public/compiler.h>
#include <doctest/parts/public/warnings.h>

DOCTEST_SUPPRESS_PUBLIC_WARNINGS_PUSH

#include <doctest/parts/public/config.h>
#include <doctest/parts/public/utility.h>
#include <doctest/parts/public/platform.h>
#include <doctest/parts/public/debugger.h>
#include <doctest/parts/public/std/fwd.h>
#include <doctest/parts/public/std/type_traits.h>
#include <doctest/parts/public/std/utility.h>
#include <doctest/parts/public/string.h>
#include <doctest/parts/public/matchers/contains.h>
#include <doctest/parts/public/matchers/approx.h>
#include <doctest/parts/public/matchers/is_nan.h>
#include <doctest/parts/public/context/options.h>
#include <doctest/parts/public/assert/type.h>
#include <doctest/parts/public/assert/data.h>
#include <doctest/parts/public/assert/comparator.h>
#include <doctest/parts/public/assert/result.h>
#include <doctest/parts/public/assert/expression.h>
#include <doctest/parts/public/color.h>
#include <doctest/parts/public/subcase.h>
#include <doctest/parts/public/test_suite.h>
#include <doctest/parts/public/test_case.h>
#include <doctest/parts/public/decorators.h>
#include <doctest/parts/public/exception_translator.h>
#include <doctest/parts/public/context_scope.h>
#include <doctest/parts/public/assert/message.h>
#include <doctest/parts/public/path.h>
#include <doctest/parts/public/exceptions.h>
#include <doctest/parts/public/context.h>
#include <doctest/parts/public/assert/handler.h>
#include <doctest/parts/public/reporter.h>
#include <doctest/parts/public/macros.h>

DOCTEST_SUPPRESS_PUBLIC_WARNINGS_POP

DOCTEST_SUPPRESS_PRIVATE_WARNINGS_PUSH

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN

// required includes - will go only in one translation unit!
#include <ctime>
#include <cmath>
#include <climits>
// borland (Embarcadero) compiler requires math.h and not cmath - https://github.com/doctest/doctest/pull/37
#ifdef __BORLANDC__
#include <math.h>
#endif // __BORLANDC__
#include <new>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <utility>
#include <fstream>
#include <sstream>
#ifndef DOCTEST_CONFIG_NO_INCLUDE_IOSTREAM
#include <iostream>
#endif // DOCTEST_CONFIG_NO_INCLUDE_IOSTREAM
#include <algorithm>
#include <iomanip>
#include <vector>
#ifndef DOCTEST_CONFIG_NO_MULTITHREADING
#include <atomic>
#include <mutex>
#define DOCTEST_DECLARE_MUTEX(name) std::mutex name;
#define DOCTEST_DECLARE_STATIC_MUTEX(name) static DOCTEST_DECLARE_MUTEX(name)
#define DOCTEST_LOCK_MUTEX(name) std::lock_guard<std::mutex> DOCTEST_ANONYMOUS(DOCTEST_ANON_LOCK_)(name);
#else // DOCTEST_CONFIG_NO_MULTITHREADING
#define DOCTEST_DECLARE_MUTEX(name)
#define DOCTEST_DECLARE_STATIC_MUTEX(name)
#define DOCTEST_LOCK_MUTEX(name)
#endif // DOCTEST_CONFIG_NO_MULTITHREADING
#include <set>
#include <map>
#include <unordered_set>
#include <exception>
#include <stdexcept>
#include <csignal>
#include <cfloat>
#include <cctype>
#include <cstdint>
#include <string>

#ifdef DOCTEST_PLATFORM_MAC
#include <sys/types.h>
#include <unistd.h>
#include <sys/sysctl.h>
#endif // DOCTEST_PLATFORM_MAC

#ifdef DOCTEST_PLATFORM_WINDOWS

// defines for a leaner windows.h
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define DOCTEST_UNDEF_WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#define DOCTEST_UNDEF_NOMINMAX
#endif // NOMINMAX

// not sure what AfxWin.h is for - here I do what Catch does
#ifdef __AFXDLL
#include <AfxWin.h>
#else
#include <windows.h>
#endif
#include <io.h>

#ifdef DOCTEST_UNDEF_WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#undef DOCTEST_UNDEF_WIN32_LEAN_AND_MEAN
#endif // DOCTEST_UNDEF_WIN32_LEAN_AND_MEAN
#ifdef DOCTEST_UNDEF_NOMINMAX
#undef NOMINMAX
#undef DOCTEST_UNDEF_NOMINMAX
#endif // DOCTEST_UNDEF_NOMINMAX

#else // DOCTEST_PLATFORM_WINDOWS

#include <sys/time.h>
#include <unistd.h>

#endif // DOCTEST_PLATFORM_WINDOWS

// this is a fix for https://github.com/doctest/doctest/issues/348
// https://mail.gnome.org/archives/xml/2012-January/msg00000.html
#if !defined(HAVE_UNISTD_H) && !defined(STDOUT_FILENO)
#define STDOUT_FILENO fileno(stdout)
#endif // HAVE_UNISTD_H

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

// counts the number of elements in a C array
#define DOCTEST_COUNTOF(x) (sizeof(x) / sizeof(x[0]))

#ifdef DOCTEST_CONFIG_DISABLE
#define DOCTEST_BRANCH_ON_DISABLED(if_disabled, if_not_disabled) if_disabled
#else // DOCTEST_CONFIG_DISABLE
#define DOCTEST_BRANCH_ON_DISABLED(if_disabled, if_not_disabled) if_not_disabled
#endif // DOCTEST_CONFIG_DISABLE

#ifndef DOCTEST_THREAD_LOCAL
#if defined(DOCTEST_CONFIG_NO_MULTITHREADING) || DOCTEST_MSVC && (DOCTEST_MSVC < DOCTEST_COMPILER(19, 0, 0))
#define DOCTEST_THREAD_LOCAL
#else // DOCTEST_MSVC
#define DOCTEST_THREAD_LOCAL thread_local
#endif // DOCTEST_MSVC
#endif // DOCTEST_THREAD_LOCAL

#ifndef DOCTEST_MULTI_LANE_ATOMICS_THREAD_LANES
#define DOCTEST_MULTI_LANE_ATOMICS_THREAD_LANES 32
#endif

#ifndef DOCTEST_MULTI_LANE_ATOMICS_CACHE_LINE_SIZE
#define DOCTEST_MULTI_LANE_ATOMICS_CACHE_LINE_SIZE 64
#endif

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
#define DOCTEST_CONFIG_NO_MULTI_LANE_ATOMICS
#endif

#ifndef DOCTEST_CDECL
#define DOCTEST_CDECL __cdecl
#endif

DOCTEST_SUPPRESS_PRIVATE_WARNINGS_POP

#endif // DOCTEST_PARTS_PRIVATE_PRELUDE
