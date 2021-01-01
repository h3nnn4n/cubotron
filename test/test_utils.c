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

void setUp() { build_move_tables(); };
void tearDown(){};

int main() {
    pcg32_srandom(43u, 55u);

    UNITY_BEGIN();

    RUN_TEST(test_random_cubie_cube_not_solved);

    // FIXME
    /*RUN_TEST(test_random_cubie_cube);*/
    // FIXME
    /*RUN_TEST(test_random_coord_cube);*/

    return UNITY_END();
}
