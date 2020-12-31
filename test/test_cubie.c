#include <pcg_variants.h>
#include <stdlib.h>
#include <unity.h>

#include <cubie_cube.h>
#include <definitions.h>
#include <move_tables.h>
#include <utils.h>

static pcg32_random_t rng;

void test_init_cubie_corner_permutations() {
    cube_cubie_t *cube = init_cubie_cube();

    for (int i = 0; i < N_CORNERS; i++) {
        TEST_ASSERT_EQUAL_INT(i, cube->corner_permutations[i]);
    }

    free(cube);
}

void test_init_cubie_corner_orientations() {
    cube_cubie_t *cube = init_cubie_cube();

    for (int i = 0; i < N_CORNERS; i++) {
        TEST_ASSERT_EQUAL_INT(0, cube->corner_orientations[i]);
    }

    free(cube);
}

void test_init_cubie_edge_permutations() {
    cube_cubie_t *cube = init_cubie_cube();

    for (int i = 0; i < N_EDGES; i++) {
        TEST_ASSERT_EQUAL_INT(i, cube->edge_permutations[i]);
    }

    free(cube);
}

void test_init_cubie_edge_orientations() {
    cube_cubie_t *cube = init_cubie_cube();

    for (int i = 0; i < N_EDGES; i++) {
        TEST_ASSERT_EQUAL_INT(0, cube->edge_orientations[i]);
    }

    free(cube);
}

void test_multiply_cube_cubie_edges_u2_is_not_identity() {
    cube_cubie_t *cube         = init_cubie_cube();
    cube_cubie_t *cube_u1_move = build_basic_move(MOVE_U1);

    multiply_cube_cubie_edges(cube, cube_u1_move);
    multiply_cube_cubie_edges(cube, cube_u1_move);

    int off_count = 0;
    for (int i = 0; i < N_EDGES; i++) {
        if (i != (int)cube->edge_permutations[i])
            off_count++;
    }

    TEST_ASSERT_EQUAL_INT(4, off_count);

    free(cube);
}

void test_multiply_cube_cubie_edges_u4_is_identity() {
    cube_cubie_t *cube         = init_cubie_cube();
    cube_cubie_t *cube_u1_move = build_basic_move(MOVE_U1);

    for (int i = 0; i < 4; i++) {
        multiply_cube_cubie_edges(cube, cube_u1_move);
    }

    for (int i = 0; i < N_EDGES; i++) {
        TEST_ASSERT_EQUAL_INT(i, cube->edge_permutations[i]);
        TEST_ASSERT_EQUAL_INT(0, cube->edge_orientations[i]);
    }

    free(cube);
}

void test_multiply_cube_cubie_corners_u2_is_not_identity() {
    cube_cubie_t *cube         = init_cubie_cube();
    cube_cubie_t *cube_u1_move = build_basic_move(MOVE_U1);

    multiply_cube_cubie_corners(cube, cube_u1_move);
    multiply_cube_cubie_corners(cube, cube_u1_move);

    int off_count = 0;
    for (int i = 0; i < N_CORNERS; i++) {
        if (i != (int)cube->corner_permutations[i])
            off_count++;
    }

    TEST_ASSERT_EQUAL_INT(4, off_count);

    free(cube);
}

void test_multiply_cube_cubie_corners_u4_is_identity() {
    cube_cubie_t *cube         = init_cubie_cube();
    cube_cubie_t *cube_u1_move = build_basic_move(MOVE_U1);

    for (int i = 0; i < 4; i++) {
        multiply_cube_cubie_corners(cube, cube_u1_move);
    }

    for (int i = 0; i < N_CORNERS; i++) {
        TEST_ASSERT_EQUAL_INT(i, cube->corner_permutations[i]);
        TEST_ASSERT_EQUAL_INT(0, cube->corner_orientations[i]);
    }

    free(cube);
}

void test_set_corner_orientations_only_makes_valid_cubes() {
    cube_cubie_t *cube = init_cubie_cube();

    for (int i = 0; i < N_CORNER_ORIENTATIONS; i++) {
        set_corner_orientations(cube, i);

        TEST_ASSERT_TRUE(is_valid(cube));
    }

    free(cube);
}

