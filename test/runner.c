#include <unity.h>

#include "test_cubie.h"
#include "test_move_tables.h"

void setUp(void) {}

void tearDown(void) {}

int main() {
    UNITY_BEGIN();
    move_tables_runner();
    cubie_runner();
    return UNITY_END();
}