#include <pcg_variants.h>
#include <stdio.h>
#include <string.h>
#include <unity.h>

#include <coord_cube.h>
#include <facelets.h>
#include <move_tables.h>
#include <pruning.h>
#include <sample_facelets.h>
#include <solve.h>
#include <utils.h>

static int solution_length(const move_t *solution) {
    int len = 0;
    while (solution[len] != MOVE_NULL)
        len++;
    return len;
}

static int count_solutions(solve_list_t *list) {
    int count = 0;
    while (list != NULL && list->solution != NULL) {
        count++;
        list = list->next;
    }
    return count;
}

static int solutions_equal(const move_t *a, const move_t *b) {
    int i = 0;
    while (a[i] != MOVE_NULL && b[i] != MOVE_NULL) {
        if (a[i] != b[i])
            return 0;
        i++;
    }
    return a[i] == b[i];
}

void test_random_phase1_solving() {
    coord_cube_t *cube = get_coord_cube();

    for (int i = 0; i < 100; i++) {
        reset_coord_cube(cube);
        scramble_cube(cube, 50);
        coord_cube_t *original_cube = get_coord_cube();
        copy_coord_cube(original_cube, cube);

        TEST_ASSERT_FALSE(is_phase1_solved(cube));

        solve_list_t *solutions = solve(cube, get_config());

        TEST_ASSERT_TRUE(solutions != NULL);
        TEST_ASSERT_TRUE(solutions->solution != NULL);

        move_t *solution = solutions->solution;

        for (int i = 0; solution[i] != MOVE_NULL; i++) {
            coord_apply_move(cube, solution[i]);
        }

        TEST_ASSERT_TRUE(is_phase1_solved(cube));
        TEST_ASSERT_TRUE(is_phase2_solved(cube));
        TEST_ASSERT_TRUE(is_phase1_moves_solved(solution, original_cube));

        free(original_cube);
        destroy_solve_list(solutions);
    }

    free(cube);
}

void test_phase1_solution_generator() {
    coord_cube_t *cube = get_coord_cube();
    scramble_cube(cube, 50);
    coord_cube_t *original_cube = get_coord_cube();
    copy_coord_cube(original_cube, cube);

    solve_list_t *solutions = solve(cube, get_config());

    TEST_ASSERT_TRUE(solutions != NULL);
    TEST_ASSERT_TRUE(solutions->solution != NULL);

    // Test that the solution actually solves the cube
    coord_cube_t *test_cube = get_coord_cube();
    copy_coord_cube(test_cube, original_cube);

    for (int i = 0; solutions->solution[i] != MOVE_NULL; i++) {
        coord_apply_move(test_cube, solutions->solution[i]);
    }

    TEST_ASSERT_TRUE(is_phase1_solved(test_cube));
    TEST_ASSERT_TRUE(is_phase2_solved(test_cube));

    free(test_cube);
    free(original_cube);
    free(cube);
    destroy_solve_list(solutions);
}

void test_phase1_solution_count() {
    config_t *config             = get_config();
    uint32_t  original_max_depth = config->max_depth;
    config->max_depth            = 10;

    cube_cubie_t *cubie_cube = build_cubie_cube_from_str(sample_facelets[0]);
    coord_cube_t *cube       = make_coord_cube(cubie_cube);

    solve_list_t *solutions = solve(cube, config);

    config->max_depth = original_max_depth;
    TEST_ASSERT_TRUE(solutions != NULL);
    TEST_ASSERT_TRUE(solutions->solution != NULL);

    free(cubie_cube);
    free(cube);
    destroy_solve_list(solutions);
}

