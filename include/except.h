#ifndef EXCEPT_INCLUDED
#define EXCEPT_INCLUDED

#include <setjmp.h>     /* for setjmp*/

#include "utils.h"      /* for begin_decl */
#include "portable.h"   /* for thread_local */

BEGIN_DECLS

#define T Except_T

typedef struct T {
    const char *reason;
} T;

typedef struct Except_Frame Except_Frame;

struct Except_Frame {
    Except_Frame *prev;
    jmp_buf env;
    const char *file;
    int line;
    const T *exception;
};

enum { Except_entered=0, Except_raised, Except_handled,   Except_finalized };

extern thread_local Except_Frame *Except_stack;

extern const    Except_T Assert_Failed;
extern thread_local Except_T Native_Exception;

void Except_raise(const T *e, const char *file,int line);

#define RAISE(e) Except_raise(&(e), __FILE__, __LINE__)

#define RAISE_RET(e) STMT_START { \
    Except_raise(&(e), __FILE__, __LINE__); \
    return; \
    } STMT_END

#define RAISE_VAL(e, val) STMT_START { \
    Except_raise(&(e), __FILE__, __LINE__); \
    return val; \
    } STMT_END

#define RAISE_INT(e) RAISE_VAL(e, 0)
#define RAISE_DOUBLE(e) RAISE_VAL(e, 0.0)
#define RAISE_STRING(e) RAISE_VAL(e, "")
#define RAISE_PTR(e) RAISE_VAL(e, NULL)

#define RERAISE Except_raise(Except_frame.exception, \
    Except_frame.file, Except_frame.line)

/* In all my examples this is not needed ...*/
/* #define RETURN switch (Except_stack = Except_stack->prev,0) default: return */

#define TRY do { \
    volatile int Except_flag; \
    Except_Frame Except_frame; \
    Except_frame.prev = Except_stack; \
    Except_stack = &Except_frame;  \
    Except_flag = setjmp(Except_frame.env); \
    if (Except_flag == Except_entered) {

#define EXCEPT(e) \
        if (Except_flag == Except_entered) Except_stack = Except_stack->prev; \
    } else if (Except_frame.exception == &(e)) { \
        Except_flag = Except_handled;

#define ELSE \
        if (Except_flag == Except_entered) Except_stack = Except_stack->prev; \
    } else { \
        Except_flag = Except_handled;

#define FINALLY \
        if (Except_flag == Except_entered) Except_stack = Except_stack->prev; \
    } { \
        if (Except_flag == Except_entered) \
            Except_flag = Except_finalized;

#define END_TRY \
        if (Except_flag == Except_entered) Except_stack = Except_stack->prev; \
        } if (Except_flag == Except_raised) RERAISE; \
} while (0)

#undef T

END_DECLS

#endif
