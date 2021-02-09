#include <unity.h>

#include <cubie_cube.h>
#include <definitions.h>
#include <facelets.h>

void test_facelets_color_count() {
    char facelets_correct[N_FACELETS] = "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB";
    TEST_ASSERT_TRUE(verify_valid_facelets(facelets_correct));

    char facelets_wrong[N_FACELETS] = "FUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB";
    TEST_ASSERT_FALSE(verify_valid_facelets(facelets_wrong));
}

void test_build_cube_from_facelet_string_solved() {
    char          facelets[N_FACELETS] = "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB";
    cube_cubie_t *cubie_cube           = build_cubie_cube_from_str(facelets);

    int edge_orientation = get_edge_orientations(cubie_cube);
    TEST_ASSERT_EQUAL_INT(0, edge_orientation);

    int E_slice = get_E_slice(cubie_cube);
    TEST_ASSERT_EQUAL_INT(0, E_slice);

    int sorted_E_slice = get_E_sorted_slice(cubie_cube);
    TEST_ASSERT_EQUAL_INT(0, sorted_E_slice);

    int UD6_edges = get_UD6_edges(cubie_cube);
    TEST_ASSERT_EQUAL_INT(0, UD6_edges);

    int corner_orientations = get_corner_orientations(cubie_cube);
    TEST_ASSERT_EQUAL_INT(0, corner_orientations);

    int corner_permutations = get_corner_permutations(cubie_cube);
    TEST_ASSERT_EQUAL_INT(0, corner_permutations);
}

void test_build_cube_from_facelet_string_scrambled() {
    char          facelets[N_FACELETS] = "BBURUDBFUFFFRRFUUFLULUFUDLRRDBBDBDBLUDDFLLRRBRLLLBRDDF";
    cube_cubie_t *cubie_cube           = build_cubie_cube_from_str(facelets);

    int edge_orientation = get_edge_orientations(cubie_cube);
    TEST_ASSERT_EQUAL_INT(841, edge_orientation);

    int E_slice = get_E_slice(cubie_cube);
    TEST_ASSERT_EQUAL_INT(80, E_slice);

    int sorted_E_slice = get_E_sorted_slice(cubie_cube);
    TEST_ASSERT_EQUAL_INT(1932, sorted_E_slice);

    int UD6_edges = get_UD6_edges(cubie_cube);
    TEST_ASSERT_EQUAL_INT(101699, UD6_edges);

    int corner_orientations = get_corner_orientations(cubie_cube);
    TEST_ASSERT_EQUAL_INT(591, corner_orientations);

    int corner_permutations = get_corner_permutations(cubie_cube);
    TEST_ASSERT_EQUAL_INT(33724, corner_permutations);
}

void setUp(void) {}

void tearDown(void) {}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_facelets_color_count);
    RUN_TEST(test_build_cube_from_facelet_string_solved);
    RUN_TEST(test_build_cube_from_facelet_string_scrambled);

    return UNITY_END();
}
