// Minimal test framework for zap
#ifndef ZAP_TEST_H
#define ZAP_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Counters are extern - defined in test_main.c
extern int tests_run;
extern int tests_passed;
extern int tests_failed;
extern int current_test_failed;

#define TEST(name) static void name(void)

#define RUN_TEST(name) do { \
    tests_run++; \
    current_test_failed = 0; \
    printf("  %s... ", #name); \
    fflush(stdout); \
    name(); \
    if (current_test_failed) { \
        tests_failed++; \
    } else { \
        tests_passed++; \
        printf("ok\n"); \
    } \
} while (0)

#define ASSERT(cond) do { \
    if (!(cond)) { \
        printf("FAILED\n    %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        current_test_failed = 1; \
        return; \
    } \
} while (0)

#define ASSERT_EQ(a, b) do { \
    if ((a) != (b)) { \
        printf("FAILED\n    %s:%d: %s != %s\n", __FILE__, __LINE__, #a, #b); \
        current_test_failed = 1; \
        return; \
    } \
} while (0)

#define ASSERT_STREQ(a, b) do { \
    if (strcmp((a), (b)) != 0) { \
        printf("FAILED\n    %s:%d: \"%s\" != \"%s\"\n", __FILE__, __LINE__, (a), (b)); \
        current_test_failed = 1; \
        return; \
    } \
} while (0)

#define ASSERT_NEAR(a, b, eps) do { \
    if (fabs((a) - (b)) > (eps)) { \
        printf("FAILED\n    %s:%d: %g != %g (eps=%g)\n", __FILE__, __LINE__, (double)(a), (double)(b), (double)(eps)); \
        current_test_failed = 1; \
        return; \
    } \
} while (0)

#define TEST_REPORT() do { \
    printf("\n%d tests, %d passed, %d failed\n", tests_run, tests_passed, tests_failed); \
    return tests_failed > 0 ? 1 : 0; \
} while (0)

#endif // ZAP_TEST_H
