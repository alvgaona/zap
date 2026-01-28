/*
 * Quick iteration example - fast feedback during development
 *
 * Compile: gcc -I.. -o quick example_quick.c -lm
 * Run: ./quick
 */

/* Faster defaults for development iteration */
#define ZAP_DEFAULT_SAMPLE_COUNT 20
#define ZAP_DEFAULT_WARMUP_TIME_NS 500000000       /* 500ms */
#define ZAP_DEFAULT_MEASUREMENT_TIME_NS 1000000000 /* 1s */

#define ZAP_IMPLEMENTATION
#include "zap.h"

void bench_example(zap_t* z) {
    int x = 0;
    ZAP_LOOP(z) {
        x = x + 1;
        x = x * 2;
        zap_black_box(x);
    }
}

ZAP_MAIN {
    zap_runtime_group_t* g = zap_benchmark_group("quick_benches");
    zap_bench_function(g, "bench_example", bench_example);
    zap_group_finish(g);
}
