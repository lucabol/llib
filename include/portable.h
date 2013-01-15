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

#endif /* PORTABLE_INCLUDED */
