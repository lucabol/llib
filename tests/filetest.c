#include "file.h"
#include "test.h"
#include "mem.h"

int test_file() {
    dir d = dir_open(".\\");
    while(dir_next(d)) {
        fileinfo f = dir_readfile(d);
        printf("%s", f->name);
        FREE(f);
    }
    dir_close(d);
    return TEST_SUCCESS;
}
