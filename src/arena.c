#include <stdlib.h> /* for malloc */
#include <string.h> /* for memcpy */

#include "assert.h"
#include "except.h"
#include "arena.h"
#include "mem.h"
#include "log.h"

#define T Arena_T

const Except_T Arena_NewFailed = { "Arena Creation Failed" };
const Except_T Arena_Failed    = { "Arena Allocation Failed" };

#ifdef NDEBUG
#define THRESHOLD 10
#else
#define THRESHOLD 0 /* In debug mode don't reuse memory blocks, so you can detect leaks */
#endif

#define RESERVED_SIZE sizeof(long) /* luca to store the size of the allocated memory */
#define BLOCK_START(ptr) ((long*)(((char*)ptr) - RESERVED_SIZE))

struct T {
    T prev;
    char *avail;
    char *limit;
};

union align {
#ifdef MAXALIGN
    char pad[MAXALIGN];
#else
    int i;
    long l;
    long *lp;
    void *p;
    void (*fp)(void);
    float f;
    double d;
    long double ld;
#endif
};

union header {
    struct T b;
    union align a;
};

static T freechunks;
static int nfree;

T Arena_new(void) {
    T arena;

    NEW(arena);
    arena->prev = NULL;
    arena->limit = arena->avail = NULL;
    log_dbg("%p new arena", arena);
    return arena;
}

void Arena_dispose(T ap) {
    assert(ap);
    log_dbg("%p dispose arena", ap);

    Arena_free(ap);

    FREE(ap);
    ap = NULL;
}

void *Arena_alloc(T arena, long nbytes, const char *file, int line) {
    void* ptr;
    assert(arena);
    assert(nbytes > 0);

    nbytes = ((nbytes + sizeof (union align) - 1)/ (sizeof (union align)))*(sizeof (union align));
    nbytes = nbytes + RESERVED_SIZE; /* luca */

    while (nbytes > arena->limit - arena->avail) {
        T ptr;
        char *limit;

        if ((ptr = freechunks) != NULL) {
            freechunks = freechunks->prev;
            nfree--;
            limit = ptr->limit;
            log_dbg("%p arena reusing chunk, left %i chunks", freechunks, nfree);
        } else {
            long m = sizeof (union header) + nbytes + 10*1024;
            ptr = ALLOC(m);
            log_dbg("%p arena alloc new chunk %li", ptr, m);

            if (ptr == NULL) {
                if (file == NULL)
                    RAISE(Arena_Failed);
                else
                    Except_raise(&Arena_Failed, file, line);
            }
            limit = (char *)ptr + m;
        }

        *ptr = *arena;
        arena->avail = (char *)((union header *)ptr + 1);
        arena->limit = limit;
        arena->prev  = ptr;
    }

    *((long*)arena->avail) = nbytes; /* luca storing the size */
    arena->avail += nbytes;
    ptr = arena->avail - nbytes + RESERVED_SIZE; /* luca */

    log_dbg("%p arena %li bytes", ptr, nbytes);
    return ptr;
}

void *Arena_calloc(T arena, long count, long nbytes, const char *file, int line) {
    void *ptr;
    assert(count > 0);

    ptr = Arena_alloc(arena, count*nbytes, file, line);
    memset(ptr, '\0', count*nbytes);

    log_dbg("%p arena %li bytes", ptr, nbytes);
    return ptr;
}

void *Arena_realloc  (T arena, void *ptr, long nbytes, const char *file, int line) {
    long bsize;
    void* p;
    assert(arena);
    assert(nbytes > 0);

    if(!nbytes) /* doesn't set ptr to NULL as it is not required */
        return NULL;

    if(!ptr) { /* if ptr is null just alloc */
        p               = Arena_alloc(arena, nbytes, file, line);
        *BLOCK_START(p) = nbytes;
        return p;
    }

    bsize = *BLOCK_START(ptr);

#ifdef NDEBUG
    if(nbytes <= bsize) {
        *BLOCK_START(ptr) = bsize;
        return ptr;
    }
#endif

    p               = Arena_alloc(arena, nbytes, file, line); /* allocate the requested size */
    *BLOCK_START(p) = nbytes;

    bsize           = bsize < nbytes ? bsize : nbytes;
    memcpy(p, ptr, bsize); /* memcopy the minimum of the two, this is why we are keeping track of size */

    log_dbg("%p arena %li bytes", p, nbytes);
    return p;
}

void Arena_free(T arena) {
    assert(arena);
    log_dbg("%p arena freed", arena);

    while (arena->prev) {
        struct T tmp = *arena->prev;

        if (nfree < THRESHOLD) {
            arena->prev->prev = freechunks;
            freechunks = arena->prev;
            nfree++;
            freechunks->limit = arena->limit;
        } else
            FREE(arena->prev);

        *arena = tmp;
    }

    assert(arena->limit == NULL);
    assert(arena->avail == NULL);
}
