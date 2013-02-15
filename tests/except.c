#include <except.h>
#include <log.h>
#include <test.h>

unsigned test_native_exceptions() {

#ifdef NATIVE_EXCEPTIONS

    Except_hook_signal();
    TRY {
        int a = 42;
        volatile int b = 0;
        log("%i", a / b);
        return TEST_FAILURE;
    } EXCEPT(Native_Exception) {
        log("%s", Except_frame.exception->reason);
        test_assert(1);
    } END_TRY;

    TRY {
        int* a = 0;
        *a = 43;
        log("%i", *a);
        return TEST_FAILURE;
    } EXCEPT(Native_Exception) {
        log("%s", Except_frame.exception->reason);
        test_assert(1);
    } END_TRY;
#endif /*NATIVE_EXCEPTIONS*/

    return TEST_SUCCESS;
}

