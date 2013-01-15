#include <stdlib.h>
#ifndef NDEBUG /* Need this to be second to avoid a translation empty error in msvc */

#include <string.h> /* for memcpy */
#include <stdio.h> /* for printf */

#include "assert.h"
#include "except.h"
#include "mem.h"
#include "log.h"

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
    long long ll;
#endif
};

#define hash(p, t) (((unsigned long)(p)>>3) & (sizeof (t) / sizeof ((t)[0])-1))

#define NDESCRIPTORS 512

/* A space of 4096 or more bytes that is a multiple of sizeof (union align) */
#define NALLOC ((4096 + sizeof (union align) - 1) / (sizeof (union align)))*(sizeof (union align))

const Except_T Mem_Failed = { "Allocation Failed" };

#define max_hashed  2048

static struct descriptor {
    struct descriptor *free;
    struct descriptor *link;
    const void *ptr;
    long size;
    const char *file;
    int line;
} *htab[max_hashed];

static struct descriptor freelist = { &freelist, NULL, NULL, 0, NULL, 0 };

static struct descriptor *find(const void *ptr) {
    struct descriptor *bp = htab[hash(ptr, htab)];

    while (bp && bp->ptr != ptr)
        bp = bp->link;
    return bp;
}

void Mem_free(void *ptr, const char *file, int line) {

    if (ptr) {
        struct descriptor *bp;

        /* Catches:
            . free on not aligned pointer (i.e. from middle of structure
            . free on pointer not given out with malloc
            . free pointer twice
        */
        int is_not_aligned = ((unsigned long)ptr) % (sizeof (union align)) != 0;
        bp = find(ptr);
        if ( is_not_aligned || bp == NULL || bp->free)
            Except_raise(&Assert_Failed, file, line);

        bp->free = freelist.free;
        freelist.free = bp;

        log_dbg("%p freed", ptr);
    }
}

void *Mem_realloc(void *ptr, long nbytes, const char *file, int line) {
    struct descriptor *bp;
    void *newptr;

    assert(ptr);
    assert(nbytes > 0);

    log_dbg("%p realloc %li bytes", ptr, nbytes);

    /* This is rather error prone, but it is needed to conform to realloc spec */
    if(!ptr)
        return Mem_alloc(nbytes, file, line);

    /* This conforms to C90. In C99 it is left implementation dependent.*/
    if(!nbytes)
        Mem_free(ptr, file, line);

    if (((unsigned long)ptr)%(sizeof (union align)) != 0 || (bp = find(ptr)) == NULL || bp->free)
        Except_raise(&Assert_Failed, file, line);

    newptr = Mem_alloc(nbytes, file, line);
    memcpy(newptr, ptr, nbytes < bp->size ? nbytes : bp->size);
    Mem_free(ptr, file, line);

    return newptr;
}

void *Mem_calloc(long count, long nbytes, const char *file, int line) {
    void *ptr;
    assert(count > 0);
    assert(nbytes > 0);

    ptr = Mem_alloc(count*nbytes, file, line);
    memset(ptr, '\0', count*nbytes);

    log_dbg("%p calloc %li bytes", ptr, nbytes);
    return ptr;
}

/* Initialize the avail descriptor in the list (allocates more if needed) */
static struct descriptor *dalloc(void *ptr, long size, const char *file, int line) {
    static struct descriptor *avail;
    static int nleft;

    if (nleft <= 0) {
        avail = malloc(NDESCRIPTORS*sizeof (*avail));
        if (avail == NULL)
            return NULL;
        nleft = NDESCRIPTORS;
    }

    avail->ptr  = ptr;
    avail->size = size;
    avail->file = file;
    avail->line = line;
    avail->free = avail->link = NULL;
    nleft--;

    return avail++;
}

void *Mem_alloc(long nbytes, const char *file, int line){
    struct descriptor *bp;
    void *ptr;
    assert(nbytes > 0);

    /* Makes nbytes a multiple of the necessary alignment size */
    nbytes = ((nbytes + sizeof (union align) - 1) / (sizeof (union align))) * (sizeof (union align));

    for (bp = freelist.free; bp; bp = bp->free) {
        if (bp->size > nbytes) {
            /* Carves out a nbytes block at the end of the block, adds it to the list and returns it */
            bp->size -= nbytes;
            ptr = (char *)bp->ptr + bp->size;

            if ((bp = dalloc(ptr, nbytes, file, line)) != NULL) {
                unsigned h = hash(ptr, htab);
                bp->link = htab[h];
                htab[h] = bp;
                return ptr;
            } else
                {
                    if (file == NULL)
                        RAISE(Mem_Failed);
                    else
                        Except_raise(&Mem_Failed, file, line);
                }
        }

        if (bp == &freelist) {
            struct descriptor *newptr;

            /* Malloc nbytes (correctly aligned) + a 4096 correctly aligned block and
               returns a pointer to a descriptor. It means that each allocation adds one more
               page in memory, so that it can satisfy requests for small blocks from this larger chunk.
            */
            if ((ptr = malloc(nbytes + NALLOC)) == NULL ||  (newptr = dalloc(ptr, nbytes + NALLOC,
                    __FILE__, __LINE__)) == NULL)
                {
                    if (file == NULL)
                        RAISE(Mem_Failed);
                    else
                        Except_raise(&Mem_Failed, file, line);
                }
            log_dbg("%p alloc %li bytes", ptr, nbytes + NALLOC);
            /* Add the block as the first one in the free list. It will be picked up in the next iteration. */
            newptr->free = freelist.free;
            freelist.free = newptr;
        }
    }

    assert(0);
    return NULL;
}

void Mem_print_allocated() {
    struct descriptor* bp;
    int i;
    int found = 0;

    printf("Allocated memory blocks (file:line size, ptr)\n");
    printf("---------------------------------------------\n");

    for(i = 0; i < max_hashed ; i++) {
        for(bp = htab[i]; bp; bp = bp->link) {
            if(bp->free == NULL) {
                printf("%20s:%i%10li%10p\n", bp->file, bp->line, bp->size, bp->ptr);
                found = 1;
            }
        }
    }
    if(!found) printf("No allocated blocks found.\n");
    else printf("\n");
}

#endif /*NDEBUG*/
