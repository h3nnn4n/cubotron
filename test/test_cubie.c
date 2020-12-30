#include <stdlib.h>
#include <unity.h>

#include <definitions.h>
#include <move_tables.h>

void test_init_cubie_corner_orientations() {
    cube_cubie *cube = init_cubie_cube();

    for (int i = 0; i < 8; i++) {
        TEST_ASSERT_EQUAL_INT(0, cube->corner_orientations[i]);
    }

    free(cube);
}

void test_init_cubie_edge_orientations() {
    cube_cubie *cube = init_cubie_cube();

    for (int i = 0; i < 12; i++) {
        TEST_ASSERT_EQUAL_INT(0, cube->edge_orientations[i]);
    }

    free(cube);
}

void cubie_runner() {
    RUN_TEST(test_init_cubie_corner_orientations);
    RUN_TEST(test_init_cubie_edge_orientations);
}
