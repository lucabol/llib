#ifndef THREAD_INCLUDED
#define THREAD_INCLUDED

#if 0
#include "except.h"

#define T Thread_T

typedef struct T *T;

extern const Except_T Thread_Failed;
extern const Except_T Thread_Alerted;
extern int  Thread_init (int preempt, ...);
extern T    Thread_new  (int apply(void *), void *args, int nbytes, ...);
extern void Thread_exit (int code);
extern void Thread_alert(T t);
extern T    Thread_self (void);
extern int  Thread_join (T t);
extern void Thread_pause(void);

#endif

/* Define thread_local for different gcc and msvc */
#ifdef __GNUC__
# define thread_local   __thread
#else
# ifdef _MSC_VER
#   define thread_local    __declspec(thread)
# else
#   define thread_local    
# endif
#endif

#undef T
#endif
