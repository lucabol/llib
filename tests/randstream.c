#include <math.h>

#include <randstream.h>
#include <test.h>
#include <log.h>
#include <timer.h>

unsigned test_randstream() {
    double average, tperf, stdev, contStd,  sumSqr = 0, sum = 0;
    int i, iters = 10000;
    Timer_T t;

    RandStream_T rs = RandStream_new();   

    t = Timer_new_start();

    for(i = 0; i < iters; ++i) {
        double r = RandStream_randU01(rs);
        sum += r;
        sumSqr += r * r;
    }
    
    RandStream_free(&rs);

    tperf = Timer_elapsed_micro_dispose(t);


    //log_set(stderr, LOG_DBG);
    //log("Time to generate rnd: %f", tperf);
    //log_set(NULL, 0);

    average = sum / iters;
    stdev = sqrt((sumSqr - sum * average) / (iters - 1));
    contStd = 1 / sqrt(12.0);

    test_assert_float(contStd, 0.001, stdev); // one order better uniform than Rand
    test_assert_float(0.5, 0.001, average);

    return TEST_SUCCESS;
}

unsigned test_streamgauss() {
    int i, iters = 10000;
    double sum = 0, average, stdev, sumSqr = 0, variance = 1.;

    RandStream_T rs = RandStream_new();   

    for(i = 0; i < iters; ++i) {
        double r = RandStream_gauss(rs, variance);
        sum += r;
        sumSqr += r * r;
    }

    RandStream_free(&rs);

    average = sum / iters;
    stdev = sqrt((sumSqr - sum * average) / (iters  - 1));

    test_assert_float(sqrt(variance), 0.01, stdev);
    test_assert_float(0, 0.05, average);

    return TEST_SUCCESS;
}

unsigned test_parallelrand() {

    int iters = 10000, i;
    double sum12 = 0, sum1 = 0, sum2 = 0, covariance;

    RandStream_T rs1 = RandStream_new();
    RandStream_T rs2 = RandStream_new();


    for (i = 0; i < iters; ++i) {
        double r1 = RandStream_randU01(rs1);
        double r2 = RandStream_randU01(rs2);
        sum1 += r1;
        sum2 += r2;
        sum12 += r1 * r2;
    }

    covariance = (sum12 - sum1 * sum2 / iters) / iters;

    test_assert_float(0., 0.01, covariance);

    sum1 = 0, sum2 = 0, sum12 = 0;

    for (i = 0; i < iters; ++i) {
        double r1 = RandStream_gauss(rs1, 1);
        double r2 = RandStream_gauss(rs2, 1);
        sum1 += r1;
        sum2 += r2;
        sum12 += r1 * r2;
    }

    covariance = (sum12 - sum1 * sum2 / iters) / iters;

    test_assert_float(0., 0.01, covariance);

    RandStream_free(&rs1);
    RandStream_free(&rs2);

    return TEST_SUCCESS;
}
