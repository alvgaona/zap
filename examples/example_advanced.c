/*
 * Advanced example demonstrating criterion-rs style features:
 * - Runtime benchmark groups
 * - Per-group configuration
 * - Parameterized benchmarks
 * - ZAP_ITER macro
 */

#define ZAP_IMPLEMENTATION
#include "zap.h"

#include <stdlib.h>
#include <string.h>

/* ========================================================================== */
/* Helper functions                                                           */
/* ========================================================================== */

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

/* ========================================================================== */
/* Traditional style (static groups with ZAP_LOOP)                      */
/* ========================================================================== */

void bench_fib_10(zap_t* c) {
    int n = 10;
    zap_black_box(n);
    int result;
    ZAP_LOOP(c) {
        result = fibonacci(n);
        zap_black_box(result);
    }
}

void bench_fib_20(zap_t* c) {
    int n = 20;
    zap_black_box(n);
    int result;
    ZAP_LOOP(c) {
        result = fibonacci(n);
        zap_black_box(result);
    }
}

ZAP_GROUP(traditional_benches, bench_fib_10, bench_fib_20);

/* ========================================================================== */
/* Runtime group with configuration (criterion-rs style)                      */
/* ========================================================================== */

static void iter_fib(zap_bencher_t* b, void* param) {
    int n = *(int*)param;
    int result;
    ZAP_ITER(b, {
        result = fibonacci(n);
        zap_black_box(result);
    });
}

static void bench_fibonacci_group(void) {
    /* Create a runtime group with custom configuration */
    zap_runtime_group_t* group = zap_benchmark_group("fibonacci");

    /* Set shorter times for faster testing */
    zap_group_warmup_time(group, ZAP_MILLIS(500));
    zap_group_measurement_time(group, ZAP_SECONDS(2));
    zap_group_sample_count(group, 50);

    /* Parameterized benchmarks - test multiple input sizes */
    int sizes[] = {5, 10, 15, 20, 25, 30};
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (size_t i = 0; i < num_sizes; i++) {
        zap_benchmark_id_t id = zap_benchmark_id("fib", sizes[i]);
        zap_bench_with_input(group, id, &sizes[i], sizeof(int), iter_fib);
    }

    zap_group_finish(group);
}

/* ========================================================================== */
/* Sorting benchmarks with parameterized sizes                                */
/* ========================================================================== */

typedef struct {
    int* data;
    int* work;  /* Working copy for sorting */
    size_t size;
} sort_input_t;

static void iter_sort(zap_bencher_t* b, void* param) {
    sort_input_t* input = (sort_input_t*)param;

    ZAP_ITER(b, {
        /* Copy data to work buffer before each sort */
        memcpy(input->work, input->data, input->size * sizeof(int));
        bubble_sort(input->work, input->size);
        zap_black_box(input->work);
    });
}

static void bench_sorting_group(void) {
    zap_runtime_group_t* group = zap_benchmark_group("sorting");

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

        zap_benchmark_id_t id = zap_benchmark_id("bubble_sort", (int64_t)n);
        zap_bench_with_input(group, id, &input, sizeof(input), iter_sort);

        free(input.data);
        free(input.work);
    }

    zap_group_finish(group);
}

/* ========================================================================== */
/* Memory allocation benchmarks                                               */
/* ========================================================================== */

static void iter_malloc(zap_bencher_t* b, void* param) {
    size_t size = *(size_t*)param;

    ZAP_ITER(b, {
        void* p = malloc(size);
        zap_black_box(p);
        free(p);
    });
}

static void bench_memory_group(void) {
    zap_runtime_group_t* group = zap_benchmark_group("memory");

    size_t sizes[] = {64, 256, 1024, 4096, 16384, 65536};
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (size_t i = 0; i < num_sizes; i++) {
        char param_str[32];
        if (sizes[i] >= 1024) {
            snprintf(param_str, sizeof(param_str), "%zuKB", sizes[i] / 1024);
        } else {
            snprintf(param_str, sizeof(param_str), "%zuB", sizes[i]);
        }

        zap_benchmark_id_t id = zap_benchmark_id_str("malloc", param_str);
        zap_bench_with_input(group, id, &sizes[i], sizeof(size_t), iter_malloc);
    }

    zap_group_finish(group);
}

/* ========================================================================== */
/* Main entry point                                                           */
/* ========================================================================== */

int main(int argc, char** argv) {
    zap_parse_args(argc, argv);

    /* Run traditional static group */
    zap_run_group_internal(&traditional_benches);

    /* Run runtime groups with parameterized benchmarks */
    bench_fibonacci_group();
    bench_sorting_group();
    bench_memory_group();

    zap_finalize();
    return 0;
}
