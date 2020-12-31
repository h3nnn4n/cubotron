#include <unity.h>

#include <coord_cube.h>
#include <coord_move_tables.h>
#include <cubie_move_table.h>
#include <definitions.h>
#include <utils.h>

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

void setUp(void) { build_move_tables(); }

void tearDown(void) {}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_coord_sanity_edge_orientations);
    RUN_TEST(test_coord_sanity_corner_orientations);

    return UNITY_END();
}
