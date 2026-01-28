/*
 * example_compare.c - Demonstration of ZAP's comparison API
 *
 * This example shows how to compare multiple implementations of the same
 * algorithm across different input sizes. It also demonstrates comparison
 * with previous runs via baseline files.
 *
 * Build: gcc -O3 -o example_compare examples/example_compare.c -lm
 * Run:   ./example_compare
 *        ./example_compare --env
 *        ./example_compare --json
 */

#define ZAP_IMPLEMENTATION
#include "zap.h"

#include <stdlib.h>
#include <string.h>

// Context passed to benchmark functions
typedef struct {
    int* arr;
    size_t n;
} sort_ctx_t;

// Simple comparison function for qsort
static int cmp_int(const void* a, const void* b) {
    return (*(const int*)a) - (*(const int*)b);
}

// Fill array with random data
static void fill_random(int* arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        arr[i] = rand() % 10000;
    }
}

// Reset array to unsorted state (copy from backup)
static int* g_backup = NULL;

static void reset_array(int* arr, size_t n) {
    memcpy(arr, g_backup, n * sizeof(int));
}

/*
 * Implementation 1: Standard library qsort
 */
void bench_qsort(zap_t* z) {
    sort_ctx_t* ctx = (sort_ctx_t*)z->param;

    ZAP_ITER(z) {
        reset_array(ctx->arr, ctx->n);
        qsort(ctx->arr, ctx->n, sizeof(int), cmp_int);
        zap_black_box(ctx->arr);
    }
}

/*
 * Implementation 2: Simple bubble sort (for comparison)
 */
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

void bench_bubble(zap_t* z) {
    sort_ctx_t* ctx = (sort_ctx_t*)z->param;

    ZAP_ITER(z) {
        reset_array(ctx->arr, ctx->n);
        bubble_sort(ctx->arr, ctx->n);
        zap_black_box(ctx->arr);
    }
}

/*
 * Implementation 3: Insertion sort
 */
static void insertion_sort(int* arr, size_t n) {
    for (size_t i = 1; i < n; i++) {
        int key = arr[i];
        size_t j = i;
        while (j > 0 && arr[j - 1] > key) {
            arr[j] = arr[j - 1];
            j--;
        }
        arr[j] = key;
    }
}

void bench_insertion(zap_t* z) {
    sort_ctx_t* ctx = (sort_ctx_t*)z->param;

    ZAP_ITER(z) {
        reset_array(ctx->arr, ctx->n);
        insertion_sort(ctx->arr, ctx->n);
        zap_black_box(ctx->arr);
    }
}

/*
 * Memory copy comparison
 */
typedef struct {
    char* src;
    char* dst;
    size_t n;
} memcpy_ctx_t;

void bench_memcpy(zap_t* z) {
    memcpy_ctx_t* ctx = (memcpy_ctx_t*)z->param;

    // Report throughput in bytes/sec
    zap_set_throughput_bytes(z, ctx->n);

    ZAP_ITER(z) {
        memcpy(ctx->dst, ctx->src, ctx->n);
        zap_black_box(ctx->dst);
    }
}

void bench_memmove(zap_t* z) {
    memcpy_ctx_t* ctx = (memcpy_ctx_t*)z->param;

    // Report throughput in bytes/sec
    zap_set_throughput_bytes(z, ctx->n);

    ZAP_ITER(z) {
        memmove(ctx->dst, ctx->src, ctx->n);
        zap_black_box(ctx->dst);
    }
}

void bench_manual_copy(zap_t* z) {
    memcpy_ctx_t* ctx = (memcpy_ctx_t*)z->param;

    // Report throughput in bytes/sec
    zap_set_throughput_bytes(z, ctx->n);

    ZAP_ITER(z) {
        for (size_t i = 0; i < ctx->n; i++) {
            ctx->dst[i] = ctx->src[i];
        }
        zap_black_box(ctx->dst);
    }
}

int main(int argc, char** argv) {
    zap_parse_args(argc, argv);

    // Seed random for reproducibility
    srand(42);

    /*
     * Comparison group 1: Sorting algorithms
     * qsort is set as the baseline (index 0)
     */
    zap_compare_group_t* sort_group = zap_compare_group("sort");
    zap_compare_set_baseline(sort_group, 0);  // qsort is baseline

    // Test with different array sizes
    size_t sort_sizes[] = {100, 500, 1000};
    size_t num_sort_sizes = sizeof(sort_sizes) / sizeof(sort_sizes[0]);

    for (size_t s = 0; s < num_sort_sizes; s++) {
        size_t n = sort_sizes[s];

        // Allocate arrays
        int* arr = (int*)malloc(n * sizeof(int));
        g_backup = (int*)malloc(n * sizeof(int));

        // Initialize with random data
        fill_random(g_backup, n);
        memcpy(arr, g_backup, n * sizeof(int));

        sort_ctx_t ctx = { arr, n };

        // Begin comparison for this size
        zap_compare_ctx_t* cmp = zap_compare_begin(
            sort_group,
            zap_benchmark_id("n", (int64_t)n),
            &ctx, sizeof(ctx)
        );

        // Run each implementation
        zap_compare_impl(cmp, "qsort", bench_qsort);
        zap_compare_impl(cmp, "insertion", bench_insertion);

        // Only run bubble sort for small sizes (it's too slow for large)
        if (n <= 500) {
            zap_compare_impl(cmp, "bubble", bench_bubble);
        }

        // End comparison and print results
        zap_compare_end(cmp);

        // Cleanup
        free(arr);
        free(g_backup);
        g_backup = NULL;
    }

    zap_compare_group_finish(sort_group);

    /*
     * Comparison group 2: Memory copy implementations
     * memcpy is set as the baseline
     */
    zap_compare_group_t* memcpy_group = zap_compare_group("memcpy");
    zap_compare_set_baseline(memcpy_group, 0);  // memcpy is baseline
    zap_compare_tag(memcpy_group, "memory");

    size_t copy_sizes[] = {1024, 4096, 65536};
    size_t num_copy_sizes = sizeof(copy_sizes) / sizeof(copy_sizes[0]);

    for (size_t s = 0; s < num_copy_sizes; s++) {
        size_t n = copy_sizes[s];

        char* src = (char*)malloc(n);
        char* dst = (char*)malloc(n);
        memset(src, 'x', n);

        memcpy_ctx_t ctx = { src, dst, n };

        // Format size nicely for benchmark ID
        char size_str[32];
        if (n >= 1024) {
            snprintf(size_str, sizeof(size_str), "%zuKB", n / 1024);
        } else {
            snprintf(size_str, sizeof(size_str), "%zuB", n);
        }

        zap_compare_ctx_t* cmp = zap_compare_begin(
            memcpy_group,
            zap_benchmark_id_str("size", size_str),
            &ctx, sizeof(ctx)
        );

        zap_compare_impl(cmp, "memcpy", bench_memcpy);
        zap_compare_impl(cmp, "memmove", bench_memmove);
        zap_compare_impl(cmp, "manual", bench_manual_copy);

        zap_compare_end(cmp);

        free(src);
        free(dst);
    }

    zap_compare_group_finish(memcpy_group);

    return 0;
}
