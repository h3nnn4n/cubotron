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

void test_build_cube_from_facelet_string() {
    char          facelets[N_FACELETS] = "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB";
    color_t *     color_cube           = build_facelet(facelets);
    cube_cubie_t *cubie_cube           = build_cubie_cube_from_facelet(color_cube);

    int edge_orientation = get_edge_orientations(cubie_cube);
    TEST_ASSERT_EQUAL_INT(0, edge_orientation);

    int sorted_E_slice = get_E_sorted_slice(cubie_cube);
    TEST_ASSERT_EQUAL_INT(0, sorted_E_slice);

    int UD6_edges = get_UD6_edges(cubie_cube);
    TEST_ASSERT_EQUAL_INT(0, UD6_edges);
}

void setUp(void) {}

void tearDown(void) {}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_facelets_color_count);
    RUN_TEST(test_build_cube_from_facelet_string);

    return UNITY_END();
}
