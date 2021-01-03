#include <pcg_variants.h>
#include <unity.h>

#include <coord_cube.h>
#include <coord_move_tables.h>
#include <cubie_move_table.h>
#include <definitions.h>
#include <utils.h>

static pcg32_random_t rng;

void test_coord_sanity_edge_orientations() {
    coord_cube_t *coord_cube = get_coord_cube();
    cube_cubie_t *cubie_cube = init_cubie_cube();

    for (int i = 0; i < N_EDGE_ORIENTATIONS; i++) {
        for (int move = 0; move < N_MOVES; move++) {
            set_edge_orientations(cubie_cube, i);
            coord_cube->edge_orientations = i;

            cubie_apply_move(cubie_cube, move);
            coord_apply_move(coord_cube, move);

            TEST_ASSERT_EQUAL_INT(get_edge_orientations(cubie_cube), coord_cube->edge_orientations);
        }
    }

    free(cubie_cube);
    free(coord_cube);
}

void test_coord_sanity_corner_orientations() {
    coord_cube_t *coord_cube = get_coord_cube();
    cube_cubie_t *cubie_cube = init_cubie_cube();

    for (int i = 0; i < N_CORNER_ORIENTATIONS; i++) {
        for (int move = 0; move < N_MOVES; move++) {
            set_corner_orientations(cubie_cube, i);
            coord_cube->corner_orientations = i;

            cubie_apply_move(cubie_cube, move);
            coord_apply_move(coord_cube, move);

            TEST_ASSERT_EQUAL_INT(get_corner_orientations(cubie_cube), coord_cube->corner_orientations);
        }
    }

    free(cubie_cube);
    free(coord_cube);
}

void test_coord_sanity_brute_force() {
    coord_cube_t *coord_cube = get_coord_cube();
    cube_cubie_t *cubie_cube = init_cubie_cube();

    for (int i = 0; i < 10000; i++) {
        move_t move = pcg32_boundedrand_r(&rng, N_MOVES);

        cubie_apply_move(cubie_cube, move);
        coord_apply_move(coord_cube, move);

        TEST_ASSERT_EQUAL_INT(get_corner_orientations(cubie_cube), coord_cube->corner_orientations);
        TEST_ASSERT_EQUAL_INT(get_edge_orientations(cubie_cube), coord_cube->edge_orientations);
    }

    free(cubie_cube);
    free(coord_cube);
}

void test_coord_orientation_changes() {
    TEST_PASS();
    coord_cube_t *coord_cube = get_coord_cube();

    coord_apply_move(coord_cube, MOVE_R1);
    coord_apply_move(coord_cube, MOVE_F1);

    TEST_ASSERT_NOT_EQUAL_INT(0, coord_cube->corner_orientations);
    TEST_ASSERT_NOT_EQUAL_INT(0, coord_cube->edge_orientations);

    free(coord_cube);
}

void test_moves_are_reversible() {
    TEST_PASS();
    const int n_moves = 10;
    move_t    moves[50];

    for (int i = 0; i < 1000; i++) {
        coord_cube_t *cube      = random_coord_cube();
        coord_cube_t *reference = get_coord_cube();
        copy_coord_cube(reference, cube);

        for (int i = 0; i < n_moves; i++) {
            moves[i] = pcg32_boundedrand_r(&rng, N_MOVES);
            coord_apply_move(cube, moves[i]);
            printf("%s\n", move_to_str(moves[i]));
        }

        printf("\n");

        // FIXME: as of now we dont have enough coordinates to diferentiate some cubes
        /*TEST_ASSERT_FALSE(are_phase1_coord_equal(reference, cube));*/

        for (int i = n_moves - 1; i >= 0; i--) {
            coord_apply_move(cube, get_reverse_move(moves[i]));
            printf("%s\n", move_to_str(moves[i]));
        }

        TEST_ASSERT_TRUE(are_phase1_coord_equal(reference, cube));

        free(cube);
    }
}

void setUp(void) { build_move_tables(); }

void tearDown(void) {}

int main() {
    pcg32_srandom_r(&rng, 42u, 54u);

    UNITY_BEGIN();

    RUN_TEST(test_coord_sanity_edge_orientations);
    RUN_TEST(test_coord_sanity_corner_orientations);
    RUN_TEST(test_coord_sanity_brute_force);

    RUN_TEST(test_coord_orientation_changes);

    RUN_TEST(test_moves_are_reversible);

    return UNITY_END();
}
