#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include <config.h>
#include <definitions.h>
#include <puzzle_2x2.h>
#include <solver.h>
#include <solvers/solver_2x2_ida.h>

static int solution_length(const move_t *solution) {
    int len = 0;
    while (solution[len] != MOVE_NULL)
        len++;
    return len;
}

static int verify_solution(const char *facelets, const move_t *solution) {
    cube_2x2_t cube;
    puzzle_2x2_ops.from_string(&cube, facelets);

    for (int i = 0; solution[i] != MOVE_NULL; i++)
        puzzle_2x2_ops.apply_move(&cube, solution[i]);

    return puzzle_2x2_ops.is_solved(&cube);
}

void test_solved_cube_returns_trivial(void) {
    char facelets[N_FACELETS_2X2 + 1] = "UUUURRRRFFFFDDDDLLLLBBBB";

    solve_list_t *solutions = solve_puzzle("2x2", facelets, get_config());
    TEST_ASSERT_NOT_NULL(solutions);
    TEST_ASSERT_NOT_NULL(solutions->solution);
    TEST_ASSERT_EQUAL_INT(0, solution_length(solutions->solution));
    TEST_ASSERT_TRUE(verify_solution(facelets, solutions->solution));

    destroy_solve_list(solutions);
}

void test_single_move_scramble(void) {
    char facelets[N_FACELETS_2X2 + 1] = "UUUURRRRFFFFDDDDLLLLBBBB";
    cube_2x2_t cube;

    puzzle_2x2_ops.from_string(&cube, facelets);
    puzzle_2x2_ops.apply_move(&cube, MOVE_U1);
    puzzle_2x2_ops.to_string(&cube, facelets, sizeof(facelets));

    solve_list_t *solutions = solve_puzzle("2x2", facelets, get_config());
    TEST_ASSERT_NOT_NULL(solutions);
    TEST_ASSERT_NOT_NULL(solutions->solution);
    TEST_ASSERT_EQUAL_INT(1, solution_length(solutions->solution));
    TEST_ASSERT_TRUE(verify_solution(facelets, solutions->solution));

    destroy_solve_list(solutions);
}

void test_blacklist_excludes_move(void) {
    config_t *cfg = get_config();
    cfg->move_black_list[MOVE_U1] = MOVE_U1;

    char  facelets[N_FACELETS_2X2 + 1] = "UUUURRRRFFFFDDDDLLLLBBBB";
    move_t scramble_moves[]             = {MOVE_U1, MOVE_NULL};

    cube_2x2_t cube;
    puzzle_2x2_ops.from_string(&cube, facelets);
    for (int i = 0; scramble_moves[i] != MOVE_NULL; i++)
        puzzle_2x2_ops.apply_move(&cube, scramble_moves[i]);
    puzzle_2x2_ops.to_string(&cube, facelets, sizeof(facelets));

    solve_list_t *solutions = solve_puzzle("2x2", facelets, cfg);
    TEST_ASSERT_NOT_NULL(solutions);
    TEST_ASSERT_NOT_NULL(solutions->solution);

    for (int i = 0; solutions->solution[i] != MOVE_NULL; i++)
        TEST_ASSERT_NOT_EQUAL(MOVE_U1, solutions->solution[i]);

    TEST_ASSERT_TRUE(verify_solution(facelets, solutions->solution));
    destroy_solve_list(solutions);

    cfg->move_black_list[MOVE_U1] = MOVE_NULL;
}

void test_random_scrambles(void) {
    solver_2x2_ida_ops.init();

    char  facelets[N_FACELETS_2X2 + 1] = "UUUURRRRFFFFDDDDLLLLBBBB";
    move_t scramble_moves[]             = {MOVE_U1, MOVE_R1, MOVE_F1, MOVE_U3, MOVE_R3, MOVE_NULL};

    cube_2x2_t cube;
    puzzle_2x2_ops.from_string(&cube, facelets);
    for (int i = 0; scramble_moves[i] != MOVE_NULL; i++)
        puzzle_2x2_ops.apply_move(&cube, scramble_moves[i]);
    puzzle_2x2_ops.to_string(&cube, facelets, sizeof(facelets));

    solve_list_t *solutions = solve_puzzle("2x2", facelets, get_config());
    TEST_ASSERT_NOT_NULL(solutions);
    TEST_ASSERT_NOT_NULL(solutions->solution);
    TEST_ASSERT_TRUE(verify_solution(facelets, solutions->solution));

    destroy_solve_list(solutions);
}

void setUp(void) {}

void tearDown(void) {}

int main() {
    init_config();
    UNITY_BEGIN();

    RUN_TEST(test_solved_cube_returns_trivial);
    RUN_TEST(test_single_move_scramble);
    RUN_TEST(test_random_scrambles);
    RUN_TEST(test_blacklist_excludes_move);

    return UNITY_END();
}
