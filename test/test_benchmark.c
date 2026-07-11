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

void setUp() {
    init_config();
}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_benchmark_config_default_values);
    RUN_TEST(test_compare_benchmark_files_missing);
    return UNITY_END();
}
