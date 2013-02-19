#include "dir.h"
#include "test.h"
#include "mem.h"

unsigned test_file() {
    dir_dir* dir;
    dir_file* file;
    dir = Dir_open( ".");

    while (file = Dir_readfile(dir))
    {
        printf("%s", file->name);
        if (file->is_dir)
        {
            printf("/");
        }
        printf("\n");

    }

    Dir_close(dir);

    return TEST_SUCCESS;
}
