#ifndef PORTABLE_INCLUDED
#define PORTABLE_INCLUDED

#ifdef __GNUC__
#define MAXALIGN 8 /* Alignment used in the allocation functions */
#endif

#ifdef __GNUC__
#define inline inline
#else
#define inline
#endif

/* Define thread_local for different gcc and msvc */
#ifdef __GNUC__
# define thread_local   __thread
#else
# ifdef _MSC_VER
#   define thread_local    __declspec(thread)
# else
#   define thread_local
# endif
#endif

/* Portable breakpoint expression (not statement) */
#if defined (_MSC_VER)
# define BREAKPOINT        __debugbreak()
#elif defined (__GNUC__)
# define BREAKPOINT __builtin_trap()
#else
# include <signal.h>
# define BREAKPOINT        (raise (SIGTRAP))
#endif

#ifdef NATIVE_EXCEPTIONS
extern void Except_hook_signal();
#endif

#ifdef _MSC_VER
#include <malloc.h>

#define Aligned_malloc  _aligned_malloc
#define Aligned_realloc _aligned_realloc
#define Aligned_free    _aligned_free
#elif __GNUC__
#include <stdlib.h>
#include <malloc.h>
#include <windows.h>

#define Aligned_malloc  __mingw_aligned_malloc
#define Aligned_realloc __mingw_aligned_realloc
#define Aligned_free    __mingw_aligned_free
#else
#define Aligned_free    free
#endif

# ifdef _MSC_VER
#   define inline
# else
# endif

#if defined(_MSC_VER)
  #define P_SIZE_T    "%Iu"
  #define P_SSIZE_T   "%Id"
  #define P_PTRDIFF_T "%Id"
#elif defined(__GNUC__)
  #define PR_SIZE_T    "%zu"
  #define PR_SSIZE_T   "%zd"
  #define PR_PTRDIFF_T "%zd"
#else
  #define PR_SIZE_T    "%zu"
  #define PR_SSIZE_T   "%zd"
  #define PR_PTRDIFF_T "%zd"
#endif

#endif /* PORTABLE_INCLUDED */
