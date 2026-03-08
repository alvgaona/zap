# Zap

[![ci](https://github.com/alvgaona/zap/actions/workflows/ci.yml/badge.svg)](https://github.com/alvgaona/zap/actions/workflows/ci.yml)
[![license](https://img.shields.io/github/license/alvgaona/zap)](LICENSE)
[![release](https://img.shields.io/github/v/release/alvgaona/zap)](https://github.com/alvgaona/zap/releases/latest)
[![prefix.dev](https://img.shields.io/badge/prefix.dev-zap-blue)](https://prefix.dev/channels/zap)

STB-style single-header benchmarking library for C, inspired by [criterion.rs](https://github.com/bheisler/criterion.rs).

## Usage

Copy `zap.h` into your project. In **one** C file:

```c
#define ZAP_IMPLEMENTATION
#include "zap.h"

void bench_example(zap_t* z) {
    ZAP_ITER(z) {
        int result = do_work();
        zap_black_box(result);
    }
}

ZAP_MAIN {
    zap_runtime_group_t* g = zap_benchmark_group("my_benches");
    zap_bench_function(g, "example", bench_example);
    zap_group_finish(g);
}
```

```bash
gcc -O2 -o bench bench.c -lm
./bench
```

## Sample Output

```
bench_example:
  20 samples × 1076000 evals, median: 1.003 ns
  Time  (mean ± σ):  1.017 ns ± 0.044 ns
  Range (min … max):  0.975 ns … 1.110 ns

   █
  ▆█ ▁▆    ▁  ▁  ▁ ▆        ▁            ▆   ▁  ▁       ▁
  0.98 ns       Histogram: frequency by time       1.11 ns
```

## Features

- High-precision timing (mach_time on macOS, clock_gettime on Linux)
- Statistical analysis with confidence intervals and outlier detection
- Baseline comparison with regression detection
- Throughput reporting (bytes/s, elements/s)
- Multi-implementation comparison API
- JSON output for CI integration
- No external dependencies (only `-lm`)

## CLI

```
--filter <pattern>    Filter benchmarks by name
--tag <tag>           Filter by group tag
--json                JSON output
--samples <n>         Sample count (default: 100)
--warmup <duration>   Warmup time (default: 1s)
--time <duration>     Measurement time (default: 3s)
--fail-threshold <n>  Exit 1 if regression exceeds n%
--histogram           Show distribution histograms
--percentiles         Show p75/p90/p95/p99
--env                 Show environment info
```

## License

Apache-2.0
