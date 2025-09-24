#include <pcg_variants.h>
#include <unity.h>

#include <move_tables.h>
#include <utils.h>

void test_random_cubie_cube_not_solved() {
    cube_cubie_t *cube = random_cubie_cube();

    TEST_ASSERT_FALSE(is_cubie_solved(cube));
}

void test_random_cubie_cube() {
    cube_cubie_t *cube = random_cubie_cube();

    TEST_ASSERT_NOT_EQUAL_INT(0, get_edge_orientations(cube));
    TEST_ASSERT_NOT_EQUAL_INT(0, get_corner_orientations(cube));
}

void test_random_coord_cube() {
    coord_cube_t *cube = random_coord_cube();

    TEST_ASSERT_NOT_EQUAL_INT(0, cube->edge_orientations);
    TEST_ASSERT_NOT_EQUAL_INT(0, cube->corner_orientations);
}

void test_rotate_right_reverts_rotate_left() {
    cube_cubie_t *cube      = init_cubie_cube();
    cube_cubie_t *reference = init_cubie_cube();

    for (int i = 1; i < 6; i++) {
        rotate_left((int *)cube->corner_permutations, 1, i + 1);
        rotate_right((int *)cube->corner_permutations, 1, i + 1);

        TEST_ASSERT_EQUAL_INT_ARRAY(reference->corner_permutations, cube->corner_permutations, N_CORNERS);
    }

    free(cube);
}

void test_rotate_right_diameter() {
    cube_cubie_t *cube      = init_cubie_cube();
    cube_cubie_t *reference = init_cubie_cube();

    rotate_right((int *)cube->corner_permutations, 1, 4);
    rotate_right((int *)cube->corner_permutations, 1, 4);
    rotate_right((int *)cube->corner_permutations, 1, 4);
    rotate_right((int *)cube->corner_permutations, 1, 4);

    TEST_ASSERT_EQUAL_INT_ARRAY(reference->corner_permutations, cube->corner_permutations, N_CORNERS);

    free(cube);
}

void test_rotate_left_diameter() {
    cube_cubie_t *cube      = init_cubie_cube();
    cube_cubie_t *reference = init_cubie_cube();

    rotate_left((int *)cube->corner_permutations, 1, 4);
    rotate_left((int *)cube->corner_permutations, 1, 4);
    rotate_left((int *)cube->corner_permutations, 1, 4);
    rotate_left((int *)cube->corner_permutations, 1, 4);

    TEST_ASSERT_EQUAL_INT_ARRAY(reference->corner_permutations, cube->corner_permutations, N_CORNERS);

    free(cube);
}

void test_is_bad_move_true_cases() {
    for (int i = 0; i < N_COLORS; i++) {
        int offset = i * 3;

        TEST_ASSERT_TRUE(is_duplicated_or_undoes_move(MOVE_U1 + offset, MOVE_U1 + offset));
        TEST_ASSERT_TRUE(is_duplicated_or_undoes_move(MOVE_U1 + offset, MOVE_U2 + offset));
        TEST_ASSERT_TRUE(is_duplicated_or_undoes_move(MOVE_U1 + offset, MOVE_U3 + offset));

        TEST_ASSERT_TRUE(is_duplicated_or_undoes_move(MOVE_U2 + offset, MOVE_U1 + offset));
        TEST_ASSERT_TRUE(is_duplicated_or_undoes_move(MOVE_U2 + offset, MOVE_U2 + offset));
        TEST_ASSERT_TRUE(is_duplicated_or_undoes_move(MOVE_U2 + offset, MOVE_U3 + offset));

        TEST_ASSERT_TRUE(is_duplicated_or_undoes_move(MOVE_U3 + offset, MOVE_U1 + offset));
        TEST_ASSERT_TRUE(is_duplicated_or_undoes_move(MOVE_U3 + offset, MOVE_U2 + offset));
        TEST_ASSERT_TRUE(is_duplicated_or_undoes_move(MOVE_U3 + offset, MOVE_U3 + offset));
    }
}

void test_is_bad_move_false_cases() {
    for (int i = 0; i < N_COLORS; i++) {
        int offset1 = i * 3;

        for (int j = i + 1; j < N_COLORS; j++) {
            int offset2 = j * 3;

            TEST_ASSERT_FALSE(is_duplicated_or_undoes_move(MOVE_U1 + offset1, MOVE_U1 + offset2));
            TEST_ASSERT_FALSE(is_duplicated_or_undoes_move(MOVE_U1 + offset1, MOVE_U2 + offset2));
            TEST_ASSERT_FALSE(is_duplicated_or_undoes_move(MOVE_U1 + offset1, MOVE_U3 + offset2));

            TEST_ASSERT_FALSE(is_duplicated_or_undoes_move(MOVE_U2 + offset1, MOVE_U1 + offset2));
            TEST_ASSERT_FALSE(is_duplicated_or_undoes_move(MOVE_U2 + offset1, MOVE_U2 + offset2));
            TEST_ASSERT_FALSE(is_duplicated_or_undoes_move(MOVE_U2 + offset1, MOVE_U3 + offset2));

            TEST_ASSERT_FALSE(is_duplicated_or_undoes_move(MOVE_U3 + offset1, MOVE_U1 + offset2));
            TEST_ASSERT_FALSE(is_duplicated_or_undoes_move(MOVE_U3 + offset1, MOVE_U2 + offset2));
            TEST_ASSERT_FALSE(is_duplicated_or_undoes_move(MOVE_U3 + offset1, MOVE_U3 + offset2));
        }
    }
}

void test_move_sequence_str_to_moves() {
    move_t *moves            = move_sequence_str_to_moves("U R2 F D' L B");
    move_t  expected_moves[] = {MOVE_U1, MOVE_R2, MOVE_F1, MOVE_D3, MOVE_L1, MOVE_B1, MOVE_NULL};

    for (int i = 0; i < 7; i++) {
        TEST_ASSERT_EQUAL_INT(moves[i], expected_moves[i]);
    }

    free(moves);
}

void test_str_to_move() {
    // This actually tests that str_to_move and move_to_str are identity
    for (move_t move = MOVE_U1; move < MOVE_NULL; move++) {
        TEST_ASSERT_TRUE(str_to_move(move_to_str(move)) == move);
    }
}

void setUp() { build_move_tables(); }
void tearDown() {}

int main() {
    pcg32_srandom(43u, 55u);

    UNITY_BEGIN();

    RUN_TEST(test_random_cubie_cube_not_solved);

    RUN_TEST(test_random_cubie_cube);
    RUN_TEST(test_random_coord_cube);

    RUN_TEST(test_rotate_right_reverts_rotate_left);
    RUN_TEST(test_rotate_right_diameter);
    RUN_TEST(test_rotate_left_diameter);

    RUN_TEST(test_is_bad_move_true_cases);
    RUN_TEST(test_is_bad_move_false_cases);

    RUN_TEST(test_move_sequence_str_to_moves);
    RUN_TEST(test_str_to_move);

    return UNITY_END();
}