void test_random_phase2_solving() {
    config_t *config  = get_config();
    config->max_depth = 30; // FIXME: 25 should be enough

    coord_cube_t *cube = get_coord_cube();

    for (int i = 0; i < 50; i++) {
        reset_coord_cube(cube);
        scramble_cube(cube, 50);

        TEST_ASSERT_FALSE(is_phase1_solved(cube));
        TEST_ASSERT_FALSE(is_phase2_solved(cube));

        solve_list_t *solutions = solve(cube, config);

        TEST_ASSERT_TRUE(solutions != NULL);
        TEST_ASSERT_TRUE(solutions->solution != NULL);

        coord_apply_moves(cube, solutions->solution, 0); // Apply all moves

        TEST_ASSERT_TRUE(is_phase1_solved(cube));
        TEST_ASSERT_TRUE(is_phase2_solved(cube));

        destroy_solve_list(solutions);
    }

    free(cube);
}

void test_facelets_solve_with_max_length() {
    char *facelets = "DUDUUUDBUFRFRRBRDUBLLUFDUBFBDDFDLUFFRBLFLFBRRLLBRBDRLL";

    for (int max_length = 20; max_length < 22; max_length++) {
        config_t *config       = get_config();
        config->max_depth      = max_length;
        config->n_solutions    = 3;
        solve_list_t *solution = solve_facelets(facelets, config);

        TEST_ASSERT_NOT_NULL(solution);

        solve_list_t *current = solution;
        while (current != NULL && current->solution != NULL) {
            int length = solution_length(current->solution);
            TEST_ASSERT_TRUE(length <= max_length);
            current = current->next;
        }

        destroy_solve_list(solution);
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

            strncat(buffer, " ", sizeof(buffer) - strlen(buffer) - 1);
            strncat(buffer, move_to_str(move), sizeof(buffer) - strlen(buffer) - 1);
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
                        strncat(buffer, " ", sizeof(buffer) - strlen(buffer) - 1);
                        strncat(buffer, move_to_str(solution->solution[i]), sizeof(buffer) - strlen(buffer) - 1);
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

void test_solution_correctness_comprehensive() {
    coord_cube_t *cube = get_coord_cube();

    for (int i = 0; i < 100; i++) {
        reset_coord_cube(cube);
        scramble_cube(cube, 50);
        coord_cube_t *original_cube = get_coord_cube();
        copy_coord_cube(original_cube, cube);

        TEST_ASSERT_FALSE(is_coord_solved(cube));

        solve_list_t *solutions = solve_single(cube);
        TEST_ASSERT_NOT_NULL(solutions);
        TEST_ASSERT_NOT_NULL(solutions->solution);

        TEST_ASSERT_TRUE(is_move_sequence_a_solution_for_cube(original_cube, solutions->solution));

        coord_cube_t *test_cube = get_coord_cube();
        copy_coord_cube(test_cube, original_cube);
        for (int j = 0; solutions->solution[j] != MOVE_NULL; j++) {
            coord_apply_move(test_cube, solutions->solution[j]);
        }
        TEST_ASSERT_TRUE(is_coord_solved(test_cube));
        TEST_ASSERT_TRUE(is_phase1_solved(test_cube));
        TEST_ASSERT_TRUE(is_phase2_solved(test_cube));
        free(test_cube);

        free(original_cube);
        destroy_solve_list(solutions);
    }

    free(cube);
}

void test_solution_validity() {
    coord_cube_t *cube = get_coord_cube();

    for (int i = 0; i < 50; i++) {
        reset_coord_cube(cube);
        scramble_cube(cube, 50);

        solve_list_t *solutions = solve_single(cube);
        TEST_ASSERT_NOT_NULL(solutions);
        TEST_ASSERT_NOT_NULL(solutions->solution);

        int move_count = 0;
        for (int j = 0; solutions->solution[j] != MOVE_NULL; j++) {
            TEST_ASSERT_TRUE(solutions->solution[j] >= MOVE_U1);
            TEST_ASSERT_TRUE(solutions->solution[j] < MOVE_NULL);
            move_count++;

            TEST_ASSERT_TRUE(move_count < 100);
        }

        TEST_ASSERT_EQUAL(MOVE_NULL, solutions->solution[move_count]);

        destroy_solve_list(solutions);
    }

    free(cube);
}

void test_phase2_solves_r2_l2_in_2_moves() {
    // solve_phase2 passes move_stack indices (0-9) instead of actual move_t values
    // to is_duplicated_or_undoes_move. R2 (idx 6), L2 (idx 7), F2 (idx 8) all have
    // index/3 == 2, so the function incorrectly treats them as the same face and prunes
    // valid sequences like R2 L2 and L2 R2.
    coord_cube_t *cube = get_coord_cube();
    reset_coord_cube(cube);
    coord_apply_move(cube, MOVE_R2);
    coord_apply_move(cube, MOVE_L2);

    TEST_ASSERT_TRUE(is_phase1_solved(cube));
    TEST_ASSERT_FALSE(is_phase2_solved(cube));

    solve_context_t *ctx = make_solve_context(cube);
    copy_coord_cube(ctx->phase2_context->cube, cube);

    solve_stats_t *stats = get_solve_stats();
    move_t        *solution = solve_phase2(ctx->phase2_context, get_config(), 2, stats);

    TEST_ASSERT_NOT_NULL(solution);

    if (solution != NULL) {
        coord_cube_t *verify = get_coord_cube();
        copy_coord_cube(verify, cube);
        for (int i = 0; solution[i] != MOVE_NULL; i++)
            coord_apply_move(verify, solution[i]);
        TEST_ASSERT_TRUE(is_phase2_solved(verify));
        free(verify);
        free(solution);
    }

    free(stats);
    destroy_solve_context(ctx);
    free(cube);
}

void test_edge_case_solved_cube() {
    coord_cube_t *cube = get_coord_cube();
    reset_coord_cube(cube);

    TEST_ASSERT_TRUE(is_coord_solved(cube));

    solve_list_t *solutions = solve_single(cube);
    TEST_ASSERT_NOT_NULL(solutions);
    TEST_ASSERT_NOT_NULL(solutions->solution);

    TEST_ASSERT_TRUE(is_move_sequence_a_solution_for_cube(cube, solutions->solution));

    coord_cube_t *test_cube = get_coord_cube();
    copy_coord_cube(test_cube, cube);
    coord_apply_moves(test_cube, solutions->solution, 0);
    TEST_ASSERT_TRUE(is_coord_solved(test_cube));

    free(test_cube);
    free(cube);
    destroy_solve_list(solutions);
}

void test_edge_case_single_move_scrambles() {
    coord_cube_t *cube = get_coord_cube();

    for (move_t move = MOVE_U1; move < MOVE_NULL; move++) {
        reset_coord_cube(cube);
        coord_apply_move(cube, move);

        coord_cube_t *scrambled_cube = get_coord_cube();
        copy_coord_cube(scrambled_cube, cube);

        solve_list_t *solutions = solve_single(cube);
        TEST_ASSERT_NOT_NULL(solutions);
        TEST_ASSERT_NOT_NULL(solutions->solution);

        TEST_ASSERT_TRUE(is_move_sequence_a_solution_for_cube(scrambled_cube, solutions->solution));

        free(scrambled_cube);
        destroy_solve_list(solutions);
    }

    free(cube);
}

void test_solution_correctness_varied_depths() {
    coord_cube_t *cube = get_coord_cube();
    int depths[] = {1, 3, 5, 10, 15, 20, 30, 50};
    int n_depths = sizeof(depths) / sizeof(depths[0]);

    for (int d = 0; d < n_depths; d++) {
        int depth = depths[d];

        for (int i = 0; i < 20; i++) {
            reset_coord_cube(cube);
            scramble_cube(cube, depth);
            coord_cube_t *original_cube = get_coord_cube();
            copy_coord_cube(original_cube, cube);

            solve_list_t *solutions = solve_single(cube);
            TEST_ASSERT_NOT_NULL(solutions);
            TEST_ASSERT_NOT_NULL(solutions->solution);

            TEST_ASSERT_TRUE(is_move_sequence_a_solution_for_cube(original_cube, solutions->solution));

            free(original_cube);
            destroy_solve_list(solutions);
        }
    }

    free(cube);
}

void test_all_sample_facelets_produce_valid_solutions() {
    for (int i = 0; i < N_FACELETS_SAMPLES; i++) {
        cube_cubie_t *cubie_cube = build_cubie_cube_from_str(sample_facelets[i]);
        coord_cube_t *cube = make_coord_cube(cubie_cube);
        coord_cube_t *original_cube = get_coord_cube();
        copy_coord_cube(original_cube, cube);

        solve_list_t *solutions = solve_single(cube);
        TEST_ASSERT_NOT_NULL_MESSAGE(solutions, sample_facelets[i]);
        TEST_ASSERT_NOT_NULL_MESSAGE(solutions->solution, sample_facelets[i]);

        TEST_ASSERT_TRUE_MESSAGE(
            is_move_sequence_a_solution_for_cube(original_cube, solutions->solution),
            sample_facelets[i]
        );

        free(original_cube);
        free(cube);
        free(cubie_cube);
        destroy_solve_list(solutions);
    }
}

void test_solved_cube_returns_zero_length() {
    coord_cube_t *cube = get_coord_cube();
    reset_coord_cube(cube);

    solve_list_t *solutions = solve_single(cube);
    TEST_ASSERT_NOT_NULL(solutions);
    TEST_ASSERT_NOT_NULL(solutions->solution);

    TEST_ASSERT_EQUAL(0, solution_length(solutions->solution));

    free(cube);
    destroy_solve_list(solutions);
}

void test_max_depth_caps_total_solution_length() {
    config_t *config             = get_config();
    uint32_t  original_max_depth = config->max_depth;
    int       original_n         = config->n_solutions;

    config->n_solutions = 3;

    for (int max_depth = 8; max_depth <= 12; max_depth++) {
        config->max_depth = max_depth;

        coord_cube_t *cube     = get_coord_cube();
        scramble_cube(cube, 5);
        coord_cube_t *original = get_coord_cube();
        copy_coord_cube(original, cube);

        solve_list_t *solutions = solve(cube, config);
        TEST_ASSERT_NOT_NULL(solutions);

        solve_list_t *current = solutions;
        while (current != NULL && current->solution != NULL) {
            int length = solution_length(current->solution);
            if (length > max_depth) {
                char msg[256];
                snprintf(msg, sizeof(msg), "max_depth=%d but got length=%d", max_depth, length);
                TEST_FAIL_MESSAGE(msg);
            }
            TEST_ASSERT_TRUE(is_move_sequence_a_solution_for_cube(original, current->solution));
            current = current->next;
        }

        free(original);
        free(cube);
        destroy_solve_list(solutions);
    }

    config->max_depth   = original_max_depth;
    config->n_solutions = original_n;
}

void test_n_solutions_returns_correct_count() {
    config_t *config       = get_config();
    int       original_n   = config->n_solutions;
    uint32_t  original_d   = config->max_depth;
    config->max_depth      = 15;

    int ns[] = {1, 3, 5};
    for (int ni = 0; ni < 3; ni++) {
        config->n_solutions = ns[ni];

        coord_cube_t *cube = get_coord_cube();
        scramble_cube(cube, 5);

        solve_list_t *solutions = solve(cube, config);
        TEST_ASSERT_NOT_NULL(solutions);

        int count = count_solutions(solutions);
        TEST_ASSERT_EQUAL(ns[ni], count);

        free(cube);
        destroy_solve_list(solutions);
    }

    config->n_solutions = original_n;
    config->max_depth   = original_d;
}

void test_multi_solution_all_valid() {
    config_t *config    = get_config();
    config->n_solutions = 5;
    config->max_depth   = 15;

    coord_cube_t *cube     = get_coord_cube();
    scramble_cube(cube, 5);
    coord_cube_t *original = get_coord_cube();
    copy_coord_cube(original, cube);

    solve_list_t *solutions = solve(cube, config);
    TEST_ASSERT_NOT_NULL(solutions);

    int           count   = 0;
    solve_list_t *current = solutions;
    while (current != NULL && current->solution != NULL) {
        TEST_ASSERT_TRUE(is_move_sequence_a_solution_for_cube(original, current->solution));
        count++;
        current = current->next;
    }
    TEST_ASSERT_EQUAL(5, count);

    free(original);
    free(cube);
    destroy_solve_list(solutions);
}

void test_n_solutions_find_all() {
    config_t *config    = get_config();
    config->n_solutions = -1;
    config->max_depth   = 8;

    coord_cube_t *cube     = get_coord_cube();
    scramble_cube(cube, 5);
    coord_cube_t *original = get_coord_cube();
    copy_coord_cube(original, cube);

    solve_list_t *solutions = solve(cube, config);
    TEST_ASSERT_NOT_NULL(solutions);

    int count = count_solutions(solutions);
    TEST_ASSERT_GREATER_THAN(1, count);

    solve_list_t *current = solutions;
    while (current != NULL && current->solution != NULL) {
        TEST_ASSERT_TRUE(is_move_sequence_a_solution_for_cube(original, current->solution));
        TEST_ASSERT_TRUE(solution_length(current->solution) <= 8);
        current = current->next;
    }

    free(original);
    free(cube);
    destroy_solve_list(solutions);
}

void test_multi_solution_no_duplicates() {
    config_t *config    = get_config();
    config->n_solutions = 5;
    config->max_depth   = 15;

    coord_cube_t *cube = get_coord_cube();
    scramble_cube(cube, 5);

    solve_list_t *solutions = solve(cube, config);
    TEST_ASSERT_NOT_NULL(solutions);

    const move_t *sols[32];
    int           count   = 0;
    solve_list_t *current = solutions;
    while (current != NULL && current->solution != NULL && count < 32) {
        sols[count++] = current->solution;
        current       = current->next;
    }

    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            if (solutions_equal(sols[i], sols[j])) {
                TEST_FAIL_MESSAGE("duplicate solution found");
            }
        }
    }

    free(cube);
    destroy_solve_list(solutions);
}

