#include <unity.h>

void setUp(void) {}

void tearDown(void) {}

void test_potato() { TEST_ASSERT_EQUAL_INT(1, 2); }

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_potato);
    return UNITY_END();
}
