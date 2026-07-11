#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include <benchmark.h>
#include <benchmark_stats.h>
#include <config.h>

static void test_benchmark_config_default_values() {
    init_config();
    config_t *cfg = get_config();

    TEST_ASSERT_EQUAL(0, cfg->do_benchmark_fast);
    TEST_ASSERT_EQUAL(0, cfg->do_benchmark_slow);
    TEST_ASSERT_NULL(cfg->compare_against);
    TEST_ASSERT_NULL(cfg->compare_benchmarks);
}

static void test_compare_benchmark_files_missing() {
    compare_benchmark_files("/tmp/nonexistent_file_1.json", "/tmp/nonexistent_file_2.json");
}

static void test_compare_benchmark_files_valid() {
    FILE *f1 = fopen("/tmp/test_benchmark_1.json", "w");
    FILE *f2 = fopen("/tmp/test_benchmark_2.json", "w");

    const char *j1 = "{\n"
        "  \"timestamp\": \"T1\", \"type\": \"test\", \"git_commit\": \"test\",\n"
        "  \"warmup_duration_ms\": 100, \"benchmark_duration_ms\": 500,\n"
        "  \"warmup_count\": 10, \"sample_count\": 3,\n"
        "  \"solves_per_second\": 100.00,\n"
        "  \"solve_times_ms\": [10.0, 12.0, 11.0],\n"
        "  \"solution_lengths\": [21, 22, 21],\n"
        "  \"stats\": {\n"
        "    \"time\": { \"mean\": 11.0, \"median\": 11.0, \"std\": 1.0,\n"
        "      \"min\": 10.0, \"max\": 12.0, \"p90\": 12.0, \"p95\": 12.0,\n"
        "      \"p99\": 12.0, \"n\": 3 },\n"
        "    \"length\": { \"mean\": 21.3, \"median\": 21.0, \"std\": 0.6,\n"
        "      \"min\": 21, \"max\": 22, \"p90\": 22, \"p95\": 22,\n"
        "      \"p99\": 22, \"n\": 3 }\n"
        "  }\n"
        "}\n";

    const char *j2 = "{\n"
        "  \"timestamp\": \"T2\", \"type\": \"test\", \"git_commit\": \"test\",\n"
        "  \"warmup_duration_ms\": 100, \"benchmark_duration_ms\": 500,\n"
        "  \"warmup_count\": 10, \"sample_count\": 3,\n"
        "  \"solves_per_second\": 110.00,\n"
        "  \"solve_times_ms\": [9.0, 11.0, 10.0],\n"
        "  \"solution_lengths\": [21, 22, 21],\n"
        "  \"stats\": {\n"
        "    \"time\": { \"mean\": 10.0, \"median\": 10.0, \"std\": 1.0,\n"
        "      \"min\": 9.0, \"max\": 11.0, \"p90\": 11.0, \"p95\": 11.0,\n"
        "      \"p99\": 11.0, \"n\": 3 },\n"
        "    \"length\": { \"mean\": 21.3, \"median\": 21.0, \"std\": 0.6,\n"
        "      \"min\": 21, \"max\": 22, \"p90\": 22, \"p95\": 22,\n"
        "      \"p99\": 22, \"n\": 3 }\n"
        "  }\n"
        "}\n";

    fputs(j1, f1);
    fclose(f1);
    fputs(j2, f2);
    fclose(f2);

    compare_benchmark_files("/tmp/test_benchmark_1.json", "/tmp/test_benchmark_2.json");

    remove("/tmp/test_benchmark_1.json");
    remove("/tmp/test_benchmark_2.json");
}

static void test_config_compare_against() {
    config_t *cfg = get_config();

    TEST_ASSERT_NULL(cfg->compare_against);
    cfg->compare_against = strdup("/tmp/test_baseline.json");
    TEST_ASSERT_NOT_NULL(cfg->compare_against);
    TEST_ASSERT_EQUAL_STRING("/tmp/test_baseline.json", cfg->compare_against);
    free(cfg->compare_against);
    cfg->compare_against = NULL;
}

static void test_config_compare_benchmarks() {
    config_t *cfg = get_config();

    TEST_ASSERT_NULL(cfg->compare_benchmarks);
    cfg->compare_benchmarks = strdup("a.json,b.json");
    TEST_ASSERT_NOT_NULL(cfg->compare_benchmarks);
    TEST_ASSERT_EQUAL_STRING("a.json,b.json", cfg->compare_benchmarks);
    free(cfg->compare_benchmarks);
    cfg->compare_benchmarks = NULL;
}

void setUp() {
    init_config();
}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_benchmark_config_default_values);
    RUN_TEST(test_compare_benchmark_files_missing);
    RUN_TEST(test_compare_benchmark_files_valid);
    RUN_TEST(test_config_compare_against);
    RUN_TEST(test_config_compare_benchmarks);
    return UNITY_END();
}
