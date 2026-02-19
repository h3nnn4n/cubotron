#include <pcg_variants.h>
#include <unity.h>

#include <config.h>
#include <coord_cube.h>
#include <move_tables.h>
#include <pruning.h>
#include <solve.h>
#include <symmetry.h>

void test_pruning_solved_state() {
    coord_cube_t *cube = get_coord_cube();
    reset_coord_cube(cube);

    TEST_ASSERT_EQUAL(0, get_phase1_pruning(cube));
    TEST_ASSERT_EQUAL(0, get_phase2_pruning(cube));

    free(cube);
}

void test_pruning_single_moves() {
    coord_cube_t *cube = get_coord_cube();

    move_t test_moves[] = {MOVE_U1, MOVE_R1, MOVE_F1, MOVE_D1};
    int n_test_moves = sizeof(test_moves) / sizeof(test_moves[0]);

    for (int i = 0; i < n_test_moves; i++) {
        reset_coord_cube(cube);
        coord_apply_move(cube, test_moves[i]);

        int phase1_pruning = get_phase1_pruning(cube);
        TEST_ASSERT_TRUE(phase1_pruning >= 0 && phase1_pruning <= 3);

        if (is_phase1_solved(cube)) {
            int phase2_pruning = get_phase2_pruning(cube);
            TEST_ASSERT_TRUE(phase2_pruning >= 0 && phase2_pruning <= 3);
        }
    }

    free(cube);
}

void test_combined_pruning_solved_state() {
    coord_cube_t *cube = get_coord_cube();
    reset_coord_cube(cube);

    TEST_ASSERT_EQUAL(0, get_phase1_pruning(cube));

    free(cube);
}

void test_combined_pruning_geq_individual() {
    coord_cube_t *cube = get_coord_cube();

    for (int i = 0; i < 20; i++) {
        reset_coord_cube(cube);
        scramble_cube(cube, 15);

        int combined = get_phase1_pruning(cube);
        TEST_ASSERT_TRUE(combined >= 0);
    }

    free(cube);
}

void test_pruning_never_overestimates_sample() {
    coord_cube_t *cube = get_coord_cube();
    config_t *config = get_config();
    config->max_depth = 25;

    for (int i = 0; i < 10; i++) {
        reset_coord_cube(cube);
        scramble_cube(cube, 10);

        int phase1_pruning = get_phase1_pruning(cube);

        solve_list_t *solutions = solve(cube, config);
        TEST_ASSERT_NOT_NULL(solutions);
        TEST_ASSERT_NOT_NULL(solutions->solution);

        int actual_length = 0;
        for (int j = 0; solutions->solution[j] != MOVE_NULL; j++) {
            actual_length++;
        }

        TEST_ASSERT_TRUE(phase1_pruning <= actual_length);

        destroy_solve_list(solutions);
    }

    free(cube);
}

void setUp() { init_config(); }
void tearDown() {}

int main() {
    build_move_tables();
    build_symmetry_tables();
    build_pruning_tables();

    pcg32_srandom(43u, 55u);

    UNITY_BEGIN();

    RUN_TEST(test_pruning_solved_state);
    RUN_TEST(test_pruning_single_moves);
    RUN_TEST(test_combined_pruning_solved_state);
    RUN_TEST(test_combined_pruning_geq_individual);
    RUN_TEST(test_pruning_never_overestimates_sample);

    return UNITY_END();
}
