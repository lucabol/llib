#ifndef TEST_INCLUDED
#define TEST_INCLUDED

#include <stdio.h>  /* for printf */
#include <string.h> /* for strcmp */

#include "utils.h"  /* for begin_decl*/

BEGIN_DECLS

#define TEST_SUCCESS 1
#define TEST_FAILURE 0

#define test_assert(e) STMT_START {                                                                 \
    if((e) == 0) { printf("%s:%i Test failed: " #e "\n", __FILE__, __LINE__);                                                     \
             return TEST_FAILURE; }} STMT_END

#define test_assert_str(s1, s2) STMT_START {                                                        \
    if(strcmp((s1), (s2)) != 0) { printf("%s:%i Test failed (got, expected): %s == %s \n", __FILE__, __LINE__, (s1), (s2));   \
        return TEST_FAILURE; } } STMT_END

#define test_assert_ex(which, ...) \
    TRY { \
        __VA_ARGS__; \
        test_assert(((void)#__VA_ARGS__, 0)); \
    } EXCEPT(which) { \
    } END_TRY;

typedef unsigned (*test_func) ();

extern void test_add(char* kind, char* feature, test_func f);
extern unsigned test_run_all();
extern double test_perf(test_func f);

END_DECLS

#endif

