#ifndef TEST_INCLUDED
#define TEST_INCLUDED

#include <stdio.h>

#include <utils.h>

BEGIN_DECLS

#define tassert(e) STMT_START {                                                                     \
    if(!e) { printf("Test failed: \n" #e); return 0; } } STMT_END

#define tassert_str(s1, s2) STMT_START {                                                            \
    if(strcmp(s1, s2) != 0) { printf("Test failed (got, expected): %s == %s \n", s1, s2);           \
        return 0; } } STMT_END

typedef int (*test_func) ();

void test_add(char* kind, char* feature, test_func f);
int test_run_all();

END_DECLS

#endif