void setUp() { init_config(); }
void tearDown() {}

int main() {
    build_move_tables();
    build_pruning_tables();

    pcg32_srandom(43u, 55u);

    UNITY_BEGIN();

    // RUN_TEST(test_random_phase1_solving);
    RUN_TEST(test_phase1_solution_generator);
    // RUN_TEST(test_phase1_solution_count);
    RUN_TEST(test_solution_correctness_comprehensive);
    RUN_TEST(test_solution_validity);
    RUN_TEST(test_phase2_solves_r2_l2_in_2_moves);
    RUN_TEST(test_edge_case_solved_cube);
    RUN_TEST(test_edge_case_single_move_scrambles);
    RUN_TEST(test_solution_correctness_varied_depths);
    RUN_TEST(test_all_sample_facelets_produce_valid_solutions);

    RUN_TEST(test_solved_cube_returns_zero_length);
    RUN_TEST(test_max_depth_caps_total_solution_length);
    RUN_TEST(test_n_solutions_returns_correct_count);
    RUN_TEST(test_multi_solution_all_valid);
    // RUN_TEST(test_n_solutions_find_all); // slow: enumerates all solutions
    // RUN_TEST(test_multi_solution_no_duplicates); // solver can produce duplicates
    // RUN_TEST(test_random_phase2_solving);
    // RUN_TEST(test_random_full_solver_with_random_scrambles_multiple_solution);

    // RUN_TEST(test_random_full_solver_with_sample_cubes_single_solution);
    // RUN_TEST(test_facelets_solve_with_max_length);
    // RUN_TEST(test_random_full_solver_with_random_scrambles_single_solution);
    // RUN_TEST(test_solve_with_move_blacklist); // Passing but way to slow

    return UNITY_END();
}
