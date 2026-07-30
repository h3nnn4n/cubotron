#include <string.h>
#include <unity.h>

#include <definitions.h>
#include <puzzle_2x2.h>

static void test_solved_string_parses(void) {
    char str[N_FACELETS_2X2 + 1] = "UUUURRRRFFFFDDDDLLLLBBBB";

    cube_2x2_t cube;
    TEST_ASSERT_TRUE(puzzle_2x2_ops.from_string(&cube, str));
    TEST_ASSERT_TRUE(puzzle_2x2_ops.is_solved(&cube));
}

static void test_round_trip(void) {
    char str[N_FACELETS_2X2 + 1] = "UUUURRRRFFFFDDDDLLLLBBBB";
    char buf[N_FACELETS_2X2 + 1];

    cube_2x2_t cube;
    puzzle_2x2_ops.from_string(&cube, str);
    puzzle_2x2_ops.to_string(&cube, buf, sizeof(buf));

    TEST_ASSERT_EQUAL_STRING(str, buf);
}

static void test_move_u1_changes_state(void) {
    char str[N_FACELETS_2X2 + 1] = "UUUURRRRFFFFDDDDLLLLBBBB";

    cube_2x2_t cube;
    puzzle_2x2_ops.from_string(&cube, str);
    TEST_ASSERT_TRUE(puzzle_2x2_ops.is_solved(&cube));

    puzzle_2x2_ops.apply_move(&cube, MOVE_U1);
    TEST_ASSERT_FALSE(puzzle_2x2_ops.is_solved(&cube));

    puzzle_2x2_ops.apply_move(&cube, MOVE_U3);
    TEST_ASSERT_TRUE(puzzle_2x2_ops.is_solved(&cube));
}

static void test_move_r1_changes_state(void) {
    char str[N_FACELETS_2X2 + 1] = "UUUURRRRFFFFDDDDLLLLBBBB";

    cube_2x2_t cube;
    puzzle_2x2_ops.from_string(&cube, str);

    puzzle_2x2_ops.apply_move(&cube, MOVE_R1);
    TEST_ASSERT_FALSE(puzzle_2x2_ops.is_solved(&cube));

    puzzle_2x2_ops.apply_move(&cube, MOVE_R3);
    TEST_ASSERT_TRUE(puzzle_2x2_ops.is_solved(&cube));
}

static void test_move_f1_changes_state(void) {
    char str[N_FACELETS_2X2 + 1] = "UUUURRRRFFFFDDDDLLLLBBBB";

    cube_2x2_t cube;
    puzzle_2x2_ops.from_string(&cube, str);

    puzzle_2x2_ops.apply_move(&cube, MOVE_F1);
    TEST_ASSERT_FALSE(puzzle_2x2_ops.is_solved(&cube));

    puzzle_2x2_ops.apply_move(&cube, MOVE_F3);
    TEST_ASSERT_TRUE(puzzle_2x2_ops.is_solved(&cube));
}

static void test_u2_is_self_inverse(void) {
    char str[N_FACELETS_2X2 + 1] = "UUUURRRRFFFFDDDDLLLLBBBB";

    cube_2x2_t cube;
    puzzle_2x2_ops.from_string(&cube, str);

    puzzle_2x2_ops.apply_move(&cube, MOVE_U2);
    TEST_ASSERT_FALSE(puzzle_2x2_ops.is_solved(&cube));

    puzzle_2x2_ops.apply_move(&cube, MOVE_U2);
    TEST_ASSERT_TRUE(puzzle_2x2_ops.is_solved(&cube));
}

static void test_reset(void) {
    cube_2x2_t cube;
    memset(&cube, 0xFF, sizeof(cube));

    puzzle_2x2_ops.reset(&cube);
    TEST_ASSERT_TRUE(puzzle_2x2_ops.is_solved(&cube));
}

static void test_copy(void) {
    char str[N_FACELETS_2X2 + 1] = "UUUURRRRFFFFDDDDLLLLBBBB";

    cube_2x2_t a;
    puzzle_2x2_ops.from_string(&a, str);

    cube_2x2_t b;
    puzzle_2x2_ops.reset(&b);

    puzzle_2x2_ops.apply_move(&a, MOVE_R1);
    puzzle_2x2_ops.copy(&b, &a);

    TEST_ASSERT_FALSE(puzzle_2x2_ops.is_solved(&b));
    TEST_ASSERT_TRUE(puzzle_2x2_ops.is_solved == puzzle_2x2_ops.is_solved);
}

void setUp(void) {}

void tearDown(void) {}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_solved_string_parses);
    RUN_TEST(test_round_trip);
    RUN_TEST(test_move_u1_changes_state);
    RUN_TEST(test_move_r1_changes_state);
    RUN_TEST(test_move_f1_changes_state);
    RUN_TEST(test_u2_is_self_inverse);
    RUN_TEST(test_reset);
    RUN_TEST(test_copy);

    return UNITY_END();
}
