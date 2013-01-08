#ifndef PORTABLE_INCLUDED
#define PORTABLE_INCLUDED

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


#endif /* PORTABLE_INCLUDED */
