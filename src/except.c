#include <stdlib.h> /* for abort */
#include <stdio.h>  /* for fprintf */

#include "assert.h"
#include "except.h"
#include "log.h"

#define T Except_T

thread_local Except_T Native_Exception = { "A native exception has occurred" };

thread_local Except_Frame *Except_stack = NULL;

void Except_raise(const T *e, const char *file, int line) {

    Except_Frame *p = Except_stack;

    assert(e);

    if (p == NULL) {
        char message[50];
        sprintf(message, "Uncaught exception");

        if (e->reason)
            sprintf(message, " %s", e->reason);
        else
            sprintf(message, " at 0x%p", e);

        if (file && line > 0)
            sprintf(message, " raised at %s:%d\n", file, line);

        sprintf(message, "aborting...\n");
        log(message);
        abort();
    }

    p->exception = e;
    p->file = file;
    p->line = line;

    Except_stack = Except_stack->prev;

    longjmp(p->env, Except_raised);
}
