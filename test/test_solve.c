#include <pcg_variants.h>
#include <unity.h>

#include <facelets.h>
#include <move_tables.h>
#include <pruning.h>
#include <sample_facelets.h>
#include <solve.h>
#include <utils.h>

void test_random_phase1_solving() {
    coord_cube_t *cube = get_coord_cube();

    for (int i = 0; i < 100; i++) {
        int    n_moves   = 50;
        move_t moves[50] = {0};

        reset_coord_cube(cube);

        for (int i = 0; i < n_moves; i++) {
            moves[i] = pcg32_boundedrand(N_MOVES);

            do {
                moves[i] = pcg32_boundedrand(N_MOVES);
            } while (i > 0 && moves[i] == moves[i - 1]);

            coord_apply_move(cube, moves[i]);
        }

        /*char buffer[512];*/
        /*sprintf(buffer, "%4d %4d %3d", cube->edge_orientations, cube->corner_orientations, cube->E_slice);*/
        /*TEST_MESSAGE(buffer);*/

        TEST_ASSERT_FALSE(is_phase1_solved(cube));

        move_t *solution = solve_phase1(cube, 25, 0, 0);

        TEST_ASSERT_TRUE(solution != NULL);

        for (int i = 0; solution[i] != MOVE_NULL; i++) {
            coord_apply_move(cube, solution[i]);
        }

        TEST_ASSERT_TRUE(is_phase1_solved(cube));

        free(solution);
    }
    free(cube);
}

void test_random_phase2_solving() {
    coord_cube_t *cube = get_coord_cube();

    for (int i = 0; i < 100; i++) {
        reset_coord_cube(cube);

        int    n_moves   = 50;
        move_t moves[50] = {0};
        for (int i = 0; i < n_moves; i++) {
            moves[i] = pcg32_boundedrand(N_MOVES);

            do {
                moves[i] = pcg32_boundedrand(N_MOVES);
            } while (i > 0 && moves[i] == moves[i - 1]);

            coord_apply_move(cube, moves[i]);
        }

        // Apply phase1 solution
        {
            move_t *solution = solve_phase1(cube, 25, 0, 0);

            for (int i = 0; solution[i] != MOVE_NULL; i++) {
                coord_apply_move(cube, solution[i]);
            }

            TEST_ASSERT_TRUE(is_phase1_solved(cube));

            free(solution);
        }

        TEST_ASSERT_FALSE(is_phase2_solved(cube));

        move_t *solution = solve_phase2(cube, 20, 0);

        TEST_ASSERT_TRUE(solution != NULL);

        for (int i = 0; solution[i] != MOVE_NULL; i++) {
            coord_apply_move(cube, solution[i]);
        }

        TEST_ASSERT_TRUE(is_phase2_solved(cube));

        free(solution);
    }

    free(cube);
}

void test_facelets_solve_with_max_length() {
    char *facelets = "DUDUUUDBUFRFRRBRDUBLLUFDUBFBDDFDLUFFRBLFLFBRRLLBRBDRLL";

    // Basic test, just gotta go fast
    // Solution with length 19 takes too long
    for (int max_length = 20; max_length < 22; max_length++) {
        move_t *solution = solve_facelets(facelets, max_length, 0, 1);

        TEST_ASSERT_TRUE(solution != NULL);

        int length = 0;
        for (int i = 0; solution[i] != MOVE_NULL; i++, length++) {
        }

        TEST_ASSERT_TRUE(length <= max_length);

        free(solution);
    }
}

void test_random_full_solver_with_random_scrambles() {
    coord_cube_t *cube = get_coord_cube();

    for (int i = 0; i < 100; i++) {
        reset_coord_cube(cube);

        int    n_moves   = 50;
        move_t moves[50] = {0};
        for (int i = 0; i < n_moves; i++) {
            moves[i] = pcg32_boundedrand(N_MOVES);

            do {
                moves[i] = pcg32_boundedrand(N_MOVES);
            } while (i > 0 && moves[i] == moves[i - 1]);

            coord_apply_move(cube, moves[i]);
        }

        TEST_ASSERT_FALSE(is_phase1_solved(cube));
        TEST_ASSERT_FALSE(is_phase2_solved(cube));

        /*char buffer[512];*/

        /*sprintf(buffer, "%4d %4d %3d %4d %4d", cube->edge_orientations, cube->corner_orientations, cube->E_slice,*/
        /*cube->E_sorted_slice, cube->corner_permutations);*/

        /*TEST_MESSAGE(buffer);*/

        move_t *solution = solve_single(cube);

        for (int i = 0; solution[i] != MOVE_NULL; i++) {
            coord_apply_move(cube, solution[i]);
        }

        TEST_ASSERT_TRUE(is_phase1_solved(cube));
        TEST_ASSERT_TRUE(is_phase2_solved(cube));

        free(solution);
    }

    free(cube);
}

void test_random_full_solver_with_sample_cubes() {
    for (int i = 0; i < N_FACELETS_SAMPLES; i++) {
        cube_cubie_t *cubie_cube = build_cubie_cube_from_str(sample_facelets[i]);
        coord_cube_t *cube       = make_coord_cube(cubie_cube);

        TEST_ASSERT_FALSE(is_phase1_solved(cube));
        TEST_ASSERT_FALSE(is_phase2_solved(cube));

        /*char buffer[512];*/

        /*sprintf(buffer, "%4d %4d %3d %4d %4d", cube->edge_orientations, cube->corner_orientations, cube->E_slice,*/
        /*cube->E_sorted_slice, cube->corner_permutations);*/

        /*TEST_MESSAGE(buffer);*/

        move_t *solution = solve_single(cube);

        for (int i = 0; solution[i] != MOVE_NULL; i++) {
            coord_apply_move(cube, solution[i]);
        }

        TEST_ASSERT_TRUE(is_phase1_solved(cube));
        TEST_ASSERT_TRUE(is_phase2_solved(cube));

        free(solution);
        free(cube);
    }
}

void setUp() {}
void tearDown() {}

int main() {
    build_move_tables();
    build_pruning_tables();

    pcg32_srandom(43u, 55u);

    UNITY_BEGIN();

    RUN_TEST(test_random_phase1_solving);
    RUN_TEST(test_random_phase2_solving);
    RUN_TEST(test_facelets_solve_with_max_length);
    RUN_TEST(test_random_full_solver_with_random_scrambles);
    RUN_TEST(test_random_full_solver_with_sample_cubes);

    return UNITY_END();
}
