#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include "mem.h"
#include "assert.h"

#include "timer.h"

#ifdef _WIN32
static LARGE_INTEGER frequency;
#endif

#define T Timer_T

struct T {
#ifdef _WIN32
    LARGE_INTEGER startCount;
#else
    timeval startCount;
#endif
    double startTimeInMicroSec;
};

T Timer_new_start() {
    T t;

    NEW(t);

#ifdef _WIN32
    if(!frequency.QuadPart) assert(QueryPerformanceFrequency(&frequency));
    assert(QueryPerformanceCounter(&t->startCount));
#else
    gettimeofday(&t->startCount, NULL);
#endif
    return t;
}

double Timer_elapsed_micro(T t) {
    double startTimeInMicroSec, endTimeInMicroSec;

#ifdef _WIN32
    LARGE_INTEGER endCount;

    assert(QueryPerformanceCounter(&endCount));

    startTimeInMicroSec = t->startCount.QuadPart * (1000000.0 / frequency.QuadPart);
    endTimeInMicroSec = endCount.QuadPart * (1000000.0 / frequency.QuadPart);
#else
    timeval endCount;

    gettimeofday(&endCount, NULL);
    startTimeInMicroSec = (t->startCount.tv_sec * 1000000.0) + t->startCount.tv_usec;
    endTimeInMicroSec = (endCount.tv_sec * 1000000.0) + endCount.tv_usec;
#endif
    return endTimeInMicroSec - startTimeInMicroSec;
}

double Timer_elapsed_micro_dispose(T t) {
    double elapsed;

    elapsed = Timer_elapsed_micro(t);
    FREE(t);
    return elapsed;
}
