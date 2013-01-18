#ifndef _MEM_INCLUDED
#define _MEM_INCLUDED

#include "except.h"

BEGIN_DECLS

extern const Except_T Mem_Failed;

extern void *Mem_alloc (long nbytes, const char *file, int line);
extern void *Mem_calloc(long count, long nbytes, const char *file, int line);
extern void Mem_free(void *ptr, const char *file, int line);
extern void *Mem_realloc(void *ptr, long nbytes, const char *file, int line);
extern void Mem_print_allocated();

END_DECLS

#endif
