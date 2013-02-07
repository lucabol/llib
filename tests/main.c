#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <test.h>
#include <except.h>
#include <log.h>

#include <mem.h>
#include <ring.h>

#include "getopt.h"
#include "log.h"

int test_arena_resize() {
    Arena_T arena   = Arena_new();
    char * aChar;

    Mem_set_arena(arena);

    aChar           = ALLOC(sizeof(char));
    *aChar          = 'a';
    test_assert(*aChar == 'a');

    REALLOC(aChar, 100);
    strcpy(aChar + 1, "bcdefghilmnopqrstuvz");
    test_assert_str(aChar, "abcdefghilmnopqrstuvz");

    REALLOC(aChar, 10);
    aChar[9]        = '\0';
    test_assert_str(aChar, "abcdefghi");

    REALLOC(aChar, 100000);
    test_assert_str(aChar, "abcdefghi");

    Mem_set_default();

    Arena_dispose(arena);
    Arena_remove_free_blocks();

    return TEST_SUCCESS;
}

int test_mem_free() {

    int result = TEST_SUCCESS;

#define TEST_ASSERT(...) \
    TRY { \
        __VA_ARGS__; \
        result = TEST_FAILURE; \
    } EXCEPT(Assert_Failed) { \
    } END_TRY;

#ifndef NDEBUG /* If the non-checking version of the mem library is used, then the code below
                  accesses wrong pointers, hence it crashes as expected*/
    int k, *i = NULL, *j = &k;
    char *str, *mid;

    TEST_ASSERT(
        NEW(i);
        FREE(j);
        );
    FREE(i);

    TEST_ASSERT(
        str = ALLOC(100);
        mid = str + 50;
        FREE(mid);
        );
    FREE(str);

    TEST_ASSERT(
        str = ALLOC(100);
        REALLOC(mid, 200);
        );
    FREE(str);

#endif /*NDEBUG*/
    return result;
}

int test_native_exceptions() {

#ifdef NATIVE_EXCEPTIONS

    Except_hook_signal();
    TRY {
        int a = 42;
        volatile int b = 0;
        log("%i", a / b);
        return TEST_FAILURE;
    } EXCEPT(Native_Exception) {
        log("%s", Except_frame.exception->reason);
        test_assert(1);
    } END_TRY;

    TRY {
        int* a = 0;
        *a = 43;
        log("%i", *a);
        return TEST_FAILURE;
    } EXCEPT(Native_Exception) {
        log("%s", Except_frame.exception->reason);
        test_assert(1);
    } END_TRY;
#endif /*NATIVE_EXCEPTIONS*/

    return TEST_SUCCESS;
}

int test_log() {
    int dbg = debug_level;
    FILE* file = dbgstream;

    log_set(NULL, LOG_DISABLE);
    log("Don't print this number: %i\n", 10);

    log_set(stderr, LOG_INFO);
    /*log("Print this number: %i", 10); */

    log_set(NULL, LOG_DISABLE);
    log("Don't print this number: %i\n", 10);

    log_set(file, dbg);

    return TEST_SUCCESS;
}

void by2(void** i, void* v) {
    int* ip;
    (void)v;
    
    ip = (int*)*i;
    *ip = *ip * 2;
}

int test_ring_apply() {
    int k = 0, *p, i, sum = 0;
    Ring_T ring = Ring_new();

    for(i=0; i < 10; i++) {
        NEW(p);
        *p = k++;
        Ring_push_front(ring, (void*) p);
    }

    Ring_map(ring, by2, NULL);

    for(i=0; i < 10; i++) {
        int* res = ((int*)Ring_pop_front(ring));
        sum += *res;
        FREE(res);
    }
    test_assert(sum == 90);

    Ring_free(&ring);
    return TEST_SUCCESS;
}

static int verbosity = 0;

static struct option long_options[] = {
    {"verbosity",      no_argument,       NULL,  'v' , "print out debug messages","", getopt_none, NULL ,&verbosity},
};

int test_mem_perf();
int test_list();
int test_list_perf();
int test_getopt_parse();
int test_utf8_roundtrip();
int test_utf8_len();

int main(int argc, char *argv[])
{
    int res;

    if(getopt_parse(argc, argv, long_options, "-- tests for llib", "", "") < 0)
        exit(-3);

    if(verbosity)
        log_set(stderr, LOG_INFO);

    test_add("mem", "free",             test_mem_free);
    test_add("mem", "perf",             test_mem_perf);
    test_add("arena", "resize",         test_arena_resize);
    test_add("exception", "native",     test_native_exceptions);
    test_add("log", "printing",         test_log);
    test_add("list", "basic",           test_list);
    test_add("list", "perf",            test_list_perf);
    test_add("ring", "apply",           test_ring_apply);
    test_add("getopt", "parse",         test_getopt_parse);
    test_add("utf8", "roundtrip",       test_utf8_roundtrip);
    test_add("utf8", "strlen",          test_utf8_len);
    res = test_run_all();

    Mem_print_stats();

    return res;
}
