#include "dir.h"
#include "test.h"
#include "mem.h"

unsigned test_file() {
    dir_entry dir;
    char* file;
    unsigned n = 0;
    dir = Dir_open( "..");

    while (file = Dir_next_entry(dir))
    {
        /*printf("%s\n", file);*/
        n++;
        FREE(file);
    }

    Dir_close(dir);
    test_assert(n >= 2);

    return TEST_SUCCESS;
}