void test_set_corner_and_get_corner_orientations() {
    cube_cubie_t *cube = init_cubie_cube();

    for (int i = 0; i < N_CORNER_ORIENTATIONS; i++) {
        set_corner_orientations(cube, i);
        int orientation = get_corner_orientations(cube);

        TEST_ASSERT_EQUAL_INT(i, orientation);
    }

    free(cube);
}

void test_get_corner_and_set_corner_orientations() {
    cube_cubie_t *cube1 = init_cubie_cube();
    cube_cubie_t *cube2 = init_cubie_cube();

    // Take 10k cubes and suffle them with 30 moves
    for (int i = 0; i < 10000; i++) {
        for (int j = 0; j < 30; j++)
            apply_move(cube1, pcg32_boundedrand_r(&rng, 18));

        int orientation = get_corner_orientations(cube1);
        set_corner_orientations(cube2, orientation);
        int orientation2 = get_corner_orientations(cube2);

        // Assert that the corner orientations are copied over from cube1 to cube2
        TEST_ASSERT_EQUAL_INT_ARRAY(cube1->corner_orientations, cube2->corner_orientations, N_CORNERS);
        TEST_ASSERT_EQUAL_INT(orientation, orientation2);
    }

    free(cube1);
    free(cube2);
}

void test_set_edge_orientations_only_makes_valid_cubes() {
    cube_cubie_t *cube = init_cubie_cube();

    for (int i = 0; i < N_EDGE_ORIENTATIONS; i++) {
        set_edge_orientations(cube, i);

        TEST_ASSERT_TRUE(is_valid(cube));
    }

    free(cube);
}

void test_set_edge_and_get_edge_orientations() {
    cube_cubie_t *cube = init_cubie_cube();

    for (int i = 0; i < N_EDGE_ORIENTATIONS; i++) {
        set_edge_orientations(cube, i);
        int orientation = get_edge_orientations(cube);

        TEST_ASSERT_EQUAL_INT(i, orientation);
    }

    free(cube);
}

void test_get_edge_and_set_edge_orientations() {
    cube_cubie_t *cube1 = init_cubie_cube();
    cube_cubie_t *cube2 = init_cubie_cube();

    // Take 10k cubes and suffle them with 30 moves
    for (int i = 0; i < 10000; i++) {
        for (int j = 0; j < 30; j++)
            apply_move(cube1, pcg32_boundedrand_r(&rng, 18));

        int orientation = get_edge_orientations(cube1);
        set_edge_orientations(cube2, orientation);
        int orientation2 = get_edge_orientations(cube2);

        // Assert that the edge orientations are copied over from cube1 to cube2
        TEST_ASSERT_EQUAL_INT_ARRAY(cube1->edge_orientations, cube2->edge_orientations, N_EDGES);
        TEST_ASSERT_EQUAL_INT(orientation, orientation2);
    }

    free(cube1);
    free(cube2);
}

void setUp(void) { build_move_table(); }

void tearDown(void) {}

int main() {
    pcg32_srandom_r(&rng, 42u, 54u);

    UNITY_BEGIN();

    RUN_TEST(test_init_cubie_corner_permutations);
    RUN_TEST(test_init_cubie_edge_permutations);
    RUN_TEST(test_init_cubie_corner_orientations);
    RUN_TEST(test_init_cubie_edge_orientations);

    RUN_TEST(test_multiply_cube_cubie_edges_u2_is_not_identity);
    RUN_TEST(test_multiply_cube_cubie_edges_u4_is_identity);

    RUN_TEST(test_multiply_cube_cubie_corners_u2_is_not_identity);
    RUN_TEST(test_multiply_cube_cubie_corners_u4_is_identity);

    RUN_TEST(test_set_corner_orientations_only_makes_valid_cubes);
    RUN_TEST(test_set_corner_and_get_corner_orientations);
    RUN_TEST(test_get_corner_and_set_corner_orientations);

    RUN_TEST(test_set_edge_orientations_only_makes_valid_cubes);
    RUN_TEST(test_set_edge_and_get_edge_orientations);
    RUN_TEST(test_get_edge_and_set_edge_orientations);

    return UNITY_END();
}
