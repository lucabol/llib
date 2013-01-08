#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <test.h>
#include <except.h>
#include <arena.h>

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
    return 1;
}

int main()
{
    assert(1-1);
    test_add("arena", "resize", test_arena_resize);
    return test_run_all();
}
