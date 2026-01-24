#define ZAP_IMPLEMENTATION
#include "zap.h"

#include <stdlib.h>

/* Example benchmark: Empty loop baseline */
void bench_empty(zap_t* c) {
    ZAP_LOOP(c) {
        /* Empty - measures loop overhead */
    }
}

/* Example benchmark: Simple arithmetic */
void bench_arithmetic(zap_t* c) {
    int x = 0;
    ZAP_LOOP(c) {
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

void bench_fibonacci_10(zap_t* c) {
    int n = 10;
    zap_black_box(n);  /* Prevent compile-time evaluation */
    int result;
    ZAP_LOOP(c) {
        result = fibonacci(n);
        zap_black_box(result);
    }
}

void bench_fibonacci_20(zap_t* c) {
    int n = 20;
    zap_black_box(n);  /* Prevent compile-time evaluation */
    int result;
    ZAP_LOOP(c) {
        result = fibonacci(n);
        zap_black_box(result);
    }
}

/* Example benchmark: Pure computation (should produce Gaussian distribution) */
void bench_compute(zap_t* c) {
    /* Fixed-size computation with no system calls or allocations */
    double data[256];
    for (int i = 0; i < 256; i++) {
        data[i] = (double)i * 0.01;
    }
    zap_black_box(data);

    ZAP_LOOP(c) {
        double sum = 0.0;
        for (int i = 0; i < 256; i++) {
            sum += data[i] * data[i];
        }
        zap_black_box(sum);
    }
}

/* Example benchmark: Memory allocation */
void bench_malloc_small(zap_t* c) {
    ZAP_LOOP(c) {
        void* p = malloc(64);
        zap_black_box(p);
        free(p);
    }
}

void bench_malloc_large(zap_t* c) {
    ZAP_LOOP(c) {
        void* p = malloc(65536);
        zap_black_box(p);
        free(p);
    }
}

/* Define benchmark groups */
ZAP_GROUP(overhead, bench_empty, bench_arithmetic);
ZAP_GROUP(compute, bench_compute);
ZAP_GROUP(fib_benches, bench_fibonacci_10, bench_fibonacci_20);
ZAP_GROUP(memory, bench_malloc_small, bench_malloc_large);

/* Main entry point - runs all groups */
ZAP_MAIN(overhead, compute, fib_benches, memory);
