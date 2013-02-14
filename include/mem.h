#ifndef MEM_INCLUDED
#define MEM_INCLUDED

#include "except.h"

BEGIN_DECLS

extern const Except_T Mem_Failed;

/* These need to be macros to pass file and line*/
#define ALLOC(nbytes) Mem_functions.alloc((nbytes), __FILE__, __LINE__)
#define CALLOC(count, nbytes) Mem_functions.calloc((count), (nbytes), __FILE__, __LINE__)
#define NEW(p) ((p) = Mem_functions.alloc((long)sizeof *(p), __FILE__, __LINE__))
#define NEW0(p) ((p) = Mem_functions.calloc(1, (long)sizeof *(p), __FILE__, __LINE__))
#define FREE(ptr) ((void)(Mem_functions.free((ptr), __FILE__, __LINE__), (ptr) = 0))
#define REALLOC(ptr, nbytes) 	((ptr) = Mem_functions.realloc((ptr), (nbytes), __FILE__, __LINE__))

typedef struct MemFuncs {
    void* (*alloc)  (size_t nbytes, const char *file, int line);
    void* (*calloc) (size_t count, size_t nbytes, const char *file, int line);
    void  (*free)   (void *ptr, const char *file, int line);
    void* (*realloc)(void *ptr, size_t nbytes, const char *file, int line);
    void  (*print_stats) ();
} MemFuncs;

extern thread_local struct MemFuncs Mem_functions;

typedef struct Arena_T* Arena_T;

extern void     Mem_print_stats();
extern MemFuncs Mem_set_functions(MemFuncs functions);
extern MemFuncs Mem_set_arena(Arena_T arena);
extern MemFuncs Mem_set_default();

extern Arena_T  Arena_new       (void);
extern void     Arena_config    (unsigned int chunks, size_t size);
extern void     Arena_dispose   (Arena_T ap);
extern void     Arena_free     (Arena_T arena);
extern void     Arena_remove_free_blocks();

extern MemFuncs Mem_set_align();

END_DECLS

#endif