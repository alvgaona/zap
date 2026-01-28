# Tasks

- [x] ZAP-001: Add group name prefix to baseline keys to avoid collisions when multiple groups have benchmarks with the same name.
- [x] ZAP-002: Add unit tests for baseline storage with group prefix feature.
- [ ] ZAP-003: Add memory leak detection to test framework to catch leaks on early ASSERT returns.
- [ ] ZAP-004: Suppress "Loaded baseline" message during tests.
- [ ] ZAP-005: Add baseline migration tool to convert old baseline files to new group-prefixed format.
- [ ] ZAP-006: Add JSON format option for baseline files.
- [ ] ZAP-007: Add benchmark result history to track multiple runs over time.
- [ ] ZAP-008: Add HTML report generation with charts and trends.
- [ ] ZAP-009: Make outlier detection method configurable between IQR and Z-score via `ZAP_OUTLIER_METHOD`, with tunable parameters (`ZAP_OUTLIER_IQR_FACTOR` default 1.5, `ZAP_OUTLIER_ZSCORE_THRESHOLD` default 3.5).
- [ ] ZAP-010: Make max iterations cap configurable via `ZAP_MAX_ITERATIONS` (default 1 billion).
- [ ] ZAP-011: Make histogram bins and height configurable via `ZAP_HISTOGRAM_MAX_BINS` and `ZAP_HISTOGRAM_HEIGHT`.
- [ ] ZAP-012: Make default baseline path configurable via `ZAP_DEFAULT_BASELINE_PATH` (default ".zap/baseline").
- [ ] ZAP-013: Add Windows support using `QueryPerformanceCounter` for high-precision timing.
- [ ] ZAP-014: Add CPU cycle counting via `rdtsc` as alternative timing mode.
- [ ] ZAP-015: Add CSV export option via `--csv` flag or `ZAP_OUTPUT_CSV`.
- [ ] ZAP-016: Add git commit comparison to track performance across commits in baseline.
- [ ] ZAP-017: Make percentiles configurable via `ZAP_PERCENTILES` instead of hardcoded p75/p90/p95/p99.
- [ ] ZAP-018: Add progress bar with ETA for benchmark groups when running long suites.
- [ ] ZAP-019: Add bootstrap confidence intervals as alternative to t-distribution CI via `ZAP_CI_METHOD`.
- [ ] ZAP-020: Add gnuplot output support via `--gnuplot` to generate data files and plot scripts.

## API Simplification (v2.0)

- [x] ZAP-021: Rename `struct criterion` to `struct zap` for consistency.
- [x] ZAP-022: Unify `zap_t` and `zap_bencher_t` by adding `void* param` and `size_t param_size` fields to `zap_t`.
- [x] ZAP-023: Remove `ZAP_ITER` macro - make `ZAP_LOOP` work with the unified `zap_t` (which now has param).
- [x] ZAP-024: Remove `zap_bencher_set_throughput_bytes` and `zap_bencher_set_throughput_elements` - use `zap_set_throughput_*` everywhere.
- [x] ZAP-025: Add auto-finalize via `atexit()` registration in `zap_parse_args()`.
- [x] ZAP-026: Remove `ZAP_GROUP` macro and `zap_run_group_internal` function.
- [x] ZAP-027: Change `ZAP_MAIN` to block-style macro that works with runtime API: `ZAP_MAIN { ... }`.
- [x] ZAP-028: Update `zap_bench_function` and `zap_bench_with_input` to use unified `zap_t` instead of `zap_bencher_t`.
- [x] ZAP-029: Update comparison API (`zap_compare_impl`) to use unified `zap_t`.
- [x] ZAP-030: Update all examples to use new unified API.
- [x] ZAP-031: Update tests for new API.
- [x] ZAP-032: Update CLAUDE.md documentation to reflect API changes.
