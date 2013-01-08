#undef assert

#include "utils.h"
#include "portable.h"

#include "except.h"

BEGIN_DECLS

#ifdef REMOVE_ASSERTS

#define assert(e) ((void)0)

#else

# ifdef NDEBUG

#  define assert(e) ((void)((e)||(RAISE(Assert_Failed),0)))

# else

#  define assert(e) ((void)((e)||(BREAKPOINT,0)))

# endif

END_DECLS

#endif

