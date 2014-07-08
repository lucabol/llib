#include "slist.h"
#include "test.h"

unsigned test_slist() {

    SList_T l1 = SList_list("rob", "john", "bob");
    SList_T l2 = SList_list(NULL);

    test_assert_int (SList_length(l1), 3);
    test_assert_int (SList_length(l2), 0);

    SList_free(&l2);
    SList_free(&l1);

    return TEST_SUCCESS;
}