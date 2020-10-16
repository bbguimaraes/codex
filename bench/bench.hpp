#ifndef CODEX_BENCH_BENCH_H
#define CODEX_BENCH_BENCH_H

namespace codex {

/**
 * Environment variable used to override the input sizes in benchmarks.
 * This variable should contain a comma-separated list of sizes followed by
 * tags, e.g.: `1,tag1,2,tag2,3,tag3`.
 */
constexpr auto BENCHMARK_SIZES_ENV = "CODEX_BENCHMARK_SIZES";

/**
 * Calls `QTest::newRow` for each size/tag pair in the environment variable.
 * See \ref BENCHMARK_SIZES_ENV for a description of the format.  This function
 * must be called from a QTest test data function that has already declared it
 * has a single `std::size_t` column.
 * \return `true` if variable contains a valid value and rows were added.
 */
bool set_benchmark_sizes_from_env(const char *env);

}

#endif
