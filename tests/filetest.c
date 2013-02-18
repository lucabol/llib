#include "dir.h"
#include "test.h"
#include "mem.h"

unsigned test_file() {
    dir_dir dir;
    if (dir_open(&dir, ".") == -1)
    {
        perror("Error opening file");
        goto bail;
    }

    while (dir.has_next)
    {
        dir_file file;
        if (dir_readfile(&dir, &file) == -1)
        {
            perror("Error getting file");
            goto bail;
        }

        printf("%s", file.name);
        if (file.is_dir)
        {
            printf("/");
        }
        printf("\n");

        dir_next(&dir);
    }

bail:
    dir_close(&dir);

    return TEST_SUCCESS;
}
