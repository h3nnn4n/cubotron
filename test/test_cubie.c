#include <stdlib.h>
#include <unity.h>

#include <definitions.h>
#include <move_tables.h>
#include <utils.h>

void test_init_cubie_corner_permutations() {
    cube_cubie *cube = init_cubie_cube();

    for (int i = 0; i < 8; i++) {
        TEST_ASSERT_EQUAL_INT(i, cube->corner_permutations[i]);
    }

    free(cube);
}

void test_init_cubie_corner_orientations() {
    cube_cubie *cube = init_cubie_cube();

    for (int i = 0; i < 8; i++) {
        TEST_ASSERT_EQUAL_INT(0, cube->corner_orientations[i]);
    }

    free(cube);
}

void test_init_cubie_edge_permutations() {
    cube_cubie *cube = init_cubie_cube();

    for (int i = 0; i < 12; i++) {
        TEST_ASSERT_EQUAL_INT(i, cube->edge_permutations[i]);
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

void test_multiply_cube_cubie_edges_u2_is_not_identity() {
    cube_cubie *cube         = init_cubie_cube();
    cube_cubie *cube_u1_move = build_basic_move(MOVE_U1);

    multiply_cube_cubie_edges(cube, cube_u1_move);
    multiply_cube_cubie_edges(cube, cube_u1_move);

    int off_count = 0;
    for (int i = 0; i < 12; i++) {
        if (i != (int)cube->edge_permutations[i])
            off_count++;
    }

    TEST_ASSERT_EQUAL_INT(4, off_count);

    free(cube);
}

void test_multiply_cube_cubie_edges_u4_is_identity() {
    cube_cubie *cube         = init_cubie_cube();
    cube_cubie *cube_u1_move = build_basic_move(MOVE_U1);

    for (int i = 0; i < 4; i++) {
        multiply_cube_cubie_edges(cube, cube_u1_move);
    }

    for (int i = 0; i < 12; i++) {
        TEST_ASSERT_EQUAL_INT(i, cube->edge_permutations[i]);
        TEST_ASSERT_EQUAL_INT(0, cube->edge_orientations[i]);
    }

    free(cube);
}

void test_multiply_cube_cubie_corners_u2_is_not_identity() {
    cube_cubie *cube         = init_cubie_cube();
    cube_cubie *cube_u1_move = build_basic_move(MOVE_U1);

    multiply_cube_cubie_corners(cube, cube_u1_move);
    multiply_cube_cubie_corners(cube, cube_u1_move);

    int off_count = 0;
    for (int i = 0; i < 8; i++) {
        if (i != (int)cube->corner_permutations[i])
            off_count++;
    }

    TEST_ASSERT_EQUAL_INT(4, off_count);

    free(cube);
}

void test_multiply_cube_cubie_corners_u4_is_identity() {
    cube_cubie *cube         = init_cubie_cube();
    cube_cubie *cube_u1_move = build_basic_move(MOVE_U1);

    for (int i = 0; i < 4; i++) {
        multiply_cube_cubie_corners(cube, cube_u1_move);
    }

    for (int i = 0; i < 8; i++) {
        TEST_ASSERT_EQUAL_INT(i, cube->corner_permutations[i]);
        TEST_ASSERT_EQUAL_INT(0, cube->corner_orientations[i]);
    }

    free(cube);
}

void test_set_corner_orientations_only_makes_valid_cubes() {
    cube_cubie *cube = init_cubie_cube();

    for (int i = 0; i < 2187; i++) {
        set_corner_orientations(cube, i);

        TEST_ASSERT_TRUE(is_valid(cube));
    }

    free(cube);
}

void test_set_corner_and_get_corner_orientations() {
    cube_cubie *cube = init_cubie_cube();

    for (int i = 0; i < 2187; i++) {
        set_corner_orientations(cube, i);
        int orientation = get_corner_orientations(cube);

        TEST_ASSERT_EQUAL_INT(i, orientation);
    }

    free(cube);
}

void setUp(void) {}

void tearDown(void) {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_init_cubie_corner_permutations);
    RUN_TEST(test_init_cubie_edge_permutations);
    RUN_TEST(test_init_cubie_corner_orientations);
    RUN_TEST(test_init_cubie_edge_orientations);

    RUN_TEST(test_multiply_cube_cubie_edges_u2_is_not_identity);
    RUN_TEST(test_multiply_cube_cubie_edges_u4_is_identity);

    RUN_TEST(test_multiply_cube_cubie_corners_u2_is_not_identity);
    RUN_TEST(test_multiply_cube_cubie_corners_u4_is_identity);

    RUN_TEST(test_set_corner_orientations_only_makes_valid_cubes);
    RUN_TEST(test_set_corner_and_get_corner_orientations);

    return UNITY_END();
}
