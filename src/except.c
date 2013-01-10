#include <stdlib.h> /* for abort */
#include <stdio.h>  /* for fprintf */

#include "assert.h"
#include "except.h"

#define T Except_T

thread_local Except_Frame *Except_stack = NULL;

void Except_raise(const T *e, const char *file, int line) {

    Except_Frame *p = Except_stack;

    assert(e);

    if (p == NULL) {
        fprintf(stderr, "Uncaught exception");

        if (e->reason)
            fprintf(stderr, " %s", e->reason);
        else
            fprintf(stderr, " at 0x%p", e);

        if (file && line > 0)
            fprintf(stderr, " raised at %s:%d\n", file, line);

        fprintf(stderr, "aborting...\n");
        fflush(stderr);
        abort();
    }

    p->exception = e;
    p->file = file;
    p->line = line;

    Except_stack = Except_stack->prev;

    longjmp(p->env, Except_raised);
}

#ifdef _WIN32
#include <windows.h>

#define EXCEPTION_CASE(code)    \
 case code:                     \
  exceptionString = #code;      \
  break

LONG(CALLBACK win_exception_handler)(LPEXCEPTION_POINTERS pep) {

    EXCEPTION_RECORD* per = pep->ExceptionRecord;
    const char* exceptionString = NULL;
    DWORD codeBase = (DWORD)GetModuleHandle(NULL);
    char message[255];

    switch(per->ExceptionCode)   {
     EXCEPTION_CASE(EXCEPTION_ACCESS_VIOLATION);
     EXCEPTION_CASE(EXCEPTION_DATATYPE_MISALIGNMENT);
     EXCEPTION_CASE(EXCEPTION_BREAKPOINT);
     EXCEPTION_CASE(EXCEPTION_SINGLE_STEP);
     EXCEPTION_CASE(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
     EXCEPTION_CASE(EXCEPTION_FLT_DENORMAL_OPERAND);
     /* Need more cases */

     case 0xE06D7363:
      exceptionString = "C++ exception (using throw)";
      break;

     default:
      exceptionString = "Unknown exception";
      break;
    }

    sprintf(message,
      "An exception occured which wasnt handled!\nCode: %s (0x%08X)\nOffset: 0x%08X\nCodebase: 0x%08X",
      exceptionString,
      per->ExceptionCode,
      (DWORD)per->ExceptionAddress - codeBase,
      codeBase
     );

    Except_raise(&Assert_Failed, message, 0);
    return 0;
}
#endif // _WIN32

void Except_hook_signal() {
    #ifdef _WIN32
    SetUnhandledExceptionFilter(win_exception_handler);
    #endif // _WIN32

}
