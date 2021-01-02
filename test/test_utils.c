#include <pcg_variants.h>
#include <unity.h>

#include <coord_move_tables.h>
#include <utils.h>

void test_random_cubie_cube_not_solved() {
    cube_cubie_t *cube = random_cubie_cube();

    TEST_ASSERT_FALSE(is_solved(cube));
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

    return UNITY_END();
}
