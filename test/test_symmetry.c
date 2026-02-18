#include <string.h>
#include <unity.h>

#include <coord_move_tables.h>
#include <cubie_cube.h>
#include <definitions.h>
#include <move_tables.h>
#include <symmetry.h>
#include <utils.h>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static cube_cubie_t make_identity(void) {
    cube_cubie_t c;

    for (int i = 0; i < N_CORNERS; i++) {
        c.corner_permutations[i] = (corner_t)i;
        c.corner_orientations[i] = 0;
    }

    for (int i = 0; i < N_EDGES; i++) {
        c.edge_permutations[i] = (edge_t)i;
        c.edge_orientations[i] = 0;
    }

    return c;
}

// ---------------------------------------------------------------------------
// Symmetry cube tests
// ---------------------------------------------------------------------------

// sym[0] should be the identity
void test_sym0_is_identity(void) {
    cube_cubie_t id  = make_identity();
    const cube_cubie_t *syms = get_sym_cubes();
    TEST_ASSERT_TRUE(are_cubie_equal(&syms[0], &id));
}

// sym[s] * sym[inv[s]] = identity for every s
void test_sym_inv_is_inverse(void) {
    const cube_cubie_t *syms = get_sym_cubes();
    const uint8_t      *inv  = get_sym_inv();
    cube_cubie_t        id   = make_identity();

    for (int s = 0; s < N_SYMMETRIES; s++) {
        cube_cubie_t tmp = syms[s];
        multiply_cube_cubie(&tmp, (cube_cubie_t *)&syms[inv[s]]);
        TEST_ASSERT_TRUE(are_cubie_equal(&tmp, &id));
    }
}

// The D4H generators cycle back to identity after their known orders.
// sym[2] = GEN_ROT_U4 (order 4), sym[8] = GEN_ROT_F2 (order 2), sym[1] = GEN_MIRR_LR2 (order 2).
// sym[16] = GEN_ROT_URF3 (order 3, first index with urf3=1).
void test_generator_orders(void) {
    const cube_cubie_t *syms = get_sym_cubes();
    cube_cubie_t        id   = make_identity();
    cube_cubie_t        acc;

    acc = id;
    for (int i = 0; i < 4; i++)
        multiply_cube_cubie(&acc, (cube_cubie_t *)&syms[2]);
    
    TEST_ASSERT_TRUE(are_cubie_equal(&acc, &id));

    acc = id;
    for (int i = 0; i < 2; i++)
        multiply_cube_cubie(&acc, (cube_cubie_t *)&syms[8]);

    TEST_ASSERT_TRUE(are_cubie_equal(&acc, &id));

    acc = id;
    for (int i = 0; i < 2; i++)
        multiply_cube_cubie(&acc, (cube_cubie_t *)&syms[1]);

    TEST_ASSERT_TRUE(are_cubie_equal(&acc, &id));

    acc = id;
    for (int i = 0; i < 3; i++)
        multiply_cube_cubie(&acc, (cube_cubie_t *)&syms[16]);

    TEST_ASSERT_TRUE(are_cubie_equal(&acc, &id));
}

// ---------------------------------------------------------------------------
// conj_move tests
// ---------------------------------------------------------------------------

// Conjugation by identity leaves every move unchanged
void test_conj_move_identity(void) {
    const uint8_t *cm = get_conj_move();

    for (int m = 0; m < N_MOVES; m++) {
        TEST_ASSERT_EQUAL_INT(m, cm[N_MOVES * 0 + m]);
    }
}

// conj_move values are all valid move indices
void test_conj_move_range(void) {
    const uint8_t *cm = get_conj_move();

    for (int s = 0; s < N_SYMMETRIES; s++) {
        for (int m = 0; m < N_MOVES; m++) {
            TEST_ASSERT_TRUE(cm[N_MOVES * s + m] < N_MOVES);
        }
    }
}

// ---------------------------------------------------------------------------
// Flipslice orbit tests
// ---------------------------------------------------------------------------

