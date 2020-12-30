#include <unity.h>

#include <definitions.h>
#include <move_tables.h>

#include "test_move_tables.h"

void test_solved(cube_cubie *cube) {
    for (int i = 0; i < 8; i++) {
        TEST_ASSERT_EQUAL_INT(i, cube->corner_permutations[i]);
        TEST_ASSERT_EQUAL_INT(0, cube->corner_orientations[i]);
    }

    for (int i = 0; i < 12; i++) {
        TEST_ASSERT_EQUAL_INT(i, cube->edge_permutations[i]);
        TEST_ASSERT_EQUAL_INT(0, cube->edge_orientations[i]);
    }
}

void test_build_basic_move_u_corner() {
    cube_cubie *cube_1 = build_basic_move(MOVE_U1);
    cube_cubie *cube_2 = build_basic_move(MOVE_U2);
    cube_cubie *cube_3 = build_basic_move(MOVE_U3);

    for (int i = 0; i < 8; i++) {
        TEST_ASSERT_EQUAL_INT(cube_1->corner_permutations[i], cube_2->corner_permutations[i]);
        TEST_ASSERT_EQUAL_INT(cube_1->corner_permutations[i], cube_3->corner_permutations[i]);

        TEST_ASSERT_EQUAL_INT(cube_1->corner_orientations[i], cube_2->corner_orientations[i]);
        TEST_ASSERT_EQUAL_INT(cube_1->corner_orientations[i], cube_3->corner_orientations[i]);
    }
}

void test_build_basic_move_u_edges() {
    cube_cubie *cube_1 = build_basic_move(MOVE_U1);
    cube_cubie *cube_2 = build_basic_move(MOVE_U2);
    cube_cubie *cube_3 = build_basic_move(MOVE_U3);

    for (int i = 0; i < 12; i++) {
        TEST_ASSERT_EQUAL_INT(cube_1->edge_permutations[i], cube_2->edge_permutations[i]);
        TEST_ASSERT_EQUAL_INT(cube_1->edge_permutations[i], cube_3->edge_permutations[i]);

        TEST_ASSERT_EQUAL_INT(cube_1->edge_orientations[i], cube_2->edge_orientations[i]);
        TEST_ASSERT_EQUAL_INT(cube_1->edge_orientations[i], cube_3->edge_orientations[i]);
    }
}

void test_apply_move_null_does_nothing() {
    cube_cubie *cube = init_cubie_cube();

    apply_move(cube, MOVE_NULL);

    test_solved(cube);

    free(cube);
}

void test_apply_move_U1_times_four_is_identity() {
    cube_cubie *cube = init_cubie_cube();

    test_solved(cube);

    apply_move(cube, MOVE_U1);
    apply_move(cube, MOVE_U1);
    apply_move(cube, MOVE_U1);
    apply_move(cube, MOVE_U1);

    test_solved(cube);

    free(cube);
}

void test_apply_move_U3_times_four_is_identity() {
    cube_cubie *cube = init_cubie_cube();

    apply_move(cube, MOVE_U3);
    apply_move(cube, MOVE_U3);
    apply_move(cube, MOVE_U3);
    apply_move(cube, MOVE_U3);

    test_solved(cube);

    free(cube);
}

void test_apply_move_U2_times_two_is_identity() {
    cube_cubie *cube = init_cubie_cube();

    test_solved(cube);

    apply_move(cube, MOVE_U2);
    apply_move(cube, MOVE_U2);

    test_solved(cube);

    free(cube);
}

void test_apply_move_U1_is_inverse_of_U3() {
    cube_cubie *cube = init_cubie_cube();

    test_solved(cube);

    apply_move(cube, MOVE_U1);
    apply_move(cube, MOVE_U3);

    test_solved(cube);

    apply_move(cube, MOVE_U3);
    apply_move(cube, MOVE_U1);

    test_solved(cube);

    free(cube);
}

void setUp(void) { build_move_table(); }

void tearDown(void) {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_build_basic_move_u_edges);
    RUN_TEST(test_build_basic_move_u_edges);

    RUN_TEST(test_apply_move_null_does_nothing);
    RUN_TEST(test_apply_move_U1_times_four_is_identity);
    RUN_TEST(test_apply_move_U3_times_four_is_identity);
    RUN_TEST(test_apply_move_U2_times_two_is_identity);
    RUN_TEST(test_apply_move_U1_is_inverse_of_U3);

    return UNITY_END();
}
