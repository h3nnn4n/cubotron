#include <pcg_variants.h>
#include <unity.h>

#include <move_tables.h>
#include <pruning.h>
#include <solve.h>
#include <utils.h>

void test_random_phase1_solving() {
    char buffer[512];

    for (int i = 0; i < 20; i++) {
        coord_cube_t *cube = get_coord_cube();

        int    n_moves   = 30;
        move_t moves[50] = {0};
        for (int i = 0; i < n_moves; i++) {
            moves[i] = pcg32_boundedrand(N_MOVES);

            do {
                moves[i] = pcg32_boundedrand(N_MOVES);
            } while (i > 0 && moves[i] == moves[i - 1]);

            coord_apply_move(cube, moves[i]);
        }

        sprintf(buffer, "%4d %4d %3d", cube->edge_orientations, cube->corner_orientations, cube->UD_slice);
        TEST_MESSAGE(buffer);

        TEST_ASSERT_FALSE(is_phase1_solved(cube));

        move_t *solution = solve_phase1(cube);

        for (int i = 0; solution[i] != MOVE_NULL; i++) {
            coord_apply_move(cube, solution[i]);
        }

        TEST_ASSERT_TRUE(is_phase1_solved(cube));

        free(solution);
        free(cube);
    }
}

void test_random_phase2_solving() {
    // FIXME: Bump this when we get the pruning tables working
    for (int i = 0; i < 20; i++) {
        coord_cube_t *cube = get_coord_cube();

        int    n_moves   = 4;
        move_t moves[50] = {0};
        for (int i = 0; i < n_moves; i++) {
            moves[i] = pcg32_boundedrand(N_MOVES);

            do {
                moves[i] = pcg32_boundedrand(N_MOVES);
            } while (i > 0 && moves[i] == moves[i - 1]);

            coord_apply_move(cube, moves[i]);
        }

        cube->corner_orientations = 0;
        cube->edge_orientations   = 0;
        cube->UD_slice            = 0;

        // FIXME: Enable once prunning is working and we can solve any cubes
        /*TEST_ASSERT_FALSE(is_phase2_solved(cube));*/

        move_t *solution = solve_phase2(cube);

        for (int i = 0; solution[i] != MOVE_NULL; i++) {
            coord_apply_move(cube, solution[i]);
        }

        TEST_ASSERT_TRUE(is_phase2_solved(cube));

        free(solution);
        free(cube);
    }
}

void test_random_full_solver() {
    char buffer[512];

    int cubes[][5] = {
        {1679, 2016, 288, 9043}, {1062, 1843, 227, 40028}, {0, 69, 106, 38653},
        {1568, 1029, 234, 5766}, {687, 2066, 292, 35841},
    };

    // FIXME: Bump this when we get the pruning tables working
    for (int i = 0; i < 5; i++) {
        coord_cube_t *cube = get_coord_cube();

        cube->edge_orientations   = cubes[i][0];
        cube->corner_orientations = cubes[i][1];
        cube->UD_slice            = cubes[i][2];
        cube->corner_permutations = cubes[i][3];

        // FIXME: Enable once prunning is working and we can solve any cubes
        /*TEST_ASSERT_FALSE(is_phase1_solved(cube));*/
        /*TEST_ASSERT_FALSE(is_phase2_solved(cube));*/

        sprintf(buffer, "%4d %4d %3d %4d", cube->edge_orientations, cube->corner_orientations, cube->UD_slice,
                cube->corner_permutations);
        TEST_MESSAGE(buffer);

        move_t *solution = solve(cube);

        for (int i = 0; solution[i] != MOVE_NULL; i++) {
            coord_apply_move(cube, solution[i]);
        }

        TEST_ASSERT_TRUE(is_phase1_solved(cube));
        TEST_ASSERT_TRUE(is_phase2_solved(cube));

        free(solution);
        free(cube);
    }
}

void setUp() {}
void tearDown() {}

int main() {
    build_move_tables();
    build_pruning_tables();

    pcg32_srandom(43u, 55u);

    UNITY_BEGIN();

    RUN_TEST(test_random_phase1_solving);
    RUN_TEST(test_random_phase2_solving);
    RUN_TEST(test_random_full_solver);

    return UNITY_END();
}
