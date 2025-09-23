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

        solve_context_t *solve_context = make_solve_context(cube);
        config_t *       config        = get_config();
        config->n_solutions            = 0;
        phase1_solve_t *phase1_solution = get_phase1_solution(solve_context, config);

        move_t *solution = phase1_solution->phase1_solution;
        TEST_ASSERT_TRUE(solution != NULL);

        for (int i = 0; solution[i] != MOVE_NULL; i++) {
            coord_apply_move(cube, solution[i]);
        }

        TEST_ASSERT_TRUE(is_phase1_solved(cube));

        free(solution);
    }
    free(cube);
}

// void test_random_phase2_solving() {
//     coord_cube_t *cube = get_coord_cube();

//     for (int i = 0; i < 100; i++) {
//         reset_coord_cube(cube);

//         int    n_moves   = 50;
//         move_t moves[50] = {0};
//         for (int i = 0; i < n_moves; i++) {
//             moves[i] = pcg32_boundedrand(N_MOVES);

//             do {
//                 moves[i] = pcg32_boundedrand(N_MOVES);
//             } while (i > 0 && moves[i] == moves[i - 1]);

//             coord_apply_move(cube, moves[i]);
//         }

//         solve_context_t *solve_context = make_solve_context(cube);
//         // Apply phase1 solution
//         {
//             config_t *config    = get_config();
//             config->n_solutions = 0;
//             move_t *solution    = get_phase1_solution(solve_context, config, NULL);

//             for (int i = 0; solution[i] != MOVE_NULL; i++) {
//                 coord_apply_move(cube, solution[i]);
//             }

//             TEST_ASSERT_TRUE(is_phase1_solved(cube));

//             free(solution);
//         }

//         TEST_ASSERT_FALSE(is_phase2_solved(cube));

//         copy_coord_cube(solve_context->phase2_context->cube, cube);
//         config_t *config   = get_config();
//         move_t *  solution = solve_phase2(solve_context->phase2_context, config, 20);

//         TEST_ASSERT_TRUE(solution != NULL);

//         for (int i = 0; solution[i] != MOVE_NULL; i++) {
//             coord_apply_move(cube, solution[i]);
//         }

//         TEST_ASSERT_TRUE(is_phase2_solved(cube));

//         free(solution);
//     }

//     free(cube);
// }

void test_facelets_solve_with_max_length() {
    char *facelets = "DUDUUUDBUFRFRRBRDUBLLUFDUBFBDDFDLUFFRBLFLFBRRLLBRBDRLL";

    // Basic test, just gotta go fast
    // Solution with length 19 takes too long
    for (int max_length = 20; max_length < 22; max_length++) {
        config_t *config       = get_config();
        config->max_depth      = max_length;
        solve_list_t *solution = solve_facelets(facelets, config);

        TEST_ASSERT_TRUE(solution != NULL);

        int length = 0;
        for (int i = 0; solution->solution[i] != MOVE_NULL; i++, length++)
            ;

        TEST_ASSERT_TRUE(length <= max_length);

        free(solution);
    }
}

void test_random_full_solver_with_random_scrambles_single_solution() {
    coord_cube_t *cube = get_coord_cube();

    for (int i = 0; i < 100; i++) {
        reset_coord_cube(cube);

        int    n_moves   = 50;
        move_t moves[50] = {0};
        for (int j = 0; j < n_moves; j++) {
            moves[j] = pcg32_boundedrand(N_MOVES);

            do {
                moves[j] = pcg32_boundedrand(N_MOVES);
            } while (j > 0 && moves[j] == moves[j - 1]);

            coord_apply_move(cube, moves[j]);
        }

        TEST_ASSERT_FALSE(is_phase1_solved(cube));
        TEST_ASSERT_FALSE(is_phase2_solved(cube));

        /*char buffer[512];*/

        /*sprintf(buffer, "%4d %4d %3d %4d %4d", cube->edge_orientations, cube->corner_orientations, cube->E_slice,*/
        /*cube->E_sorted_slice, cube->corner_permutations);*/

        /*TEST_MESSAGE(buffer);*/

        solve_list_t *solution = solve_single(cube);

        for (int i = 0; solution->solution[i] != MOVE_NULL; i++) {
            coord_apply_move(cube, solution->solution[i]);
        }

        TEST_ASSERT_TRUE(is_phase1_solved(cube));
        TEST_ASSERT_TRUE(is_phase2_solved(cube));

        free(solution);
    }

    free(cube);
}

