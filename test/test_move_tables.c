#include <definitions.h>
#include <move_tables.h>

#include "unity.h"

void test_build_basic_move_u_corner() {
    cube_cubie *cube_1 = build_basic_move(MOVE_U1);
    cube_cubie *cube_2 = build_basic_move(MOVE_U1);
    cube_cubie *cube_3 = build_basic_move(MOVE_U1);

    for (int i = 0; i < 8; i++) {
        TEST_ASSERT_EQUAL_INT(cube_1->corner_permutations[i], cube_2->corner_permutations[i]);
        TEST_ASSERT_EQUAL_INT(cube_1->corner_permutations[i], cube_3->corner_permutations[i]);

        TEST_ASSERT_EQUAL_INT(cube_1->corner_orientations[i], cube_2->corner_orientations[i]);
        TEST_ASSERT_EQUAL_INT(cube_1->corner_orientations[i], cube_3->corner_orientations[i]);
    }
}

void test_build_basic_move_u_edges() {
    cube_cubie *cube_1 = build_basic_move(MOVE_U1);
    cube_cubie *cube_2 = build_basic_move(MOVE_U1);
    cube_cubie *cube_3 = build_basic_move(MOVE_U1);

    for (int i = 0; i < 12; i++) {
        TEST_ASSERT_EQUAL_INT(cube_1->edge_permutations[i], cube_2->edge_permutations[i]);
        TEST_ASSERT_EQUAL_INT(cube_1->edge_permutations[i], cube_3->edge_permutations[i]);

        TEST_ASSERT_EQUAL_INT(cube_1->edge_orientations[i], cube_2->edge_orientations[i]);
        TEST_ASSERT_EQUAL_INT(cube_1->edge_orientations[i], cube_3->edge_orientations[i]);
    }
}
