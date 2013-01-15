#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <test.h>
#include <except.h>
#include <arena.h>
#include <mem.h>
#include <log.h>

int test_arena_resize() {
    Arena_T arena   = Arena_new();

    char* aChar     = Arena_alloc(arena, sizeof(char), NULL, 0);
    *aChar          = 'a';
    test_assert(*aChar == 'a');

    aChar           = Arena_realloc(arena, aChar, 100, NULL, 0);
    strcpy(aChar + 1, "bcdefghilmnopqrstuvz");
    test_assert_str(aChar, "abcdefghilmnopqrstuvz");

    aChar           = Arena_realloc(arena, aChar, 10, NULL, 0);
    aChar[9]        = '\0';
    test_assert_str(aChar, "abcdefghi");

    aChar           = Arena_realloc(arena, aChar, 100000, NULL, 0);
    test_assert_str(aChar, "abcdefghi");

    Arena_dispose(arena);

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
        printf("%i", a / b);
        return TEST_FAILURE;
    } EXCEPT(Native_Exception) {
        printf("%s", Except_frame.exception->reason);
        test_assert(1);
    } END_TRY;

    TRY {
        int* a = 0;
        *a = 43;
        printf("%i", *a);
        return TEST_FAILURE;
    } EXCEPT(Native_Exception) {
        printf("%s", Except_frame.exception->reason);
        test_assert(1);
    } END_TRY;
#endif /*NATIVE_EXCEPTIONS*/

    return TEST_SUCCESS;
}

int test_log() {
    log_init(NULL, LOG_DISABLE);
    log("Don't print this number: %i\n", 10);

    log_init(stderr, LOG_INFO);
    log("Print this number: %i", 10);

    log_init(NULL, LOG_DISABLE);
    log("Don't print this number: %i\n", 10);

    return TEST_SUCCESS;
}

int test_mem_perf();

int main()
{
    int res;

    /* log_init(stderr, LOG_DBG); */

    test_add("mem", "free",             test_mem_free);
    test_add("mem", "perf",             test_mem_perf);
    test_add("arena", "resize",         test_arena_resize);
    test_add("exception", "native",     test_native_exceptions);
    test_add("log", "printing",         test_log);
    res = test_run_all();

    Mem_print_allocated();
    return res;
}
