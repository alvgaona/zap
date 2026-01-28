/*
 * Verbose output example - shows all details by default
 *
 * Compile: gcc -I.. -o verbose example_verbose.c -lm
 * Run: ./verbose
 */

/* Enable all output by default */
#define ZAP_DEFAULT_SHOW_ENV 1
#define ZAP_DEFAULT_SHOW_HISTOGRAM 1
#define ZAP_DEFAULT_SHOW_PERCENTILES 1

#define ZAP_IMPLEMENTATION
#include "zap.h"

#include <stdlib.h>
#include <string.h>

/* Benchmark: memory copy */
#define COPY_SIZE 4096

void bench_memcpy(zap_t* z) {
    char* src = (char*)malloc(COPY_SIZE);
    char* dst = (char*)malloc(COPY_SIZE);
    memset(src, 'x', COPY_SIZE);

    zap_set_throughput_bytes(z, COPY_SIZE);

    ZAP_ITER(z) {
        memcpy(dst, src, COPY_SIZE);
        zap_black_box(dst);
    }

    free(src);
    free(dst);
}

/* Benchmark: computation */
void bench_compute(zap_t* z) {
    double data[64];
    for (int i = 0; i < 64; i++) {
        data[i] = (double)i * 0.01;
    }
    zap_black_box(data);

    ZAP_ITER(z) {
        double sum = 0.0;
        for (int i = 0; i < 64; i++) {
            sum += data[i] * data[i];
        }
        zap_black_box(sum);
    }
}

ZAP_MAIN {
    zap_runtime_group_t* g = zap_benchmark_group("verbose_benches");
    zap_bench_function(g, "bench_memcpy", bench_memcpy);
    zap_bench_function(g, "bench_compute", bench_compute);
    zap_group_finish(g);
}
