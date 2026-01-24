// Filter matching tests
#include "test.h"
#include <stdbool.h>

// Declaration from zap.h
bool zap_matches_filter(const char* name, const char* pattern);

TEST(test_filter_null_pattern) {
    ASSERT(zap_matches_filter("anything", NULL) == true);
}

TEST(test_filter_empty_pattern) {
    ASSERT(zap_matches_filter("anything", "") == true);
}

TEST(test_filter_null_name) {
    ASSERT(zap_matches_filter(NULL, "pattern") == false);
}

TEST(test_filter_substring_match) {
    ASSERT(zap_matches_filter("bench_sort_quick", "sort") == true);
    ASSERT(zap_matches_filter("bench_sort_quick", "quick") == true);
    ASSERT(zap_matches_filter("bench_sort_quick", "bench") == true);
    ASSERT(zap_matches_filter("bench_sort_quick", "xyz") == false);
}

TEST(test_filter_exact_match) {
    ASSERT(zap_matches_filter("sort", "sort") == true);
    ASSERT(zap_matches_filter("sort", "Sort") == false);  // case sensitive
}

TEST(test_filter_wildcard_star) {
    ASSERT(zap_matches_filter("bench_sort", "bench_*") == true);
    ASSERT(zap_matches_filter("bench_sort", "*_sort") == true);
    ASSERT(zap_matches_filter("bench_sort", "*") == true);
    ASSERT(zap_matches_filter("bench_sort", "bench*sort") == true);
    ASSERT(zap_matches_filter("bench_sort", "xyz*") == false);
}

TEST(test_filter_wildcard_question) {
    ASSERT(zap_matches_filter("abc", "a?c") == true);
    ASSERT(zap_matches_filter("abc", "???") == true);
    ASSERT(zap_matches_filter("abc", "??") == false);
    ASSERT(zap_matches_filter("abc", "????") == false);
}

TEST(test_filter_mixed_wildcards) {
    ASSERT(zap_matches_filter("bench_sort_quick", "bench_*_?????") == true);
    ASSERT(zap_matches_filter("bench_sort_quick", "*sort*") == true);
    ASSERT(zap_matches_filter("bench_sort_quick", "?????_*") == true);
}

void test_filter(void) {
    RUN_TEST(test_filter_null_pattern);
    RUN_TEST(test_filter_empty_pattern);
    RUN_TEST(test_filter_null_name);
    RUN_TEST(test_filter_substring_match);
    RUN_TEST(test_filter_exact_match);
    RUN_TEST(test_filter_wildcard_star);
    RUN_TEST(test_filter_wildcard_question);
    RUN_TEST(test_filter_mixed_wildcards);
}
