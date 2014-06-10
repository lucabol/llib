#include <math.h>

#include <rand.h>
#include <test.h>
#include <log.h>
#include <timer.h>

unsigned test_rand() {
    double average, tperf, stdev, contStd,  sumSqr = 0, sum = 0;
    int i, iters = 10000;
    Timer_T t;

    Rand_init(1);
    
    t = Timer_new_start();

    for(i = 0; i < iters; ++i) {
        double r = Rand_next();
        sum += r;
        sumSqr += r * r;
    }
    
    tperf = Timer_elapsed_micro_dispose(t);


    //log_set(stderr, LOG_DBG);
    log("Time to generate rnd: %f", tperf);
    //log_set(NULL, 0);

    average = sum / iters;
    stdev = sqrt((sumSqr - sum * average) / (iters - 1));
    contStd = 1 / sqrt(12.0);

    test_assert_float(contStd, 0.01, stdev);
    test_assert_float(0.5, 0.01, average);

    return TEST_SUCCESS;
}

unsigned test_gauss() {
#define iters  10000
    int i;
    double sum = 0, average, stdev, sumSqr = 0;

    Rand_init(1);

    for(i = 0; i < iters / 2; ++i) {
        double r1, r2;
        Rand_gauss(&r1, &r2);

        sum += r1 + r2;
        sumSqr += r1 * r1 + r2 * r2;
    }

    average = sum / iters;
    stdev = sqrt((sumSqr - sum * average) / (iters  - 1));

    test_assert_float(1., 0.01, stdev);
    test_assert_float(0, 0.05, average);

    return TEST_SUCCESS;
}