void test_random_full_solver_with_random_scrambles_multiple_solution() {
    coord_cube_t *cube = get_coord_cube();

    for (int i = 0; i < 10; i++) {
        reset_coord_cube(cube);

        int    n_moves   = 50;
        move_t moves[50] = {0};
        for (int j = 0; j < n_moves; j++) {
            moves[j] = pcg32_boundedrand(N_MOVES);

            do {
                moves[j] = pcg32_boundedrand(N_MOVES);
            } while (j > 0 && moves[j] == moves[j - 1]);

            coord_apply_move(cube, moves[j]);
        }

        TEST_ASSERT_FALSE(is_phase1_solved(cube));
        TEST_ASSERT_FALSE(is_phase2_solved(cube));

        /*char buffer[512];*/

        /*sprintf(buffer, "%4d %4d %3d %4d %4d", cube->edge_orientations, cube->corner_orientations, cube->E_slice,*/
        /*cube->E_sorted_slice, cube->corner_permutations);*/

        /*TEST_MESSAGE(buffer);*/

        solve_list_t *solution = solve_single(cube);

        do {
            coord_cube_t *cube_copy = get_coord_cube();
            copy_coord_cube(cube_copy, cube);

            for (int j = 0; solution->solution[j] != MOVE_NULL; j++) {
                coord_apply_move(cube_copy, solution->solution[j]);
            }

            TEST_ASSERT_TRUE(is_phase1_solved(cube_copy));
            TEST_ASSERT_TRUE(is_phase2_solved(cube_copy));

            solution = solution->next;

            free(cube_copy);
        } while (solution != NULL && solution->solution != NULL);

        free(solution);
    }

    free(cube);
}

void test_random_full_solver_with_sample_cubes_single_solution() {
    for (int i = 0; i < N_FACELETS_SAMPLES; i++) {
        cube_cubie_t *cubie_cube = build_cubie_cube_from_str(sample_facelets[i]);
        coord_cube_t *cube       = make_coord_cube(cubie_cube);

        TEST_ASSERT_FALSE(is_phase1_solved(cube));
        TEST_ASSERT_FALSE(is_phase2_solved(cube));

        /*char buffer[512];*/

        /*sprintf(buffer, "%4d %4d %3d %4d %4d", cube->edge_orientations, cube->corner_orientations, cube->E_slice,*/
        /*cube->E_sorted_slice, cube->corner_permutations);*/

        /*TEST_MESSAGE(buffer);*/

        solve_list_t *solution = solve_single(cube);

        for (int i = 0; solution->solution[i] != MOVE_NULL; i++) {
            coord_apply_move(cube, solution->solution[i]);
        }

        TEST_ASSERT_TRUE(is_phase1_solved(cube));
        TEST_ASSERT_TRUE(is_phase2_solved(cube));

        free(solution);
        free(cube);
    }
}

void test_solve_with_move_blacklist() {
    for (move_t base_move = 0; base_move < N_MOVES; base_move += 3) {
        init_config();

        char buffer[512];
        sprintf(buffer, "basemove: %2d %s  blacklist:", base_move, move_to_str(base_move));

        config_t *config  = get_config();
        config->max_depth = 30; // solves with blacklists take longer
        for (int move_offset = 0; move_offset < 3; move_offset++) {
            move_t move                   = base_move + move_offset;
            config->move_black_list[move] = move;

            sprintf(buffer, "%s %s", buffer, move_to_str(move));
        }

        TEST_MESSAGE(buffer);

        char blacklisted_char = move_to_str(base_move)[0];

        for (int i = 0; i < 10; i++) {
            coord_cube_t *cube = get_coord_cube();

            int    n_moves   = 50;
            move_t moves[50] = {0};
            for (int j = 0; j < n_moves; j++) {
                moves[j] = pcg32_boundedrand(N_MOVES);

                do {
                    moves[j] = pcg32_boundedrand(N_MOVES);
                } while (j > 0 && moves[j] == moves[j - 1]);

                coord_apply_move(cube, moves[j]);
            }

            TEST_ASSERT_FALSE(is_phase1_solved(cube));
            TEST_ASSERT_FALSE(is_phase2_solved(cube));

            solve_list_t *solution = solve(cube, config);

            for (int i = 0; solution->solution[i] != MOVE_NULL; i++) {
                coord_apply_move(cube, solution->solution[i]);

                if (move_to_str(solution->solution[i])[0] == blacklisted_char) {
                    sprintf(buffer, "solution has blacklisted move:");

                    for (int i = 0; solution->solution[i] != MOVE_NULL; i++) {
                        sprintf(buffer, "%s %s", buffer, move_to_str(solution->solution[i]));
                    }

                    TEST_MESSAGE(buffer);

                    TEST_FAIL();
                }
            }

            TEST_ASSERT_TRUE(is_phase1_solved(cube));
            TEST_ASSERT_TRUE(is_phase2_solved(cube));

            free(solution);
            free(cube);
        }
    }
}

void setUp() { init_config(); }
void tearDown() {}

int main() {
    build_move_tables();
    build_pruning_tables();

    pcg32_srandom(43u, 55u);

    UNITY_BEGIN();

    RUN_TEST(test_random_phase1_solving);
    // RUN_TEST(test_random_phase2_solving);
    RUN_TEST(test_facelets_solve_with_max_length);
    RUN_TEST(test_random_full_solver_with_random_scrambles_single_solution);
    RUN_TEST(test_random_full_solver_with_random_scrambles_multiple_solution);
    RUN_TEST(test_random_full_solver_with_sample_cubes_single_solution);
    RUN_TEST(test_solve_with_move_blacklist);

    return UNITY_END();
}
