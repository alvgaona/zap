# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.2.0] - 2025-01-24

### Added

#### Comparison API
- New API for comparing multiple implementations of the same operation
- `zap_compare_group()`: Create a comparison group
- `zap_compare_set_baseline()`: Set which implementation is the baseline
- `zap_compare_begin()`: Start a comparison with a benchmark ID and input
- `zap_compare_impl()`: Add an implementation to compare
- `zap_compare_end()`: Finish comparison and print results
- `zap_compare_group_finish()`: Clean up comparison group

#### Comparison Output
- Detailed per-implementation statistics (samples, evals, median, mean, stddev, range)
- Throughput reporting per implementation
- Speedup/slowdown vs baseline (e.g., "2.1x faster" or "0.8x (25% slower)")
- Speedup vs all other implementations
- Support for 2+ implementations (not limited to pairwise comparison)

#### Examples
- `example_compare.c`: Demonstrates the comparison API

### Changed
- Removed `[baseline]` label from comparison output for cleaner display

### Fixed
- Comparison results now show speedup vs all implementations, not just baseline

## [0.1.0] - 2025-01-24

Initial release of Zap, an STB-style single-header benchmarking library for C.

### Added

#### Core Benchmarking
- Single-header library (`zap.h`) with no external dependencies (except `-lm`)
- High-precision timing using platform-native APIs (mach_time on macOS, clock_gettime on Linux)
- Automatic warmup phase with iteration calibration
- Configurable measurement time and sample count
- Black box to prevent compiler optimization of benchmarked code

#### Two Programming Styles
- **Static/macro-based**: `ZAP_LOOP`, `ZAP_GROUP`, `ZAP_MAIN` macros
- **Runtime/criterion-rs style**: `zap_benchmark_group()`, `zap_bench_function()`, `zap_bench_with_input()`

#### Statistical Analysis
- Mean, median, standard deviation
- Percentiles (p1, p5, p25, p50, p75, p95, p99)
- 95% confidence intervals with t-distribution
- Outlier detection using MAD (Median Absolute Deviation)

#### Baseline Comparison
- Save/load baselines to `.zap/baseline`
- Statistical significance testing
- Regression detection with percentage change reporting

#### Throughput Reporting
- Bytes per second (`zap_set_throughput_bytes`)
- Elements per second (`zap_set_throughput_elements`)

#### Output Formats
- Text output with ANSI colors (synthwave neon palette)
- JSON output (`--json`)
- Environment info display (`--env`)
- Histogram visualization (`--histogram`)
- Percentile tables (`--percentiles`)

#### CLI Options
- `--filter` / `-f`: Filter benchmarks by pattern (glob wildcards supported)
- `--tag` / `-t`: Filter by benchmark tags
- `--json`: JSON output format
- `--color` / `--no-color`: Color output control
- `--warmup`: Custom warmup duration
- `--time`: Custom measurement duration
- `--samples`: Custom sample count
- `--baseline`: Custom baseline file path
- `--no-save`: Skip saving results
- `--no-compare`: Skip baseline comparison
- `--dry-run`: List benchmarks without running

#### Configuration
- Compile-time defaults via macros:
  - `ZAP_DEFAULT_SAMPLE_COUNT`
  - `ZAP_DEFAULT_WARMUP_TIME_NS`
  - `ZAP_DEFAULT_MEASUREMENT_TIME_NS`
  - `ZAP_DEFAULT_MIN_ITERS`
  - `ZAP_DEFAULT_SHOW_ENV`
  - `ZAP_DEFAULT_SHOW_HISTOGRAM`
  - `ZAP_DEFAULT_SHOW_PERCENTILES`
  - `ZAP_DEFAULT_COLOR_MODE`

#### Setup/Teardown Hooks
- Per-group setup and teardown functions
- `zap_group_setup()` and `zap_group_teardown()`

#### Benchmark Tagging
- Tag groups with `zap_group_tag()`
- Filter by tags with `--tag`

#### Environment Detection
- CPU model and core count
- OS information
- Compiler detection

#### Build System
- Makefile with `make`, `make test`, `make run`
- Example filtering with `make run E=<name>`

#### Testing
- Unit test framework in `tests/`
- Tests for statistics, filter matching
- GitHub Actions CI workflow

### Platform Support
- macOS (Apple Silicon and Intel)
- Linux (x86_64, ARM64)
- GCC and Clang compilers
