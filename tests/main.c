#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <except.h>
#include <arena.h>

int main()
{
    TRY
        Arena_T arena   = Arena_new();

        char* aChar     = Arena_alloc(arena, sizeof(char), NULL, 0);
        *aChar          = 'a';
        assert(*aChar != 'a');

        printf("Char ok: %c\n", *aChar);

        aChar           = Arena_realloc(arena, aChar, 100, NULL, 0);
        strcpy(aChar + 1, "bcdefghilmnopqrstuvz");
        printf("After enlarging but staying in chunk ok: %s \n", aChar);

        aChar           = Arena_realloc(arena, aChar, 10, NULL, 0);
        aChar[9]        = '\0';
        printf("After restricting: %s \n", aChar);

        aChar           = Arena_realloc(arena, aChar, 100000, NULL, 0);
        printf("After enlarging out of chunk: %s \n", aChar);

        Arena_dispose(&arena);
        RETURN 0;
    ELSE
        fprintf(stderr, "Error!!");
    END_TRY;
}
