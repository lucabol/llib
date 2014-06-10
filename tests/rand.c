#include <math.h>

#include <rand.h>
#include <test.h>
#include <log.h>
#include <timer.h>

unsigned test_rand() {
    double average, tperf, stdev, contStd,  sumSqr = 0, sum = 0;
    int i, iters = 100000;
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

    test_assert(average > 0.49 && average < 0.51);
    test_assert(stdev > contStd - 0.01 && stdev < contStd + 0.01);

    return TEST_SUCCESS;
}
