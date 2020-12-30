#include "test_move_tables.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_build_basic_move_u_edges);
    RUN_TEST(test_build_basic_move_u_corner);
    return UNITY_END();
}
