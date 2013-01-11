#include <stdlib.h> /* for abort */
#include <stdio.h>  /* for fprintf */

#include "assert.h"
#include "except.h"

#define T Except_T

extern thread_local Except_T Native_Exception = { "A native exception has occurred" };

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
/* from here: http://stackoverflow.com/questions/3523716/is-there-a-function-to-convert-exception-pointers-struct-to-a-string */

// Compile with /EHa
#include <windows.h>
#include <Psapi.h>

static const char* opDescription( const ULONG opcode )
{
    switch( opcode ) {
    case 0: return "read";
    case 1: return "write";
    case 8: return "user-mode data execution prevention (DEP) violation";
    default: return "unknown";
    }
}

static const char* seDescription( const unsigned int code )
{
    switch( code ) {
        case EXCEPTION_ACCESS_VIOLATION:         return "EXCEPTION_ACCESS_VIOLATION"         ;
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED"    ;
        case EXCEPTION_BREAKPOINT:               return "EXCEPTION_BREAKPOINT"               ;
        case EXCEPTION_DATATYPE_MISALIGNMENT:    return "EXCEPTION_DATATYPE_MISALIGNMENT"    ;
        case EXCEPTION_FLT_DENORMAL_OPERAND:     return "EXCEPTION_FLT_DENORMAL_OPERAND"     ;
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "EXCEPTION_FLT_DIVIDE_BY_ZERO"       ;
        case EXCEPTION_FLT_INEXACT_RESULT:       return "EXCEPTION_FLT_INEXACT_RESULT"       ;
        case EXCEPTION_FLT_INVALID_OPERATION:    return "EXCEPTION_FLT_INVALID_OPERATION"    ;
        case EXCEPTION_FLT_OVERFLOW:             return "EXCEPTION_FLT_OVERFLOW"             ;
        case EXCEPTION_FLT_STACK_CHECK:          return "EXCEPTION_FLT_STACK_CHECK"          ;
        case EXCEPTION_FLT_UNDERFLOW:            return "EXCEPTION_FLT_UNDERFLOW"            ;
        case EXCEPTION_ILLEGAL_INSTRUCTION:      return "EXCEPTION_ILLEGAL_INSTRUCTION"      ;
        case EXCEPTION_IN_PAGE_ERROR:            return "EXCEPTION_IN_PAGE_ERROR"            ;
        case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "EXCEPTION_INT_DIVIDE_BY_ZERO"       ;
        case EXCEPTION_INT_OVERFLOW:             return "EXCEPTION_INT_OVERFLOW"             ;
        case EXCEPTION_INVALID_DISPOSITION:      return "EXCEPTION_INVALID_DISPOSITION"      ;
        case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION" ;
        case EXCEPTION_PRIV_INSTRUCTION:         return "EXCEPTION_PRIV_INSTRUCTION"         ;
        case EXCEPTION_SINGLE_STEP:              return "EXCEPTION_SINGLE_STEP"              ;
        case EXCEPTION_STACK_OVERFLOW:           return "EXCEPTION_STACK_OVERFLOW"           ;
        default: return "UNKNOWN EXCEPTION" ;
    }
}


LONG(CALLBACK win_exception_handler)(LPEXCEPTION_POINTERS ep) {

    HMODULE hm;
    MODULEINFO mi;
    char fn[MAX_PATH];
    char message[512];
    unsigned int code;

    code = ep->ExceptionRecord->ExceptionCode;

    GetModuleHandleEx(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)(ep->ExceptionRecord->ExceptionAddress), &hm );
    GetModuleInformation(GetCurrentProcess(), hm, &mi, sizeof(mi) );
    GetModuleFileNameExA(GetCurrentProcess(), hm, fn, MAX_PATH );

    sprintf(message, "SE %s at address %p inside %s loaded at base address %p\n",
        seDescription(code), ep->ExceptionRecord->ExceptionAddress, fn,
        mi.lpBaseOfDll);

    if(code == EXCEPTION_ACCESS_VIOLATION || code == EXCEPTION_IN_PAGE_ERROR ) {
        sprintf("%sInvalid operation: %s at address %p\n", message,
            opDescription(ep->ExceptionRecord->ExceptionInformation[0]),
            ep->ExceptionRecord->ExceptionInformation[1]);
    }

    if(code == EXCEPTION_IN_PAGE_ERROR) {
        sprintf("%sUnderlying NTSTATUS code that resulted in the exception %i", message,
            ep->ExceptionRecord->ExceptionInformation[2]);
    }

    Native_Exception.reason = message;
    Except_raise(&Native_Exception, "", 0);
    return 0;
}
#endif // _WIN32

void Except_hook_signal() {
    #ifdef _WIN32
    SetUnhandledExceptionFilter(win_exception_handler);
    #endif // _WIN32

}
