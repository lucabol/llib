#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <test.h>
#include <except.h>
#include <arena.h>
#include <mem.h>

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
    Except_hook_signal();
    TRY {
        int a = 42;
        volatile int b = 0;
        printf("%i", a / b);
        return TEST_FAILURE;
    } EXCEPT(Native_Exception) {
        printf("%s", Except_frame.exception->reason);
        test_assert(1);
        return TEST_SUCCESS;
    } END_TRY;

    return TEST_SUCCESS;
}

int main()
{
    int res;
    test_add("mem", "free", test_mem_free);
    test_add("arena", "resize", test_arena_resize);
    test_add("exception", "native", test_native_exceptions);
    res = test_run_all();

    Mem_print_allocated();
    return res;
}
