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

    Arena_dispose(&arena);
    return TEST_SUCCESS;
}

int test_mem_free() {

#define TEST_ASSERT(...) \
    TRY { \
        __VA_ARGS__; \
        RETURN TEST_FAILURE; \
    } EXCEPT(Assert_Failed) { \
        RETURN TEST_SUCCESS; \
    } END_TRY;

    TEST_ASSERT(
        int *i, *j;
        NEW(i);
        FREE(j); /* free not allocated pointer */
        );
    TEST_ASSERT(
        char *str, *mid;
        str = ALLOC(100);
        FREE(mid); /* free in the middle of the block */
        );

    return TEST_SUCCESS;
}

int main()
{
    test_add("arena", "resize", test_arena_resize);
    test_add("mem", "free", test_mem_free);
    return test_run_all();
}
