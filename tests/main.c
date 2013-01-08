#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <test.h>
#include <except.h>
#include <arena.h>

int test_arena_resize() {
    Arena_T arena   = Arena_new();

    char* aChar     = Arena_alloc(arena, sizeof(char), NULL, 0);
    *aChar          = 'a';
    tassert(*aChar == 'a');

    aChar           = Arena_realloc(arena, aChar, 100, NULL, 0);
    strcpy(aChar + 1, "bcdefghilmnopqrstuvz");
    tassert_str(aChar, "abcdefghilmnopqrstuvz");

    aChar           = Arena_realloc(arena, aChar, 10, NULL, 0);
    aChar[9]        = '\0';
    tassert_str(aChar, "abcdefghi");        

    aChar           = Arena_realloc(arena, aChar, 100000, NULL, 0);
    tassert_str(aChar, "abcdefghi");

    Arena_dispose(&arena);
    return 1;
}

int main()
{
    test_add("arena", "resize", test_arena_resize);
    test_run_all();
}
