#ifndef DOCTEST_PARTS_PRIVATE_SIGNALS
#define DOCTEST_PARTS_PRIVATE_SIGNALS

#include "doctest/parts/public/config.h"

DOCTEST_SUPPRESS_PRIVATE_WARNINGS_PUSH

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
#include <exception>
DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

#ifndef DOCTEST_CDECL
#define DOCTEST_CDECL __cdecl
#endif

/// BEGIN TEMP
DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
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
DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END
/// END TEMP

#ifndef DOCTEST_CONFIG_DISABLE

namespace doctest {
namespace detail {

#if !defined(DOCTEST_CONFIG_POSIX_SIGNALS) && !defined(DOCTEST_CONFIG_WINDOWS_SEH)
struct FatalConditionHandler {
    static void reset();
    static void allocateAltStackMem();
    static void freeAltStackMem();
};
#else // DOCTEST_CONFIG_POSIX_SIGNALS || DOCTEST_CONFIG_WINDOWS_SEH

#ifdef DOCTEST_PLATFORM_WINDOWS

struct SignalDefs {
    DWORD id;
    const char *name;
};

struct FatalConditionHandler {
    static LONG CALLBACK handleException(PEXCEPTION_POINTERS ExceptionInfo);
    static void allocateAltStackMem();
    static void freeAltStackMem();

    FatalConditionHandler();

    static void reset();

    ~FatalConditionHandler();

private:
    static UINT prev_error_mode_1;
    static int prev_error_mode_2;
    static unsigned int prev_abort_behavior;
    static int prev_report_mode;
    static _HFILE prev_report_file;
    static void(DOCTEST_CDECL *prev_sigabrt_handler)(int);
    static std::terminate_handler original_terminate_handler;
    static bool isSet;
    static ULONG guaranteeSize;
    static LPTOP_LEVEL_EXCEPTION_FILTER previousTop;
};

#else // DOCTEST_PLATFORM_WINDOWS

struct SignalDefs {
    int id;
    const char *name;
};

struct FatalConditionHandler {
    static bool isSet;
    static struct sigaction oldSigActions[6];
    static stack_t oldSigStack;
    static size_t altStackSize;
    static char *altStackMem;

    static void handleSignal(int sig);

    static void allocateAltStackMem();

    static void freeAltStackMem();

    FatalConditionHandler();

    ~FatalConditionHandler();
    static void reset();
};

#endif // DOCTEST_PLATFORM_WINDOWS
#endif // DOCTEST_CONFIG_POSIX_SIGNALS || DOCTEST_CONFIG_WINDOWS_SEH

} // namespace detail
} // namespace doctest

#endif // DOCTEST_CONFIG_DISABLE

DOCTEST_SUPPRESS_PRIVATE_WARNINGS_POP

#endif // DOCTEST_PARTS_PRIVATE_SIGNALS
