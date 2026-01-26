// Zap unit tests
#define ZAP_IMPLEMENTATION
#include "zap.h"
#include "test.h"

// Define counters
int tests_run = 0;
int tests_passed = 0;
int tests_failed = 0;
int current_test_failed = 0;

// Forward declarations for test suites
void test_stats(void);
void test_duration(void);
void test_filter(void);
void test_baseline(void);

int main(void) {
    printf("Running zap tests...\n\n");

    printf("Statistics:\n");
    test_stats();

    printf("\nDuration parsing:\n");
    test_duration();

    printf("\nFilter matching:\n");
    test_filter();

    printf("\nBaseline storage:\n");
    test_baseline();

    TEST_REPORT();
}
