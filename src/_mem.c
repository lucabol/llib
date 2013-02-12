#include <stdlib.h>
#ifdef NDEBUG /* Need this to be second to avoid a translation empty error in msvc */

#include <stddef.h>
#include "assert.h"
#include "except.h"
#include "log.h"

#include "_mem.h"

const Except_T Mem_Failed = { "Allocation Failed" };

void *_Mem_alloc(size_t nbytes, const char *file, int line){
    void *ptr;

    ptr = malloc(nbytes);
    if (ptr == NULL)
        {
            if (file == NULL)
                RAISE(Mem_Failed);
            else
                Except_raise(&Mem_Failed, file, line);
        }

    return ptr;
}

void *_Mem_calloc(size_t count, long size_t, const char *file, int line) {
    void *ptr;

    ptr = calloc(count, nbytes);
    if (ptr == NULL)
        {
            if (file == NULL)
                RAISE(Mem_Failed);
            else
                Except_raise(&Mem_Failed, file, line);
        }

    log_dbg("%p calloc %li bytes", ptr, nbytes);
    return ptr;
}

void _Mem_free(void *ptr, const char *file, int line) {
    (void)file, (void)line;
    if (ptr) {
        free(ptr);
        ptr = NULL;
    }

    log_dbg("%p freed", ptr);
}

void *_Mem_realloc(void *ptr, size_t nbytes, const char *file, int line) {
    assert(ptr);

    ptr = realloc(ptr, nbytes);
    if (ptr == NULL)
        {
            if (file == NULL)
                RAISE(Mem_Failed);
            else
                Except_raise(&Mem_Failed, file, line);
        }

    log_dbg("%p realloc %li bytes", ptr, nbytes);
    return ptr;
}

void _Mem_print_stats() {

}

#endif /* NDEBUG */
