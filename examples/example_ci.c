/*
 * CI/Headless example - optimized for automated testing
 *
 * Compile: gcc -I.. -o ci example_ci.c -lm
 * Run: ./ci --json --fail-threshold 5
 */

/* CI-optimized defaults: no colors, high precision */
#define ZAP_DEFAULT_COLOR_MODE 2                     /* never */
#define ZAP_DEFAULT_SAMPLE_COUNT 200
#define ZAP_DEFAULT_MEASUREMENT_TIME_NS 5000000000   /* 5s */

#define ZAP_IMPLEMENTATION
#include "zap.h"

#include <stdlib.h>

/* Benchmark: allocation performance */
void bench_malloc_64(zap_t* c) {
    ZAP_LOOP(c) {
        void* p = malloc(64);
        zap_black_box(p);
        free(p);
    }
}

void bench_malloc_1024(zap_t* c) {
    ZAP_LOOP(c) {
        void* p = malloc(1024);
        zap_black_box(p);
        free(p);
    }
}

void bench_malloc_4096(zap_t* c) {
    ZAP_LOOP(c) {
        void* p = malloc(4096);
        zap_black_box(p);
        free(p);
    }
}

ZAP_GROUP(allocation, bench_malloc_64, bench_malloc_1024, bench_malloc_4096);
ZAP_MAIN(allocation);
