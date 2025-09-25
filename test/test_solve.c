#include <pcg_variants.h>
#include <string.h>
#include <unity.h>

#include <coord_cube.h>
#include <facelets.h>
#include <move_tables.h>
#include <pruning.h>
#include <sample_facelets.h>
#include <solve.h>
#include <utils.h>

void test_random_phase1_solving() {
    coord_cube_t *cube = get_coord_cube();

    for (int i = 0; i < 100; i++) {
        reset_coord_cube(cube);
        scramble_cube(cube, 50);

        TEST_ASSERT_FALSE(is_phase1_solved(cube));

        solve_context_t *solve_context   = make_solve_context(cube);
        phase1_solve_t  *phase1_solution = get_phase1_solution(solve_context, get_config());

        move_t *solution = phase1_solution->solution;
        TEST_ASSERT_TRUE(solution != NULL);

        for (int i = 0; solution[i] != MOVE_NULL; i++) {
            coord_apply_move(cube, solution[i]);
        }

        TEST_ASSERT_TRUE(is_phase1_solved(cube));

        free(phase1_solution);
    }

    free(cube);
}

void test_phase1_solution_generator() {
    coord_cube_t *cube = get_coord_cube();
    scramble_cube(cube, 50);

    solve_context_t *solve_context = make_solve_context(cube);

    const uint8_t         N_SOLUTIONS = 50;
    const phase1_solve_t *solutions[N_SOLUTIONS];

    for (int i = 0; i < N_SOLUTIONS; i++) {
        phase1_solve_t *phase1_solution = get_phase1_solution(solve_context, get_config());

        TEST_ASSERT_TRUE(phase1_solution != NULL);

        solutions[i] = phase1_solution;
    }

    for (int i = 0; i < N_SOLUTIONS; i++) {
        for (int j = 0; j < N_SOLUTIONS; j++) {
            if (i == j)
                continue;

            // printf("\n");

            // printf("Checking if solutions %d and %d are equal\n", i, j);
            // printf("Solution %d: ", i);
            // print_move_sequence(solutions[i]->solution);
            // printf("Solution %d: ", j);
            // print_move_sequence(solutions[j]->solution);
            // printf("\n");

            TEST_ASSERT_FALSE(are_move_sequences_equal(solutions[i]->solution, solutions[j]->solution));
        }
    }

    for (int i = 0; i < N_SOLUTIONS; i++) {
        destroy_phase1_solve(solutions[i]);
    }

    free(solve_context);
}

void test_phase1_solution_count() {
    config_t *config             = get_config();
    uint32_t  original_max_depth = config->max_depth;
    config->max_depth            = 10;

    cube_cubie_t    *cubie_cube    = build_cubie_cube_from_str(sample_facelets[0]);
    coord_cube_t    *cube          = make_coord_cube(cubie_cube);
    solve_context_t *solve_context = make_solve_context(cube);

    uint64_t solution_count = 0;

    while (1) {
        phase1_solve_t *phase1_solution = get_phase1_solution(solve_context, config);
        if (phase1_solution == NULL)
            break;

        solution_count++;

        // print_move_sequence(phase1_solution->solution);
        free(phase1_solution);
    }

    config->max_depth = original_max_depth;
    TEST_ASSERT_TRUE(solution_count > 0);

    free(solve_context);
    free(cube);

    // printf("Solution count: %lu\n", solution_count);

    TEST_ASSERT_TRUE(solution_count == 1235401);
}

void test_random_phase2_solving() {
    coord_cube_t *cube = get_coord_cube();

    for (int i = 0; i < 50; i++) {
        reset_coord_cube(cube);
        scramble_cube(cube, 50);

        solve_context_t *phase1_solve_context = make_solve_context(cube);

        TEST_ASSERT_FALSE(is_phase1_solved(cube));
        TEST_ASSERT_FALSE(is_phase2_solved(cube));

        phase1_solve_t *phase1_solution = get_phase1_solution(phase1_solve_context, get_config());

        coord_apply_moves(cube, phase1_solution->solution, phase1_solution->depth);

        TEST_ASSERT_TRUE(is_phase1_solved(cube));
        TEST_ASSERT_FALSE(is_phase2_solved(cube));

        solve_context_t *phase2_solve_context = make_solve_context(phase1_solution->cube);
        phase2_solve_t  *phase2_solution      = solve_phase2(phase2_solve_context, get_config());

        TEST_ASSERT_TRUE(phase2_solution != NULL);

        coord_apply_moves(cube, phase2_solution->solution, phase2_solution->depth);

        TEST_ASSERT_TRUE(is_phase1_solved(cube));
        TEST_ASSERT_TRUE(is_phase2_solved(cube));

        free(phase1_solution);
        free(phase2_solution);

        destroy_solve_context(phase1_solve_context);
        destroy_solve_context(phase2_solve_context);
    }

    free(cube);
}

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

        printf("Length: %d\n", length);
        printf("Max length: %d\n", max_length);

        TEST_ASSERT_TRUE(length <= max_length);

        free(solution);
    }
}

void test_random_full_solver_with_random_scrambles_single_solution() {
    coord_cube_t *cube = get_coord_cube();

    for (int i = 0; i < 100; i++) {
        printf("\n\n\nNew test:\n");
        reset_coord_cube(cube);
        scramble_cube(cube, 50);

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

// What does this do?
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

            scramble_cube(cube, 50);

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
    RUN_TEST(test_phase1_solution_generator);
    RUN_TEST(test_phase1_solution_count);
    // RUN_TEST(test_random_phase2_solving);
    // RUN_TEST(test_facelets_solve_with_max_length);
    // RUN_TEST(test_random_full_solver_with_random_scrambles_single_solution);
    RUN_TEST(test_random_full_solver_with_random_scrambles_multiple_solution);
    // RUN_TEST(test_random_full_solver_with_sample_cubes_single_solution);
    // RUN_TEST(test_solve_with_move_blacklist); // Passing but way to slow

    return UNITY_END();
}
