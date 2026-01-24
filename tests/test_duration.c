// Duration parsing tests
// Note: zap__parse_duration is static, so we test it indirectly
// or skip for now until we expose it
#include "test.h"

TEST(test_duration_placeholder) {
    // zap__parse_duration is static, testing skipped for now
    ASSERT(1);
}

void test_duration(void) {
    RUN_TEST(test_duration_placeholder);
}
