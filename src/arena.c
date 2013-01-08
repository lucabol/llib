#include "_config.h"

#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "except.h"
#include "arena.h"

#define T Arena_T

const Except_T Arena_NewFailed = { "Arena Creation Failed" };
const Except_T Arena_Failed    = { "Arena Allocation Failed" };

#define THRESHOLD 10
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
    T arena = malloc(sizeof (*arena));
    if (arena == NULL)
        RAISE(Arena_NewFailed);

    arena->prev = NULL;
    arena->limit = arena->avail = NULL;
    return arena;
}

void Arena_dispose(T *ap) {
    assert(ap && *ap);
    Arena_free(*ap);

    free(*ap);
    *ap = NULL;
}

void *Arena_alloc(T arena, long nbytes, const char *file, int line) {
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
        } else {
            long m = sizeof (union header) + nbytes + 10*1024;
            ptr = malloc(m);
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
    return arena->avail - nbytes + RESERVED_SIZE; /* luca */
}

void *Arena_calloc(T arena, long count, long nbytes, const char *file, int line) {
    void *ptr;
    assert(count > 0);

    ptr = Arena_alloc(arena, count*nbytes, file, line);
    memset(ptr, '\0', count*nbytes);

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
        p   = Arena_alloc(arena, nbytes, file, line);
        *BLOCK_START(p) = nbytes;
        return p;
    }

    bsize = *BLOCK_START(ptr);

#ifndef ALWAYS_REALLOC
    if(nbytes <= bsize) {
        *BLOCK_START(ptr) = bsize;
        return ptr;
    }
#endif

    p               = Arena_alloc(arena, nbytes, file, line); /* allocate the requested size */
    *BLOCK_START(p) = nbytes;

    bsize           = bsize < nbytes ? bsize : nbytes;
    memcpy(p, ptr, bsize); /* memcopy the minimum of the two, this is why we are keeping track of size */

    return p;
}

void Arena_free(T arena) {
    assert(arena);

    while (arena->prev) {
        struct T tmp = *arena->prev;

        if (nfree < THRESHOLD) {
            arena->prev->prev = freechunks;
            freechunks = arena->prev;
            nfree++;
            freechunks->limit = arena->limit;
        } else
            free(arena->prev);
        *arena = tmp;
    }

    assert(arena->limit == NULL);
    assert(arena->avail == NULL);
}
