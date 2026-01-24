// Statistics function tests
#include "test.h"

// Declarations from zap.h
double zap_mean(const double* samples, size_t n);
double zap_median(double* samples, size_t n);
double zap_percentile(const double* sorted_samples, size_t n, double p);
double zap_std_dev(const double* samples, size_t n, double mean);

TEST(test_mean_basic) {
    double samples[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double m = zap_mean(samples, 5);
    ASSERT_NEAR(m, 3.0, 0.0001);
}

TEST(test_mean_single) {
    double samples[] = {42.0};
    double m = zap_mean(samples, 1);
    ASSERT_NEAR(m, 42.0, 0.0001);
}

TEST(test_mean_empty) {
    double m = zap_mean(NULL, 0);
    ASSERT_NEAR(m, 0.0, 0.0001);
}

TEST(test_median_odd) {
    double samples[] = {5.0, 1.0, 3.0, 2.0, 4.0};
    double m = zap_median(samples, 5);
    ASSERT_NEAR(m, 3.0, 0.0001);
}

TEST(test_median_even) {
    double samples[] = {4.0, 1.0, 3.0, 2.0};
    double m = zap_median(samples, 4);
    ASSERT_NEAR(m, 2.5, 0.0001);
}

TEST(test_median_single) {
    double samples[] = {42.0};
    double m = zap_median(samples, 1);
    ASSERT_NEAR(m, 42.0, 0.0001);
}

TEST(test_percentile_p50) {
    double sorted[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double p = zap_percentile(sorted, 5, 50.0);
    ASSERT_NEAR(p, 3.0, 0.0001);
}

TEST(test_percentile_p0) {
    double sorted[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double p = zap_percentile(sorted, 5, 0.0);
    ASSERT_NEAR(p, 1.0, 0.0001);
}

TEST(test_percentile_p100) {
    double sorted[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double p = zap_percentile(sorted, 5, 100.0);
    ASSERT_NEAR(p, 5.0, 0.0001);
}

TEST(test_std_dev_basic) {
    // Values: 2, 4, 4, 4, 5, 5, 7, 9
    // Mean = 5, variance = 4, std_dev = 2
    double samples[] = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
    double mean = zap_mean(samples, 8);
    double sd = zap_std_dev(samples, 8, mean);
    ASSERT_NEAR(mean, 5.0, 0.0001);
    ASSERT_NEAR(sd, 2.138, 0.01);  // Sample std dev
}

TEST(test_std_dev_single) {
    double samples[] = {42.0};
    double sd = zap_std_dev(samples, 1, 42.0);
    ASSERT_NEAR(sd, 0.0, 0.0001);
}

void test_stats(void) {
    RUN_TEST(test_mean_basic);
    RUN_TEST(test_mean_single);
    RUN_TEST(test_mean_empty);
    RUN_TEST(test_median_odd);
    RUN_TEST(test_median_even);
    RUN_TEST(test_median_single);
    RUN_TEST(test_percentile_p50);
    RUN_TEST(test_percentile_p0);
    RUN_TEST(test_percentile_p100);
    RUN_TEST(test_std_dev_basic);
    RUN_TEST(test_std_dev_single);
}
