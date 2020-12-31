#include <unity.h>

#include <cubie_move_table.h>
#include <definitions.h>
#include <utils.h>

void test_cubie_build_basic_move_u_corner() {
    cube_cubie_t *cube_1 = cubie_build_basic_move(MOVE_U1);
    cube_cubie_t *cube_2 = cubie_build_basic_move(MOVE_U2);
    cube_cubie_t *cube_3 = cubie_build_basic_move(MOVE_U3);

    for (int i = 0; i < N_CORNERS; i++) {
        TEST_ASSERT_EQUAL_INT(cube_1->corner_permutations[i], cube_2->corner_permutations[i]);
        TEST_ASSERT_EQUAL_INT(cube_1->corner_permutations[i], cube_3->corner_permutations[i]);

        TEST_ASSERT_EQUAL_INT(cube_1->corner_orientations[i], cube_2->corner_orientations[i]);
        TEST_ASSERT_EQUAL_INT(cube_1->corner_orientations[i], cube_3->corner_orientations[i]);
    }
}

void test_cubie_build_basic_move_u_edges() {
    cube_cubie_t *cube_1 = cubie_build_basic_move(MOVE_U1);
    cube_cubie_t *cube_2 = cubie_build_basic_move(MOVE_U2);
    cube_cubie_t *cube_3 = cubie_build_basic_move(MOVE_U3);

    for (int i = 0; i < N_EDGES; i++) {
        TEST_ASSERT_EQUAL_INT(cube_1->edge_permutations[i], cube_2->edge_permutations[i]);
        TEST_ASSERT_EQUAL_INT(cube_1->edge_permutations[i], cube_3->edge_permutations[i]);

        TEST_ASSERT_EQUAL_INT(cube_1->edge_orientations[i], cube_2->edge_orientations[i]);
        TEST_ASSERT_EQUAL_INT(cube_1->edge_orientations[i], cube_3->edge_orientations[i]);
    }
}

void test_cubie_apply_move_null_does_nothing() {
    cube_cubie_t *cube = init_cubie_cube();

    cubie_apply_move(cube, MOVE_NULL);

    TEST_ASSERT_TRUE(is_solved(cube));

    free(cube);
}

void test_cubie_apply_move_U1_times_four_is_identity() {
    cube_cubie_t *cube = init_cubie_cube();

    cubie_apply_move(cube, MOVE_U1);
    cubie_apply_move(cube, MOVE_U1);
    cubie_apply_move(cube, MOVE_U1);
    cubie_apply_move(cube, MOVE_U1);

    TEST_ASSERT_TRUE(is_solved(cube));

    free(cube);
}

void test_cubie_apply_move_U3_times_four_is_identity() {
    cube_cubie_t *cube = init_cubie_cube();

    cubie_apply_move(cube, MOVE_U3);
    cubie_apply_move(cube, MOVE_U3);
    cubie_apply_move(cube, MOVE_U3);
    cubie_apply_move(cube, MOVE_U3);

    TEST_ASSERT_TRUE(is_solved(cube));

    free(cube);
}

void test_cubie_apply_move_U2_times_two_is_identity() {
    cube_cubie_t *cube = init_cubie_cube();

    cubie_apply_move(cube, MOVE_U2);
    cubie_apply_move(cube, MOVE_U2);

    TEST_ASSERT_TRUE(is_solved(cube));

    free(cube);
}

void test_cubie_apply_move_U1_is_inverse_of_U3() {
    cube_cubie_t *cube = init_cubie_cube();

    cubie_apply_move(cube, MOVE_U1);
    cubie_apply_move(cube, MOVE_U3);

    TEST_ASSERT_TRUE(is_solved(cube));

    cubie_apply_move(cube, MOVE_U3);
    cubie_apply_move(cube, MOVE_U1);

    TEST_ASSERT_TRUE(is_solved(cube));

    free(cube);
}

void setUp(void) { cubie_build_move_table(); }

void tearDown(void) {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_cubie_build_basic_move_u_edges);
    RUN_TEST(test_cubie_build_basic_move_u_edges);

    RUN_TEST(test_cubie_apply_move_null_does_nothing);
    RUN_TEST(test_cubie_apply_move_U1_times_four_is_identity);
    RUN_TEST(test_cubie_apply_move_U3_times_four_is_identity);
    RUN_TEST(test_cubie_apply_move_U2_times_two_is_identity);
    RUN_TEST(test_cubie_apply_move_U1_is_inverse_of_U3);

    return UNITY_END();
}
