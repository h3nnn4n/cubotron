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

static int solution_length(const move_t *solution) {
    int len = 0;
    while (solution[len] != MOVE_NULL)
        len++;
    return len;
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

void test_solved_cube_returns_zero_length() {
    coord_cube_t *cube = get_coord_cube();
    reset_coord_cube(cube);

    solve_list_t *solutions = solve_single(cube);
    TEST_ASSERT_NOT_NULL(solutions);
    TEST_ASSERT_NOT_NULL(solutions->solution);

    int length = 0;
    for (int i = 0; solutions->solution[i] != MOVE_NULL; i++) length++;
    TEST_ASSERT_EQUAL(0, length);

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

void test_multiple_solutions() {
    config_t *config = get_config();
    int       orig_n_solutions = config->n_solutions;
    int       orig_max_depth   = config->max_depth;
    config->n_solutions = 3;
    config->max_depth   = 12;

    coord_cube_t *cube = get_coord_cube();
    scramble_cube(cube, 10);

    solve_list_t *solutions = solve(cube, config);

    int count = 0;
    for (solve_list_t *cur = solutions; cur != NULL && cur->solution != NULL; cur = cur->next) {
        coord_cube_t *test_cube = get_coord_cube();
        copy_coord_cube(test_cube, cube);
        for (int i = 0; cur->solution[i] != MOVE_NULL; i++) {
            coord_apply_move(test_cube, cur->solution[i]);
        }
        TEST_ASSERT_TRUE(is_coord_solved(test_cube));
        free(test_cube);
        count++;
    }

    TEST_ASSERT_TRUE(count >= 1);

    destroy_solve_list(solutions);
    free(cube);

    config->n_solutions = orig_n_solutions;
    config->max_depth   = orig_max_depth;
}

void test_are_solutions_equal() {
    move_t a[] = {MOVE_U1, MOVE_R2, MOVE_NULL};
    move_t b[] = {MOVE_U1, MOVE_R2, MOVE_NULL};
    move_t c[] = {MOVE_U1, MOVE_R3, MOVE_NULL};
    move_t d[] = {MOVE_U1, MOVE_NULL};
    move_t e[] = {MOVE_NULL};

    move_t f[] = {MOVE_U2, MOVE_R2, MOVE_NULL};
    move_t g[] = {MOVE_R2, MOVE_NULL};

    TEST_ASSERT_TRUE(are_solutions_equal(a, b));
    TEST_ASSERT_TRUE(are_solutions_equal(e, e));
    TEST_ASSERT_FALSE(are_solutions_equal(a, c));
    TEST_ASSERT_FALSE(are_solutions_equal(a, d));
    TEST_ASSERT_FALSE(are_solutions_equal(a, e));
    TEST_ASSERT_FALSE(are_solutions_equal(a, f));
    TEST_ASSERT_FALSE(are_solutions_equal(a, g));
}

void test_multi_solution_no_duplicates() {
    config_t *config    = get_config();
    int       orig_n    = config->n_solutions;
    int       orig_d    = config->max_depth;
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
            if (are_solutions_equal(sols[i], sols[j])) {
                TEST_FAIL_MESSAGE("duplicate solution found");
            }
        }
    }

    free(cube);
    destroy_solve_list(solutions);

    config->n_solutions = orig_n;
    config->max_depth   = orig_d;
}

void test_stats_per_thread_consistent() {
    config_t *config   = get_config();
    int       orig_n   = config->n_solutions;
    int       orig_d   = config->max_depth;
    config->n_solutions = 3;
    config->max_depth   = 15;

    coord_cube_t *cube = get_coord_cube();
    scramble_cube(cube, 5);

    solve_list_t *solutions = solve(cube, config);
    TEST_ASSERT_NOT_NULL(solutions);
    TEST_ASSERT_NOT_NULL(solutions->stats);

    solve_stats_t *stats = solutions->stats;

    TEST_ASSERT_EQUAL(stats->phase1_move_count + stats->phase2_move_count, stats->total_moves);

    float phase_sum = stats->phase1_solve_time + stats->total_phase2_time;
    TEST_ASSERT_TRUE(stats->wall_time >= phase_sum - 0.001f);

    int sol_len = solution_length(solutions->solution);
    TEST_ASSERT_EQUAL(sol_len, stats->solution_length);

    free(cube);
    destroy_solve_list(solutions);

    config->n_solutions = orig_n;
    config->max_depth   = orig_d;
}

