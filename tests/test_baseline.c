// Baseline storage tests
#include "test.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

// Declarations from zap.h
typedef struct zap_stats {
    double mean;
    double median;
    double std_dev;
    double min;
    double max;
    double ci_lower;
    double ci_upper;
    size_t outliers;
    size_t sample_count;
    size_t iterations;
    double p75, p90, p95, p99;
    int throughput_type;
    uint64_t throughput_value;
} zap_stats_t;

typedef struct zap_baseline_entry {
    char name[256];
    double mean;
    double std_dev;
    double ci_lower;
    double ci_upper;
} zap_baseline_entry_t;

typedef struct zap_baseline {
    zap_baseline_entry_t* entries;
    size_t count;
    size_t capacity;
} zap_baseline_t;

void zap_baseline_init(zap_baseline_t* b);
void zap_baseline_free(zap_baseline_t* b);
void zap_baseline_add(zap_baseline_t* b, const char* name, const zap_stats_t* stats);
const zap_baseline_entry_t* zap_baseline_find(const zap_baseline_t* b, const char* name);
bool zap_baseline_save(const zap_baseline_t* b, const char* path);
bool zap_baseline_load(zap_baseline_t* b, const char* path);

// Helper to create stats
static zap_stats_t make_stats(double mean, double std_dev) {
    zap_stats_t s = {0};
    s.mean = mean;
    s.std_dev = std_dev;
    s.ci_lower = mean - std_dev * 2;
    s.ci_upper = mean + std_dev * 2;
    return s;
}

TEST(test_baseline_init_free) {
    zap_baseline_t b;
    zap_baseline_init(&b);
    ASSERT_EQ(b.count, 0);
    ASSERT(b.capacity > 0);  // Preallocated
    ASSERT(b.entries != NULL);
    zap_baseline_free(&b);
}

TEST(test_baseline_add_find) {
    zap_baseline_t b;
    zap_baseline_init(&b);

    zap_stats_t stats = make_stats(100.0, 5.0);
    zap_baseline_add(&b, "test_bench", &stats);

    ASSERT_EQ(b.count, 1);

    const zap_baseline_entry_t* e = zap_baseline_find(&b, "test_bench");
    ASSERT(e != NULL);
    ASSERT_STREQ(e->name, "test_bench");
    ASSERT_NEAR(e->mean, 100.0, 0.001);

    zap_baseline_free(&b);
}

TEST(test_baseline_find_not_found) {
    zap_baseline_t b;
    zap_baseline_init(&b);

    zap_stats_t stats = make_stats(100.0, 5.0);
    zap_baseline_add(&b, "test_bench", &stats);

    const zap_baseline_entry_t* e = zap_baseline_find(&b, "nonexistent");
    ASSERT(e == NULL);

    zap_baseline_free(&b);
}

TEST(test_baseline_group_prefix_no_collision) {
    // Test that same benchmark name in different groups don't collide
    zap_baseline_t b;
    zap_baseline_init(&b);

    zap_stats_t stats1 = make_stats(100.0, 5.0);
    zap_stats_t stats2 = make_stats(200.0, 10.0);

    // Simulate two groups with same benchmark name
    zap_baseline_add(&b, "group_a/bench_test", &stats1);
    zap_baseline_add(&b, "group_b/bench_test", &stats2);

    ASSERT_EQ(b.count, 2);

    // Verify each can be found independently
    const zap_baseline_entry_t* e1 = zap_baseline_find(&b, "group_a/bench_test");
    const zap_baseline_entry_t* e2 = zap_baseline_find(&b, "group_b/bench_test");

    ASSERT(e1 != NULL);
    ASSERT(e2 != NULL);
    ASSERT_NEAR(e1->mean, 100.0, 0.001);
    ASSERT_NEAR(e2->mean, 200.0, 0.001);

    zap_baseline_free(&b);
}

