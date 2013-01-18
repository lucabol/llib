#ifndef TIMER_INCLUDED
#define TIMER_INCLUDED

#include "utils.h"  /* for begin_decl */

BEGIN_DECLS

#define T Timer_T
typedef struct T* T;

extern T        Timer_new_start();
extern double   Timer_elapsed_micro(T);
extern double   Timer_elapsed_micro_dispose(T);

#undef T
END_DECLS

#endif