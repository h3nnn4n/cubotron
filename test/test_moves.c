#include <unity.h>

#include <cubie_move_table.h>
#include <definitions.h>
#include <utils.h>

void test_double_turn_moves_have_diameter_2() {
    move_t moves[] = {
        MOVE_U2, MOVE_R2, MOVE_F2, MOVE_D2, MOVE_L2, MOVE_B2,
    };

    for (int j = 0; j < N_COLORS; j++) {
        cube_cubie_t *cube = init_cubie_cube();

        cubie_apply_move(cube, moves[j]);
        TEST_ASSERT_FALSE(is_solved(cube));
        cubie_apply_move(cube, moves[j]);
        TEST_ASSERT_TRUE(is_solved(cube));

        free(cube);
    }
}

void test_half_turn_moves_have_diameter_4() {
    move_t moves[] = {
        MOVE_U1, MOVE_U3, MOVE_R1, MOVE_R3, MOVE_F1, MOVE_F3, MOVE_D1, MOVE_D3, MOVE_L1, MOVE_L3, MOVE_B1, MOVE_B3,
    };

    for (int j = 0; j < 12; j++) {
        cube_cubie_t *cube = init_cubie_cube();

        for (int i = 0; i < 3; i++) {
            cubie_apply_move(cube, moves[j]);
            TEST_ASSERT_FALSE(is_solved(cube));
        }

        cubie_apply_move(cube, moves[j]);

        TEST_ASSERT_TRUE(is_solved(cube));

        free(cube);
    }
}

void test_half_turn_moves_inverses() {
    move_t moves[] = {
        MOVE_U1, MOVE_R1, MOVE_F1, MOVE_D1, MOVE_L1, MOVE_B1,
    };

    for (int j = 0; j < N_COLORS; j++) {
        cube_cubie_t *cube = init_cubie_cube();

        // Tests that U3 undoes U1
        cubie_apply_move(cube, moves[j]);
        TEST_ASSERT_FALSE(is_solved(cube));
        cubie_apply_move(cube, moves[j] + 2); // U1 is 0 and U3 is U1 + 2, and so on
        TEST_ASSERT_TRUE(is_solved(cube));

        // Tests that U1 undoes U3
        cubie_apply_move(cube, moves[j] + 2);
        TEST_ASSERT_FALSE(is_solved(cube));
        cubie_apply_move(cube, moves[j]);
        TEST_ASSERT_TRUE(is_solved(cube));

        free(cube);
    }
}

void test_move_sequences_diameter() {
    move_t move_book[14][4] = {{MOVE_R1, MOVE_U1, MOVE_R3, MOVE_U3}, {MOVE_R3, MOVE_U1, MOVE_R1, MOVE_U3},
                               {MOVE_F1, MOVE_U1, MOVE_F3, MOVE_U3}, {MOVE_F3, MOVE_U1, MOVE_F1, MOVE_U3},
                               {MOVE_L1, MOVE_U1, MOVE_L3, MOVE_U3}, {MOVE_L3, MOVE_U1, MOVE_L1, MOVE_U3},
                               {MOVE_L1, MOVE_B1, MOVE_L3, MOVE_B3}, {MOVE_U3, MOVE_B1, MOVE_U1, MOVE_B3},
                               {MOVE_R1, MOVE_D1, MOVE_R3, MOVE_D3}, {MOVE_R3, MOVE_D1, MOVE_R1, MOVE_D3},
                               {MOVE_F1, MOVE_D1, MOVE_F3, MOVE_D3}, {MOVE_F3, MOVE_D1, MOVE_F1, MOVE_D3},
                               {MOVE_L1, MOVE_D1, MOVE_L3, MOVE_D3}, {MOVE_L3, MOVE_D1, MOVE_L1, MOVE_D3}};

    for (int move_book_index = 0; move_book_index < 14; move_book_index++) {
        /*char buffer[256] = "";*/
        /*for (int move_index = 0; move_index < 4; move_index++)*/
        /*sprintf(buffer, "%s %s", buffer, move_to_str(move_book[move_book_index][move_index]));*/
        /*TEST_MESSAGE(buffer);*/

        cube_cubie_t *cube = init_cubie_cube();

        for (int sequence_index = 0; sequence_index < 6; sequence_index++) {
            for (int move_index = 0; move_index < 4; move_index++) {
                cubie_apply_move(cube, move_book[move_book_index][move_index]);
            }

            if (sequence_index < 5) { // It gets solved on the last iteration
                TEST_ASSERT_FALSE(is_solved(cube));
            }
        }

        TEST_ASSERT_TRUE(is_solved(cube));

        free(cube);
    }
}

void test_move_sequences_diameter_2() {
    move_t move_book[4][16] = {
        {MOVE_R3, MOVE_U1, MOVE_R1, MOVE_U3, MOVE_R2, MOVE_F3, MOVE_U3, MOVE_F1, MOVE_U1, MOVE_R1, MOVE_F1, MOVE_R3,
         MOVE_F3, MOVE_R2, MOVE_U3, MOVE_NULL}, // F perm

        {MOVE_R1, MOVE_U1, MOVE_R3, MOVE_U3, MOVE_R3, MOVE_F1, MOVE_R2, MOVE_U3, MOVE_R3, MOVE_U3, MOVE_R1, MOVE_U1,
         MOVE_R3, MOVE_F3, MOVE_NULL, MOVE_NULL}, // T perm

        {MOVE_R3, MOVE_U1, MOVE_R3, MOVE_U3, MOVE_B3, MOVE_R3, MOVE_B2, MOVE_U3, MOVE_B3, MOVE_U1, MOVE_B3, MOVE_R1,
         MOVE_B1, MOVE_R1, MOVE_NULL, MOVE_NULL}, // V perm

        {MOVE_F2, MOVE_D1, MOVE_R2, MOVE_U1, MOVE_R2, MOVE_D3, MOVE_R3, MOVE_U3, MOVE_R1, MOVE_F2, MOVE_R3, MOVE_U1,
         MOVE_R1, MOVE_NULL, MOVE_NULL, MOVE_NULL} // Y perm
    };

    for (int move_book_index = 0; move_book_index < 4; move_book_index++) {
        /*char buffer[256] = "";*/
        /*for (int move_index = 0; move_index < 16; move_index++)*/
        /*sprintf(buffer, "%s %s", buffer, move_to_str(move_book[move_book_index][move_index]));*/
        /*TEST_MESSAGE(buffer);*/

        cube_cubie_t *cube = init_cubie_cube();

        for (int sequence_index = 0; sequence_index < 2; sequence_index++) {
            for (int move_index = 0; move_index < 16; move_index++) {
                cubie_apply_move(cube, move_book[move_book_index][move_index]);
            }
        }

        TEST_ASSERT_TRUE(is_solved(cube));

        free(cube);
    }
}

void setUp(void) { cubie_build_move_table(); }

void tearDown(void) {}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_half_turn_moves_have_diameter_4);
    RUN_TEST(test_double_turn_moves_have_diameter_2);
    RUN_TEST(test_half_turn_moves_inverses);
    RUN_TEST(test_move_sequences_diameter);
    RUN_TEST(test_move_sequences_diameter_2);

    return UNITY_END();
}