TEST(test_baseline_update_existing) {
    zap_baseline_t b;
    zap_baseline_init(&b);

    zap_stats_t stats1 = make_stats(100.0, 5.0);
    zap_stats_t stats2 = make_stats(150.0, 7.0);

    zap_baseline_add(&b, "group/bench", &stats1);
    ASSERT_EQ(b.count, 1);

    // Adding same key should update, not add
    zap_baseline_add(&b, "group/bench", &stats2);
    ASSERT_EQ(b.count, 1);

    const zap_baseline_entry_t* e = zap_baseline_find(&b, "group/bench");
    ASSERT(e != NULL);
    ASSERT_NEAR(e->mean, 150.0, 0.001);

    zap_baseline_free(&b);
}

TEST(test_baseline_comparison_api_format) {
    // Test the comparison API key format: group/label/param [impl]
    zap_baseline_t b;
    zap_baseline_init(&b);

    zap_stats_t stats1 = make_stats(100.0, 5.0);
    zap_stats_t stats2 = make_stats(200.0, 10.0);

    zap_baseline_add(&b, "sorting/sort/1000 [quicksort]", &stats1);
    zap_baseline_add(&b, "sorting/sort/1000 [mergesort]", &stats2);

    ASSERT_EQ(b.count, 2);

    const zap_baseline_entry_t* e1 = zap_baseline_find(&b, "sorting/sort/1000 [quicksort]");
    const zap_baseline_entry_t* e2 = zap_baseline_find(&b, "sorting/sort/1000 [mergesort]");

    ASSERT(e1 != NULL);
    ASSERT(e2 != NULL);
    ASSERT_NEAR(e1->mean, 100.0, 0.001);
    ASSERT_NEAR(e2->mean, 200.0, 0.001);

    zap_baseline_free(&b);
}

TEST(test_baseline_save_load) {
    const char* test_path = "/tmp/zap_test_baseline.txt";

    // Create and save
    zap_baseline_t b1;
    zap_baseline_init(&b1);

    zap_stats_t stats1 = make_stats(100.0, 5.0);
    zap_stats_t stats2 = make_stats(200.0, 10.0);

    zap_baseline_add(&b1, "group_a/bench_test", &stats1);
    zap_baseline_add(&b1, "group_b/bench_test", &stats2);

    bool saved = zap_baseline_save(&b1, test_path);
    ASSERT(saved);

    // Load into new baseline
    zap_baseline_t b2;
    zap_baseline_init(&b2);

    bool loaded = zap_baseline_load(&b2, test_path);
    ASSERT(loaded);
    ASSERT_EQ(b2.count, 2);

    // Verify contents preserved
    const zap_baseline_entry_t* e1 = zap_baseline_find(&b2, "group_a/bench_test");
    const zap_baseline_entry_t* e2 = zap_baseline_find(&b2, "group_b/bench_test");

    ASSERT(e1 != NULL);
    ASSERT(e2 != NULL);
    ASSERT_NEAR(e1->mean, 100.0, 0.001);
    ASSERT_NEAR(e2->mean, 200.0, 0.001);

    zap_baseline_free(&b1);
    zap_baseline_free(&b2);

    // Cleanup
    unlink(test_path);
}

TEST(test_baseline_load_nonexistent) {
    zap_baseline_t b;
    zap_baseline_init(&b);

    bool loaded = zap_baseline_load(&b, "/tmp/nonexistent_baseline_file.txt");
    ASSERT(loaded == false);
    ASSERT_EQ(b.count, 0);

    zap_baseline_free(&b);
}

void test_baseline(void) {
    RUN_TEST(test_baseline_init_free);
    RUN_TEST(test_baseline_add_find);
    RUN_TEST(test_baseline_find_not_found);
    RUN_TEST(test_baseline_group_prefix_no_collision);
    RUN_TEST(test_baseline_update_existing);
    RUN_TEST(test_baseline_comparison_api_format);
    RUN_TEST(test_baseline_save_load);
    RUN_TEST(test_baseline_load_nonexistent);
}
