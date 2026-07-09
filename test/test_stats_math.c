#include <unity.h>
#include <string.h>

#include "stats_math.h"

void test_sort_float() {
    float v[] = {3.0f, 1.0f, 4.0f, 1.5f, 9.0f};
    sort_float(v, 5);
    TEST_ASSERT_EQUAL_FLOAT(1.0f,  v[0]);
    TEST_ASSERT_EQUAL_FLOAT(1.5f, v[1]);
    TEST_ASSERT_EQUAL_FLOAT(3.0f, v[2]);
    TEST_ASSERT_EQUAL_FLOAT(4.0f, v[3]);
    TEST_ASSERT_EQUAL_FLOAT(9.0f, v[4]);
}

void test_sort_float_single() {
    float v[] = {42.0f};
    sort_float(v, 1);
    TEST_ASSERT_EQUAL_FLOAT(42.0f, v[0]);
}

void test_min_f() {
    float v[] = {3.0f, 1.0f, 4.0f, 1.5f, 9.0f};
    TEST_ASSERT_EQUAL_FLOAT(1.0f, min_f(v, 5));
}

void test_min_f_single() {
    float v[] = {42.0f};
    TEST_ASSERT_EQUAL_FLOAT(42.0f, min_f(v, 1));
}

void test_max_f() {
    float v[] = {3.0f, 1.0f, 4.0f, 1.5f, 9.0f};
    TEST_ASSERT_EQUAL_FLOAT(9.0f, max_f(v, 5));
}

void test_max_f_single() {
    float v[] = {42.0f};
    TEST_ASSERT_EQUAL_FLOAT(42.0f, max_f(v, 1));
}

void test_avg_f() {
    float v[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    TEST_ASSERT_EQUAL_FLOAT(3.0f, avg_f(v, 5));
}

void test_avg_f_single() {
    float v[] = {7.5f};
    TEST_ASSERT_EQUAL_FLOAT(7.5f, avg_f(v, 1));
}

void test_std_f() {
    float v[] = {2.0f, 4.0f, 4.0f, 4.0f, 5.0f, 5.0f, 7.0f, 9.0f};
    float result = std_f(v, 8);
    TEST_ASSERT_EQUAL_FLOAT(2.0f, result);
}

void test_std_f_constant() {
    float v[] = {5.0f, 5.0f, 5.0f};
    TEST_ASSERT_EQUAL_FLOAT(0.0f, std_f(v, 3));
}

void test_percentile_f() {
    float v[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    sort_float(v, 10);
    TEST_ASSERT_EQUAL_FLOAT(1.0f,  percentile_f(v, 10, 0.0f));
    TEST_ASSERT_EQUAL_FLOAT(10.0f, percentile_f(v, 10, 100.0f));
    TEST_ASSERT_EQUAL_FLOAT(5.5f,  percentile_f(v, 10, 50.0f));
    TEST_ASSERT_EQUAL_FLOAT(9.1f,  percentile_f(v, 10, 90.0f));
    TEST_ASSERT_EQUAL_FLOAT(9.55f, percentile_f(v, 10, 95.0f));
    TEST_ASSERT_EQUAL_FLOAT(9.91f, percentile_f(v, 10, 99.0f));
}

void test_percentile_f_two_values() {
    float v[] = {10.0f, 20.0f};
    sort_float(v, 2);
    TEST_ASSERT_EQUAL_FLOAT(10.0f, percentile_f(v, 2, 0.0f));
    TEST_ASSERT_EQUAL_FLOAT(20.0f, percentile_f(v, 2, 100.0f));
    TEST_ASSERT_EQUAL_FLOAT(15.0f, percentile_f(v, 2, 50.0f));
}

void test_percentile_f_single() {
    float v[] = {7.0f};
    TEST_ASSERT_EQUAL_FLOAT(7.0f, percentile_f(v, 1, 0.0f));
    TEST_ASSERT_EQUAL_FLOAT(7.0f, percentile_f(v, 1, 50.0f));
    TEST_ASSERT_EQUAL_FLOAT(7.0f, percentile_f(v, 1, 100.0f));
}

void test_compute_float_aggregate_basic() {
    float v[] = {0.0f, 10.0f, 20.0f, 30.0f, 40.0f, 50.0f};
    float min, max, avg, std, p90, p95, p99;
    compute_float_aggregate(v, 6, &min, &max, &avg, &std, &p90, &p95, &p99);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, min);
    TEST_ASSERT_EQUAL_FLOAT(50.0f, max);
    TEST_ASSERT_EQUAL_FLOAT(25.0f, avg);
    TEST_ASSERT_EQUAL_FLOAT(45.0f, p90);
    TEST_ASSERT_EQUAL_FLOAT(47.5f, p95);
    TEST_ASSERT_EQUAL_FLOAT(49.5f, p99);
}

void test_compute_int_aggregate_basic() {
    int v[] = {10, 20, 30, 40, 50};
    int min, max;
    float avg, std, p90, p95, p99;
    compute_int_aggregate(v, 5, &min, &max, &avg, &std, &p90, &p95, &p99);
    TEST_ASSERT_EQUAL(10, min);
    TEST_ASSERT_EQUAL(50, max);
    TEST_ASSERT_EQUAL_FLOAT(30.0f, avg);
}

void setUp() {}
void tearDown() {}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_sort_float);
    RUN_TEST(test_sort_float_single);
    RUN_TEST(test_min_f);
    RUN_TEST(test_min_f_single);
    RUN_TEST(test_max_f);
    RUN_TEST(test_max_f_single);
    RUN_TEST(test_avg_f);
    RUN_TEST(test_avg_f_single);
    RUN_TEST(test_std_f);
    RUN_TEST(test_std_f_constant);
    RUN_TEST(test_percentile_f);
    RUN_TEST(test_percentile_f_two_values);
    RUN_TEST(test_percentile_f_single);

    RUN_TEST(test_compute_float_aggregate_basic);
    RUN_TEST(test_compute_int_aggregate_basic);

    return UNITY_END();
}
