#include <pcg_variants.h>
#include <stdlib.h>
#include <unity.h>

#include <definitions.h>
#include <move_tables.h>
#include <utils.h>

static pcg32_random_t rng;

void test_copy_coord_cube() {
    coord_cube_t *cube  = get_coord_cube();
    coord_cube_t *other = get_coord_cube();

    for (int i = 0; i < 1000; i++) {
        move_t move = pcg32_boundedrand(N_MOVES);

        coord_apply_move(cube, move);
        /*TEST_ASSERT_FALSE(are_coord_equal(cube, other));*/

        copy_coord_cube(other, cube);
        TEST_ASSERT_TRUE(are_all_coord_equal(cube, other));
    }

    free(cube);
}

void setUp(void) { build_move_tables(); }

void tearDown(void) {}

int main() {
    pcg32_srandom_r(&rng, 42u, 54u);

    UNITY_BEGIN();

    RUN_TEST(test_copy_coord_cube);

    return UNITY_END();
}
