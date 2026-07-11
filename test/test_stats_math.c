#include <unity.h>

#include <stats_math.h>

void test_sort_float() {
    float v[] = {3.0f, 1.0f, 2.0f};
    sort_float(v, 3);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, v[0]);
    TEST_ASSERT_EQUAL_FLOAT(2.0f, v[1]);
    TEST_ASSERT_EQUAL_FLOAT(3.0f, v[2]);
}

void test_min_f() {
    float v[] = {3.0f, 1.0f, 2.0f};
    TEST_ASSERT_EQUAL_FLOAT(1.0f, min_f(v, 3));
}

void test_max_f() {
    float v[] = {3.0f, 1.0f, 2.0f};
    TEST_ASSERT_EQUAL_FLOAT(3.0f, max_f(v, 3));
}

void test_avg_f() {
    float v[] = {1.0f, 2.0f, 3.0f};
    TEST_ASSERT_EQUAL_FLOAT(2.0f, avg_f(v, 3));
}

void test_std_f() {
    float v[] = {1.0f, 1.0f, 1.0f};
    TEST_ASSERT_EQUAL_FLOAT(0.0f, std_f(v, 3));
}

void test_percentile_f() {
    float v[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    sort_float(v, 5);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, percentile_f(v, 5, 0.0f));
    TEST_ASSERT_EQUAL_FLOAT(5.0f, percentile_f(v, 5, 100.0f));
    TEST_ASSERT_EQUAL_FLOAT(3.0f, percentile_f(v, 5, 50.0f));
}

void test_compute_float_aggregate() {
    float v[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    float min, max, avg, std, p90, p95, p99;
    compute_float_aggregate(v, 5, &min, &max, &avg, &std, &p90, &p95, &p99);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, min);
    TEST_ASSERT_EQUAL_FLOAT(5.0f, max);
    TEST_ASSERT_EQUAL_FLOAT(3.0f, avg);
}

void test_compute_int_aggregate() {
    int v[] = {1, 2, 3, 4, 5};
    int min, max;
    float avg, std, p90, p95, p99;
    compute_int_aggregate(v, 5, &min, &max, &avg, &std, &p90, &p95, &p99);
    TEST_ASSERT_EQUAL(1, min);
    TEST_ASSERT_EQUAL(5, max);
    TEST_ASSERT_EQUAL_FLOAT(3.0f, avg);
}

void setUp() {}
void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_sort_float);
    RUN_TEST(test_min_f);
    RUN_TEST(test_max_f);
    RUN_TEST(test_avg_f);
    RUN_TEST(test_std_f);
    RUN_TEST(test_percentile_f);
    RUN_TEST(test_compute_float_aggregate);
    RUN_TEST(test_compute_int_aggregate);
    return UNITY_END();
}
