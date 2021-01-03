#include <pcg_variants.h>
#include <unity.h>

#include <coord_move_tables.h>
#include <solve.h>
#include <utils.h>

void test_random_solves() {
    coord_cube_t *solved = get_coord_cube();

    for (int i = 0; i < 250; i++) {
        coord_cube_t *cube = get_coord_cube();

        int n_moves = pcg32_boundedrand(3) + 2;
        for (int i = 0; i < n_moves; i++) {
            move_t move = pcg32_boundedrand(N_MOVES);
            coord_apply_move(cube, move);
        }

        // FIXME: The coordinate encoding doesnt have enough resolution yet to differentiate this
        /*TEST_ASSERT_FALSE(are_coord_equal(solved, cube));*/

        move_t *solution = solve(cube);

        for (int i = 0; solution[i] != MOVE_NULL; i++) {
            coord_apply_move(cube, solution[i]);
        }

        TEST_ASSERT_TRUE(are_phase1_coord_equal(solved, cube));

        free(solution);
        free(cube);
    }
}

void setUp() { build_move_tables(); }
void tearDown() {}

int main() {
    pcg32_srandom(43u, 55u);

    UNITY_BEGIN();

    RUN_TEST(test_random_solves);

    return UNITY_END();
}
