/*
 * Advanced example demonstrating:
 * - Runtime benchmark groups
 * - Per-group configuration
 * - Parameterized benchmarks
 * - Tags for filtering
 * - Setup/teardown hooks
 */

#define ZAP_IMPLEMENTATION
#include "zap.h"

#include <stdlib.h>
#include <string.h>

/* Helper functions */

static int fibonacci(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        int tmp = a + b;
        a = b;
        b = tmp;
    }
    return b;
}

static void fill_random(int* arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        arr[i] = rand() % 1000;
    }
}

static void bubble_sort(int* arr, size_t n) {
    for (size_t i = 0; i < n - 1; i++) {
        for (size_t j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
}

/* Fibonacci benchmark - uses z->param for input */
static void bench_fib(zap_t* z) {
    int n = z->param ? *(int*)z->param : 10;
    int result;
    ZAP_ITER(z) {
        result = fibonacci(n);
        zap_black_box(result);
    }
}

/* Sorting benchmark */
typedef struct {
    int* data;
    int* work;
    size_t size;
} sort_input_t;

static void bench_sort(zap_t* z) {
    sort_input_t* input = (sort_input_t*)z->param;

    ZAP_ITER(z) {
        memcpy(input->work, input->data, input->size * sizeof(int));
        bubble_sort(input->work, input->size);
        zap_black_box(input->work);
    }
}

/* Memory allocation benchmark */
static void bench_malloc(zap_t* z) {
    size_t size = z->param ? *(size_t*)z->param : 64;

    ZAP_ITER(z) {
        void* p = malloc(size);
        zap_black_box(p);
        free(p);
    }
}

/* Benchmark groups */

static void run_fibonacci_benchmarks(void) {
    zap_runtime_group_t* group = zap_benchmark_group("fibonacci");

    /* Tag this group for filtering */
    zap_group_tag(group, "fast");
    zap_group_tag(group, "cpu");

    /* Set shorter times for faster testing */
    zap_group_warmup_time(group, ZAP_MILLIS(500));
    zap_group_measurement_time(group, ZAP_SECONDS(2));
    zap_group_sample_count(group, 50);

    /* Parameterized benchmarks - test multiple input sizes */
    int sizes[] = {5, 10, 15, 20, 25, 30};
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (size_t i = 0; i < num_sizes; i++) {
        zap_bench_with_input(group, zap_benchmark_id("fib", sizes[i]),
                             &sizes[i], sizeof(int), bench_fib);
    }

    zap_group_finish(group);
}

static void run_sorting_benchmarks(void) {
    zap_runtime_group_t* group = zap_benchmark_group("sorting");

    /* Tag this group */
    zap_group_tag(group, "slow");
    zap_group_tag(group, "cpu");

    /* Use shorter times since sorting is slow */
    zap_group_warmup_time(group, ZAP_MILLIS(200));
    zap_group_measurement_time(group, ZAP_SECONDS(1));

    size_t sizes[] = {10, 50, 100, 200};
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (size_t i = 0; i < num_sizes; i++) {
        size_t n = sizes[i];

        /* Allocate and prepare input */
        sort_input_t input;
        input.size = n;
        input.data = malloc(n * sizeof(int));
        input.work = malloc(n * sizeof(int));
        fill_random(input.data, n);

        zap_bench_with_input(group, zap_benchmark_id("bubble_sort", (int64_t)n),
                             &input, sizeof(input), bench_sort);

        free(input.data);
        free(input.work);
    }

    zap_group_finish(group);
}

static void run_memory_benchmarks(void) {
    zap_runtime_group_t* group = zap_benchmark_group("memory");

    /* Tag this group */
    zap_group_tag(group, "fast");
    zap_group_tag(group, "alloc");

    size_t sizes[] = {64, 256, 1024, 4096, 16384, 65536};
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (size_t i = 0; i < num_sizes; i++) {
        char param_str[32];
        if (sizes[i] >= 1024) {
            snprintf(param_str, sizeof(param_str), "%zuKB", sizes[i] / 1024);
        } else {
            snprintf(param_str, sizeof(param_str), "%zuB", sizes[i]);
        }

        zap_bench_with_input(group, zap_benchmark_id_str("malloc", param_str),
                             &sizes[i], sizeof(size_t), bench_malloc);
    }

    zap_group_finish(group);
}

ZAP_MAIN {
    run_fibonacci_benchmarks();
    run_sorting_benchmarks();
    run_memory_benchmarks();
}
