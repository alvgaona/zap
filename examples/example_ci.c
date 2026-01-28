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
void bench_malloc(zap_t* z) {
    size_t size = z->param ? *(size_t*)z->param : 64;
    ZAP_ITER(z) {
        void* p = malloc(size);
        zap_black_box(p);
        free(p);
    }
}

ZAP_MAIN {
    zap_runtime_group_t* g = zap_benchmark_group("allocation");

    size_t sizes[] = {64, 1024, 4096};
    for (size_t i = 0; i < sizeof(sizes)/sizeof(sizes[0]); i++) {
        char label[32];
        snprintf(label, sizeof(label), "malloc_%zu", sizes[i]);
        zap_bench_with_input(g, zap_benchmark_id_str("malloc", label),
                             &sizes[i], sizeof(size_t), bench_malloc);
    }

    zap_group_finish(g);
}