// Exactly N_FLIPSLICE_CLASSES distinct classes were produced
void test_flipslice_class_count(void) {
    const uint16_t *classidx = get_flipslice_classidx();
    uint8_t         seen[N_FLIPSLICE_CLASSES];
    memset(seen, 0, sizeof(seen));
    int n_flipslice = N_EDGE_ORIENTATIONS * N_SLICES;

    for (int i = 0; i < n_flipslice; i++) {
        TEST_ASSERT_TRUE(classidx[i] < N_FLIPSLICE_CLASSES);
        seen[classidx[i]] = 1;
    }

    int count = 0;
    for (int i = 0; i < N_FLIPSLICE_CLASSES; i++)
        if (seen[i]) count++;

    TEST_ASSERT_EQUAL_INT(N_FLIPSLICE_CLASSES, count);
}

// The representative of each class maps back to itself with sym=0
void test_flipslice_rep_is_class0(void) {
    const uint16_t *classidx = get_flipslice_classidx();
    const uint8_t  *fsym     = get_flipslice_sym();
    const uint32_t *rep      = get_flipslice_rep();

    for (int c = 0; c < N_FLIPSLICE_CLASSES; c++) {
        uint32_t r = rep[c];
        TEST_ASSERT_EQUAL_INT(c, classidx[r]);
        TEST_ASSERT_EQUAL_INT(0, fsym[r]);
    }
}

// ---------------------------------------------------------------------------
// Corner orbit tests
// ---------------------------------------------------------------------------

void test_corner_rep_is_class0(void) {
    const uint16_t *classidx = get_corner_classidx();
    const uint8_t  *csym     = get_corner_sym();
    const uint16_t *rep      = get_corner_rep();

    for (int c = 0; c < N_CORNER_CLASSES; c++) {
        uint16_t r = rep[c];
        TEST_ASSERT_EQUAL_INT(c, classidx[r]);
        TEST_ASSERT_EQUAL_INT(0, csym[r]);
    }
}

void test_corner_class_count(void) {
    const uint16_t *classidx = get_corner_classidx();
    uint8_t         seen[N_CORNER_CLASSES];
    memset(seen, 0, sizeof(seen));

    for (int cp = 0; cp < N_CORNER_PERMUTATIONS; cp++) {
        TEST_ASSERT_TRUE(classidx[cp] < N_CORNER_CLASSES);
        seen[classidx[cp]] = 1;
    }

    int count = 0;
    for (int i = 0; i < N_CORNER_CLASSES; i++)
        if (seen[i]) count++;
    TEST_ASSERT_EQUAL_INT(N_CORNER_CLASSES, count);
}

// ---------------------------------------------------------------------------
// twist_conj and ud_edges_conj range tests
// ---------------------------------------------------------------------------

void test_twist_conj_range(void) {
    const uint16_t *tc = get_twist_conj();

    for (int t = 0; t < N_CORNER_ORIENTATIONS; t++) {
        for (int s = 0; s < N_SYMMETRIES_D4H; s++) {
            TEST_ASSERT_TRUE(tc[t * N_SYMMETRIES_D4H + s] < N_CORNER_ORIENTATIONS);
        }
    }
}

void test_ud_edges_conj_range(void) {
    const uint16_t *uc = get_ud_edges_conj();

    for (int ud = 0; ud < N_UD7_PHASE2_PERMUTATIONS; ud++) {
        for (int s = 0; s < N_SYMMETRIES_D4H; s++) {
            TEST_ASSERT_TRUE(uc[ud * N_SYMMETRIES_D4H + s] < N_UD7_PHASE2_PERMUTATIONS);
        }
    }
}

void setUp(void) {
    build_move_tables();
    build_symmetry_tables();
}

void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_sym0_is_identity);
    RUN_TEST(test_sym_inv_is_inverse);
    RUN_TEST(test_generator_orders);
    RUN_TEST(test_conj_move_identity);
    RUN_TEST(test_conj_move_range);
    RUN_TEST(test_flipslice_class_count);
    RUN_TEST(test_flipslice_rep_is_class0);
    RUN_TEST(test_corner_class_count);
    RUN_TEST(test_corner_rep_is_class0);
    RUN_TEST(test_twist_conj_range);
    RUN_TEST(test_ud_edges_conj_range);

    return UNITY_END();
}
