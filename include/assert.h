#undef assert

#include "utils.h"      /* for begin_decl*/
#include "portable.h"   /* for BREAKPOINT */ 

#include "except.h"     /* for raise */

BEGIN_DECLS

#ifdef REMOVE_ASSERTS
# define assert(e) ((void)0)
#elif defined(NDEBUG)
#  define assert(e) ((void)((e)||(RAISE(Assert_Failed),0)))
# else
#  define assert(e) ((void)((e)||(BREAKPOINT,0)))
# endif /* REMOVE_ASSERTS */

#ifndef NDEBUG
# define trace_log(...) fprint(stderr, __VA_ARGS__) 
#else
# define trace_log(...) 
#endif

END_DECLS
