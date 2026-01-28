#define ZAP_IMPLEMENTATION
#include "zap.h"

#include <stdlib.h>
#include <string.h>

/* Example benchmark: Empty loop baseline */
void bench_empty(zap_t* z) {
    ZAP_ITER(z) {
        /* Empty - measures loop overhead */
    }
}

/* Example benchmark: Simple arithmetic */
void bench_arithmetic(zap_t* z) {
    int x = 0;
    ZAP_ITER(z) {
        x = x + 1;
        x = x * 2;
        x = x - 1;
        x = x / 2;
        zap_black_box(x);
    }
}

/* Example benchmark: Fibonacci (iterative) */
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

void bench_fibonacci(zap_t* z) {
    int n = z->param ? *(int*)z->param : 10;
    int result;
    ZAP_ITER(z) {
        result = fibonacci(n);
        zap_black_box(result);
    }
}

/* Example benchmark: Pure computation (should produce Gaussian distribution) */
void bench_compute(zap_t* z) {
    /* Fixed-size computation with no system calls or allocations */
    double data[256];
    for (int i = 0; i < 256; i++) {
        data[i] = (double)i * 0.01;
    }
    zap_black_box(data);

    ZAP_ITER(z) {
        double sum = 0.0;
        for (int i = 0; i < 256; i++) {
            sum += data[i] * data[i];
        }
        zap_black_box(sum);
    }
}

/* Example benchmark: Memory allocation */
void bench_malloc(zap_t* z) {
    size_t size = z->param ? *(size_t*)z->param : 64;
    ZAP_ITER(z) {
        void* p = malloc(size);
        zap_black_box(p);
        free(p);
    }
}

/* Example benchmark: Memory copy with throughput reporting */
#define COPY_SIZE (1024 * 1024)  /* 1 MB */

void bench_memcpy_1mb(zap_t* z) {
    char* src = (char*)malloc(COPY_SIZE);
    char* dst = (char*)malloc(COPY_SIZE);
    memset(src, 'x', COPY_SIZE);

    /* Set throughput: each iteration copies COPY_SIZE bytes */
    zap_set_throughput_bytes(z, COPY_SIZE);

    ZAP_ITER(z) {
        memcpy(dst, src, COPY_SIZE);
        zap_black_box(dst);
    }

    free(src);
    free(dst);
}

void bench_memset_1mb(zap_t* z) {
    char* dst = (char*)malloc(COPY_SIZE);

    /* Set throughput: each iteration writes COPY_SIZE bytes */
    zap_set_throughput_bytes(z, COPY_SIZE);

    ZAP_ITER(z) {
        memset(dst, 'x', COPY_SIZE);
        zap_black_box(dst);
    }

    free(dst);
}

ZAP_MAIN {
    /* Overhead benchmarks */
    zap_runtime_group_t* overhead = zap_benchmark_group("overhead");
    zap_bench_function(overhead, "empty", bench_empty);
    zap_bench_function(overhead, "arithmetic", bench_arithmetic);
    zap_group_finish(overhead);

    /* Compute benchmark */
    zap_runtime_group_t* compute = zap_benchmark_group("compute");
    zap_bench_function(compute, "compute", bench_compute);
    zap_group_finish(compute);

    /* Fibonacci with different input sizes */
    zap_runtime_group_t* fib = zap_benchmark_group("fibonacci");
    int fib_sizes[] = {10, 20, 30};
    for (size_t i = 0; i < sizeof(fib_sizes)/sizeof(fib_sizes[0]); i++) {
        zap_bench_with_input(fib, zap_benchmark_id("fib", fib_sizes[i]),
                             &fib_sizes[i], sizeof(int), bench_fibonacci);
    }
    zap_group_finish(fib);

    /* Memory allocation with different sizes */
    zap_runtime_group_t* memory = zap_benchmark_group("memory");
    size_t malloc_sizes[] = {64, 1024, 65536};
    for (size_t i = 0; i < sizeof(malloc_sizes)/sizeof(malloc_sizes[0]); i++) {
        char label[32];
        if (malloc_sizes[i] >= 1024) {
            snprintf(label, sizeof(label), "%zuKB", malloc_sizes[i] / 1024);
        } else {
            snprintf(label, sizeof(label), "%zuB", malloc_sizes[i]);
        }
        zap_bench_with_input(memory, zap_benchmark_id_str("malloc", label),
                             &malloc_sizes[i], sizeof(size_t), bench_malloc);
    }
    zap_group_finish(memory);

    /* Throughput demo */
    zap_runtime_group_t* throughput = zap_benchmark_group("throughput");
    zap_bench_function(throughput, "memcpy_1mb", bench_memcpy_1mb);
    zap_bench_function(throughput, "memset_1mb", bench_memset_1mb);
    zap_group_finish(throughput);
}