void test_aggregate_consistent() {
    config_t *config   = get_config();
    int       orig_n   = config->n_solutions;
    int       orig_d   = config->max_depth;
    config->n_solutions = 1;
    config->max_depth   = 15;

    coord_cube_t *cube = get_coord_cube();
    scramble_cube(cube, 5);

    solve_list_t *solutions = solve(cube, config);
    TEST_ASSERT_NOT_NULL(solutions);
    TEST_ASSERT_NOT_NULL(solutions->aggregate);

    aggregate_stats_t *agg = solutions->aggregate;

    TEST_ASSERT_TRUE(agg->total_moves_all_threads > 0);
    TEST_ASSERT_TRUE(agg->overall_wall_time > 0);

    float expected_mps = (float)agg->total_moves_all_threads / agg->overall_wall_time;
    TEST_ASSERT_FLOAT_WITHIN(1.0f, expected_mps, agg->overall_moves_per_second);

    TEST_ASSERT_EQUAL(agg->thread_count, agg->threads_die_aborted + agg->threads_completed);
    TEST_ASSERT_TRUE(agg->total_phase2_successes <= agg->total_phase2_attempts);
    TEST_ASSERT_TRUE(agg->solution_lengths_count >= 1);

    free(cube);
    destroy_solve_list(solutions);

    config->n_solutions = orig_n;
    config->max_depth   = orig_d;
}

void test_is_duplicate_solution() {
    solve_list_t *head = new_solve_list_node();
    solve_list_t *empty = new_solve_list_node();

    move_t sol1[] = {MOVE_U1, MOVE_R2, MOVE_NULL};
    move_t sol2[] = {MOVE_U2, MOVE_R2, MOVE_NULL};

    head->solution = sol1;
    head->next = new_solve_list_node();
    move_t sol3[] = {MOVE_D1, MOVE_NULL};
    head->next->solution = sol3;

    TEST_ASSERT_TRUE(is_duplicate_solution(head, sol1));
    TEST_ASSERT_TRUE(is_duplicate_solution(head, sol3));
    TEST_ASSERT_FALSE(is_duplicate_solution(head, sol2));
    TEST_ASSERT_FALSE(is_duplicate_solution(empty, sol1));

    head->solution = NULL;
    head->next->solution = NULL;
    destroy_solve_list(head);
    destroy_solve_list(empty);
}

void test_truncate_solutions() {
    solve_list_t *head = new_solve_list_node();
    solve_list_t *cur  = head;

    cur->solution = malloc(sizeof(move_t) * 2);
    cur->solution[0] = MOVE_U1;
    cur->solution[1] = MOVE_NULL;

    for (int i = 1; i < 5; i++) {
        cur->next       = new_solve_list_node();
        cur             = cur->next;
        cur->solution   = malloc(sizeof(move_t) * 2);
        cur->solution[0] = MOVE_U1 + i;
        cur->solution[1] = MOVE_NULL;
    }

    truncate_solutions(head, 3);

    int count = 0;
    for (solve_list_t *n = head; n != NULL && n->solution != NULL; n = n->next)
        count++;
    TEST_ASSERT_EQUAL(3, count);

    truncate_solutions(head, 1);
    count = 0;
    for (solve_list_t *n = head; n != NULL && n->solution != NULL; n = n->next)
        count++;
    TEST_ASSERT_EQUAL(1, count);

    destroy_solve_list(head);
}

void test_phase1_only_solution() {
    config_t *config = get_config();
    config->n_solutions = 0;
    config->max_depth   = 15;

    coord_cube_t *cube = get_coord_cube();
    scramble_cube(cube, 5);
    coord_cube_t *original = get_coord_cube();
    copy_coord_cube(original, cube);

    solve_list_t *solutions = solve(cube, config);
    TEST_ASSERT_NOT_NULL(solutions);
    TEST_ASSERT_NOT_NULL(solutions->solution);
    TEST_ASSERT_NOT_NULL(solutions->phase1_solution);

    coord_cube_t *test = get_coord_cube();
    copy_coord_cube(test, original);

    for (int i = 0; solutions->solution[i] != MOVE_NULL; i++)
        coord_apply_move(test, solutions->solution[i]);

    TEST_ASSERT_TRUE(is_phase1_solved(test));

    config->n_solutions = 1;
    config->max_depth   = 25;

    free(original);
    free(test);
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
    RUN_TEST(test_solved_cube_returns_zero_length);
    RUN_TEST(test_multiple_solutions);
    RUN_TEST(test_are_solutions_equal);
    RUN_TEST(test_multi_solution_no_duplicates);
    RUN_TEST(test_is_duplicate_solution);
    RUN_TEST(test_truncate_solutions);
    RUN_TEST(test_phase1_only_solution);
    RUN_TEST(test_stats_per_thread_consistent);
    RUN_TEST(test_aggregate_consistent);
    RUN_TEST(test_edge_case_single_move_scrambles);
    RUN_TEST(test_solution_correctness_varied_depths);
    RUN_TEST(test_all_sample_facelets_produce_valid_solutions);
    // RUN_TEST(test_random_phase2_solving);
    // RUN_TEST(test_random_full_solver_with_random_scrambles_multiple_solution);

    // RUN_TEST(test_random_full_solver_with_sample_cubes_single_solution);
    // RUN_TEST(test_facelets_solve_with_max_length);
    // RUN_TEST(test_random_full_solver_with_random_scrambles_single_solution);
    // RUN_TEST(test_solve_with_move_blacklist); // Passing but way to slow

    return UNITY_END();
}
