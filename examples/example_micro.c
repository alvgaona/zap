/*
 * Micro-benchmark example - for very fast operations
 *
 * Compile: gcc -I.. -o micro example_micro.c -lm
 * Run: ./micro
 *
 * Uses minimum iterations to ensure accurate timing of fast code.
 */

/* Micro-benchmark defaults: ensure enough iterations */
#define ZAP_DEFAULT_MIN_ITERS 10000
#define ZAP_DEFAULT_SAMPLE_COUNT 100
#define ZAP_DEFAULT_SHOW_PERCENTILES 1

#define ZAP_IMPLEMENTATION
#include "zap.h"

/* Benchmark: empty loop (measures overhead) */
void bench_noop(zap_t* c) {
    ZAP_LOOP(c) {
        /* Empty - measures loop overhead */
    }
}

/* Benchmark: single integer operation */
void bench_int_add(zap_t* c) {
    int x = 0;
    zap_black_box(x);
    ZAP_LOOP(c) {
        x = x + 1;
        zap_black_box(x);
    }
}

/* Benchmark: single multiply */
void bench_int_mul(zap_t* c) {
    int x = 1;
    zap_black_box(x);
    ZAP_LOOP(c) {
        x = x * 3;
        zap_black_box(x);
    }
}

/* Benchmark: single division */
void bench_int_div(zap_t* c) {
    int x = 1000000;
    zap_black_box(x);
    ZAP_LOOP(c) {
        x = x / 2;
        if (x == 0) x = 1000000;
        zap_black_box(x);
    }
}

/* Benchmark: floating point */
void bench_float_mul(zap_t* c) {
    double x = 1.5;
    zap_black_box(x);
    ZAP_LOOP(c) {
        x = x * 1.000001;
        zap_black_box(x);
    }
}

ZAP_GROUP(micro, bench_noop, bench_int_add, bench_int_mul, bench_int_div, bench_float_mul);
ZAP_MAIN(micro);
