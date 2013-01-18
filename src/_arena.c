/*
TODO: increase performance, keep track of the biggest freeblock and use it as new one
TODO: decrease memory usage, look in existing chunks to find one that has enough space
*/

#include <stdlib.h> /* for malloc */
#include <string.h> /* for memcpy */

#include "assert.h"
#include "except.h"
#include "log.h"

#include "portable.h" /* for thread_local*/
#include "_arena.h"
#include "_mem.h"

#define T Arena_T

const Except_T Arena_NewFailed = { "Arena Creation Failed" };
const Except_T Arena_Failed    = { "Arena Allocation Failed" };

static thread_local int  Arena_chunk_n           = 10;
static thread_local long Arena_additional_size   = 10 * 1024;

/* for stats */
#ifndef NDEBUG
static thread_local unsigned long chunk_allocations = 0;
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

void Arena_config (int chunks, long size) {
    assert(chunks > 0);
    assert(size > 0);

    Arena_chunk_n           = chunks;
    Arena_additional_size   = size;
}

T Arena_new(void) {
    T arena;

    arena           = _Mem_alloc(sizeof(arena), __FILE__, __LINE__);
    arena->prev     = NULL;
    arena->limit    = arena->avail = NULL;
    log_dbg("%p new arena", arena);
    return arena;
}

void Arena_dispose(T ap) {
    assert(ap);
    log_dbg("%p dispose arena", ap);

    Arena_free(ap);

    _Mem_free(ap, __FILE__, __LINE__);
    ap = NULL;
}

void *Arena_alloc(T arena, long nbytes, const char *file, int line) {
    void* ptr;
    assert(arena);
    assert(nbytes > 0);

    /* A space of nbytes + RESERVED_SIZE or more bytes that is a multiple of sizeof (union align) */
    nbytes = ((nbytes + RESERVED_SIZE + sizeof (union align) - 1)/ (sizeof (union align)))*(sizeof (union align));

    while (nbytes > arena->limit - arena->avail) {
        T ptr;
        char *limit;

        if ((ptr = freechunks) != NULL) {
            freechunks = freechunks->prev;
            nfree--;
            limit = ptr->limit;
            log_dbg("%p arena reusing chunk, left %i chunks", freechunks, nfree);
        } else {
            long m = sizeof (union header) + nbytes + Arena_additional_size;
            ptr = _Mem_alloc(m, __FILE__, __LINE__);

#ifndef NDEBUG
            chunk_allocations = chunk_allocations + 1;
#endif
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

#ifdef NDEBUG /* Optimized for the case when the requested block fits into existing one*/
    if(nbytes <= bsize) {
        *BLOCK_START(ptr) = nbytes;
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

        if (nfree < Arena_chunk_n) {
            arena->prev->prev = freechunks;
            freechunks = arena->prev;
            nfree++;
            freechunks->limit = arena->limit;
        } else
            _Mem_free(arena->prev, __FILE__, __LINE__);

        *arena = tmp;
    }

    assert(arena->limit == NULL);
    assert(arena->avail == NULL);
}

void Arena_remove_free_blocks() {
    while(freechunks) {
        T tmp = freechunks;
        freechunks = freechunks->prev;
        _Mem_free(tmp, __FILE__, __LINE__);
    }
}

void Arena_print_stats() {
#ifndef NDEBUG
    log_dbg("Allocated memory chunks");
    log_dbg("---------------------------------------------");
    log_dbg("%lu", chunk_allocations);
#endif
}





