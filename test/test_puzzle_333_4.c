#include "puzzles/puzzle_333_4.h"
#include <unity.h>

#include <cubie_move_table.h>
#include <definitions.h>
#include <puzzle_333_4.h>

static void test_reset(void) {
    cube_333_4_t cube;
    puzzle_333_4_ops.reset(&cube);
    TEST_ASSERT_TRUE(puzzle_333_4_ops.is_solved(&cube));
}

static void test_move_changes_state(void) {
    cube_333_4_t cube;
    puzzle_333_4_ops.reset(&cube);

    puzzle_333_4_ops.apply_move(&cube, MOVE_F2);
    puzzle_333_4_ops.apply_move(&cube, MOVE_R1);
    TEST_ASSERT_FALSE(puzzle_333_4_ops.is_solved(&cube));

    puzzle_333_4_ops.reset(&cube);

    TEST_ASSERT_TRUE(puzzle_333_4_ops.is_solved(&cube));
}

static void test_copy(void) {
    cube_333_4_t cube_src;
    cube_333_4_t cube_dest;

    puzzle_333_4_ops.reset(&cube_src);
    puzzle_333_4_ops.reset(&cube_dest);

    puzzle_333_4_ops.apply_move(&cube_src, MOVE_F2);
    puzzle_333_4_ops.apply_move(&cube_src, MOVE_R1);
    TEST_ASSERT_FALSE(puzzle_333_4_ops.is_solved(&cube_src));

    TEST_ASSERT_TRUE(puzzle_333_4_ops.is_solved(&cube_dest));

    puzzle_333_4_ops.copy(&cube_dest, &cube_src);

    TEST_ASSERT_FALSE(puzzle_333_4_ops.is_solved(&cube_dest));
}

void setUp(void) { cubie_build_move_table(); }

void tearDown(void) {}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_reset);
    RUN_TEST(test_move_changes_state);
    RUN_TEST(test_copy);

    return UNITY_END();
}
