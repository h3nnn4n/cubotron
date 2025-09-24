#include <pcg_variants.h>
#include <stdlib.h>
#include <unity.h>

#include <definitions.h>
#include <move_tables.h>
#include <utils.h>

static pcg32_random_t rng;

void test_copy_coord_cube() {
    coord_cube_t *cube  = get_coord_cube();
    coord_cube_t *other = get_coord_cube();

    for (int i = 0; i < 1000; i++) {
        move_t move = pcg32_boundedrand(N_MOVES);

        coord_apply_move(cube, move);
        /*TEST_ASSERT_FALSE(are_coord_equal(cube, other));*/

        copy_coord_cube(other, cube);
        TEST_ASSERT_TRUE(are_all_coord_equal(cube, other));
    }

    free(cube);
}

void test_is_phase1_solved() {
    coord_cube_t *cube = get_coord_cube();

    TEST_ASSERT_TRUE(is_phase1_solved(cube));

    coord_apply_move(cube, MOVE_F1);
    coord_apply_move(cube, MOVE_R1);
    coord_apply_move(cube, MOVE_B1);
    coord_apply_move(cube, MOVE_R1);

    TEST_ASSERT_FALSE(is_phase1_solved(cube));

    coord_apply_move(cube, MOVE_R3);
    coord_apply_move(cube, MOVE_B3);
    coord_apply_move(cube, MOVE_R3);
    coord_apply_move(cube, MOVE_F3);

    TEST_ASSERT_TRUE(is_phase1_solved(cube));

    free(cube);
}

void test_is_phase2_solved() {
    coord_cube_t *cube = get_coord_cube();

    TEST_ASSERT_TRUE(is_phase2_solved(cube));

    coord_apply_move(cube, MOVE_F1);
    coord_apply_move(cube, MOVE_R1);
    coord_apply_move(cube, MOVE_B1);
    coord_apply_move(cube, MOVE_R1);

    TEST_ASSERT_FALSE(is_phase2_solved(cube));

    coord_apply_move(cube, MOVE_R3);
    coord_apply_move(cube, MOVE_B3);
    coord_apply_move(cube, MOVE_R3);
    coord_apply_move(cube, MOVE_F3);

    TEST_ASSERT_TRUE(is_phase1_solved(cube));

    free(cube);
}

void test_is_move_sequence_a_solution_for_cube() {
    coord_cube_t *cube = get_coord_cube();

    move_t solution[] = {MOVE_NULL};
    TEST_ASSERT_TRUE(is_move_sequence_a_solution_for_cube(cube, solution));

    coord_apply_move(cube, MOVE_F1);
    coord_apply_move(cube, MOVE_R1);
    coord_apply_move(cube, MOVE_B1);
    coord_apply_move(cube, MOVE_R1);

    move_t reverse_solution[] = {MOVE_R3, MOVE_B3, MOVE_R3, MOVE_F3, MOVE_NULL};
    TEST_ASSERT_TRUE(is_move_sequence_a_solution_for_cube(cube, reverse_solution));

    move_t wrong_solution[] = {MOVE_U1, MOVE_NULL};
    TEST_ASSERT_FALSE(is_move_sequence_a_solution_for_cube(cube, wrong_solution));

    free(cube);
}

void setUp(void) { build_move_tables(); }

void tearDown(void) {}

int main() {
    pcg32_srandom_r(&rng, 42u, 54u);

    UNITY_BEGIN();

    RUN_TEST(test_copy_coord_cube);
    RUN_TEST(test_is_phase1_solved);
    RUN_TEST(test_is_phase2_solved);
    RUN_TEST(test_is_move_sequence_a_solution_for_cube);

    return UNITY_END();
}
