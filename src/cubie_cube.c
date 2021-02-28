#include <assert.h>

#include "coord_cube.h"
#include "cubie_cube.h"
#include "definitions.h"
#include "utils.h"

cube_cubie_t *init_cubie_cube() {
    cube_cubie_t *cube = (cube_cubie_t *)malloc(sizeof(cube_cubie_t));

    for (int i = 0; i < N_CORNERS; i++) {
        cube->corner_permutations[i] = i;
        cube->corner_orientations[i] = 0;
    }

    for (int i = 0; i < N_EDGES; i++) {
        cube->edge_permutations[i] = i;
        cube->edge_orientations[i] = 0;
    }

    return cube;
}

void set_corner_orientations(cube_cubie_t *cube, int orientations) {
    int parity = 0;

    // Start from the second to last corner
    for (int i = DRB - 1; i >= URF; i--) {
        cube->corner_orientations[i] = orientations % 3;
        parity += orientations % 3;
        orientations /= 3;
    }

    // The last corner parity is a function of the other 7
    cube->corner_orientations[DRB] = (3 - parity % 3) % 3;

    assert(is_valid(cube));
}

int get_corner_orientations(cube_cubie_t *cube) {
    int orientations = 0;

    // This intentionaly skips the last one (DRB), since it is a funcion of the other 7 corners
    for (int i = URF; i < DRB; i++)
        orientations = 3 * orientations + cube->corner_orientations[i];

    return orientations;
}

void set_edge_orientations(cube_cubie_t *cube, int orientations) {
    int parity = 0;

    // Start from the second to last edge
    for (int i = BR - 1; i >= UR; i--) {
        cube->edge_orientations[i] = orientations % 2;
        parity += orientations % 2;
        orientations /= 2;
    }

    // The last edge parity is a function of the other 11
    cube->edge_orientations[BR] = (2 - parity % 2) % 2;

    assert(is_valid(cube));
}

int get_edge_orientations(cube_cubie_t *cube) {
    int orientations = 0;

    // This intentionaly skips the last one (BR), since it is a funcion of the other 11 edges
    for (int i = UR; i < BR; i++)
        orientations = 2 * orientations + cube->edge_orientations[i];

    return orientations;
}

int get_E_slice(cube_cubie_t *cube) {
    int slice = 0;
    int x     = 1;

    for (int i = BR; i >= UR; i--) {
        int ep = cube->edge_permutations[i];

        if (ep >= FR && ep <= BR) {
            slice += Cnk(BR - i, x);
            x++;
        }
    }

    return slice;
}

void set_E_slice(cube_cubie_t *cube, int slice) {
    static int slice_edges[4] = {FR, FL, BL, BR};
    static int other_edges[8] = {UR, UF, UL, UB, DR, DF, DL, DB};

    for (int i = 0; i < N_EDGES; i++)
        cube->edge_permutations[i] = -1;

    for (int x = 4, i = 0; i < N_EDGES; i++) {
        if (slice - Cnk(BR - i, x) >= 0) {
            cube->edge_permutations[i] = slice_edges[4 - x];
            slice -= Cnk(BR - i, x);
            x--;
        }
    }

    for (int x = 0, i = 0; i < N_EDGES; i++) {
        if (cube->edge_permutations[i] == (edge_t)-1) {
            cube->edge_permutations[i] = other_edges[x];
            x++;
        }
    }
}

int get_E_sorted_slice(cube_cubie_t *cube) {
    int    combination = 0;
    int    permutation = 0;
    int    seen        = 0;
    edge_t edges[4]    = {0};

    for (int i = BR; i >= UR; i--)
        if (FR <= cube->edge_permutations[i] && cube->edge_permutations[i] <= BR) {
            combination += Cnk(11 - i, seen + 1);
            edges[3 - seen] = cube->edge_permutations[i];
            seen += 1;
        }

    for (int i = 3; i > 0; i--) {
        seen = 0;

        while ((int)edges[i] != i + 8) {
            rotate_left((int *)edges, 0, i);
            seen++;
        }

        permutation = (i + 1) * permutation + seen;
    }

    return (24 * combination + permutation);
}

void set_E_sorted_slice(cube_cubie_t *cube, int slice) {
    edge_t slice_edges[4] = {FR, FL, BL, BR};
    edge_t other_edges[8] = {UR, UF, UL, UB, DR, DF, DL, DB};

    int permutation = slice % 24;
    int combination = slice / 24;

    for (int i = 0; i < N_EDGES; i++) {
        cube->edge_permutations[i] = DB;
    }

    for (int i = 1, k; i < 4; i++) {
        k = permutation % (i + 1);
        permutation /= i + 1;

        while (k-- > 0)
            rotate_right((int *)slice_edges, 0, i);
    }

    for (int i = UR, seen = 3; i <= BR; i++) {
        if (combination - Cnk(11 - i, seen + 1) >= 0) {
            cube->edge_permutations[i] = slice_edges[3 - seen];
            combination -= Cnk(11 - i, seen + 1);
            seen -= 1;
        }
    }

    for (int i = UR, seen = 0; i <= BR; i++) {
        if (cube->edge_permutations[i] == DB) {
            cube->edge_permutations[i] = other_edges[seen];
            seen += 1;
        }
    }
}

int get_UD7_edges(cube_cubie_t *cubiecube) {
    int    combination = 0;
    int    permutation = 0;
    edge_t edges[7]    = {0};

    // This is 791, or
    // (7 choose 6) + (8 choose 6) + (9 choose 6) + (10 choose 6) + (11 choose 6)
    for (int i = UR, seen = 0; i <= BR; i++) {
        if (cubiecube->edge_permutations[i] <= DL) {
            combination += Cnk(i, seen + 1);
            edges[seen] = cubiecube->edge_permutations[i];
            seen += 1;
        }
    }

    assert(combination >= 0);
    assert(combination <= 791);

    // This is 7! = 5040
    for (int i = 6, seen = 0; i > 0; i--) {
        seen = 0;

        while ((int)edges[i] != i) {
            rotate_left((int *)edges, 0, i);
            seen++;
        }

        permutation = (i + 1) * permutation + seen;
    }

    assert(permutation >= 0);
    assert(permutation < 5040);

    int result = 5040 * combination + permutation;

    assert(result >= 0);
    assert(result < N_UD7_PHASE1_PERMUTATIONS);

    return result;
}

void set_UD7_edges(cube_cubie_t *cubiecube, int idx) {
    edge_t slice_edges[7] = {UR, UF, UL, UB, DR, DF, DL};
    edge_t other_edges[5] = {DB, FR, FL, BL, BR};
    int    permutation    = idx % 5040;
    int    combination    = idx / 5040;

    assert(combination >= 0);
    assert(combination <= 791);

    assert(permutation >= 0);
    assert(permutation < 5040);

    for (int i = 0; i < N_EDGES; i++) {
        cubiecube->edge_permutations[i] = BR;
    }

    for (int i = 1, j = 1; i < 7; i++) {
        j = permutation % (i + 1);
        permutation /= i + 1;
        while (j-- > 0) {
            rotate_right((int *)slice_edges, 0, i);
        }
    }

    for (int i = BR, seen = 6; i >= 0; i--) {
        if (combination - Cnk(i, seen + 1) >= 0) {
            assert(seen >= 0);
            assert(seen < 7);

            cubiecube->edge_permutations[i] = slice_edges[seen];
            combination -= Cnk(i, seen + 1);
            seen -= 1;
        }
    }

    for (int i = UR, seen = 0; i <= BR; i++) {
        if (cubiecube->edge_permutations[i] == BR) {
            assert(seen >= 0);
            assert(seen < 5);

            cubiecube->edge_permutations[i] = other_edges[seen];
            seen += 1;
        }
    }

    assert(is_valid(cubiecube));
}

int get_UD6_edges(cube_cubie_t *cubiecube) {
    int    combination = 0;
    int    permutation = 0;
    edge_t edges[6]    = {0};

    for (int i = UR, seen = 0; i <= BR; i++) {
        if (cubiecube->edge_permutations[i] <= DF) {
            combination += Cnk(i, seen + 1);
            edges[seen] = cubiecube->edge_permutations[i];
            seen += 1;
        }
    }

    for (int i = 5, seen = 0; i > 0; i--) {
        seen = 0;

        while ((int)edges[i] != i) {
            rotate_left((int *)edges, 0, i);
            seen++;
        }

        permutation = (i + 1) * permutation + seen;
    }

    return 720 * combination + permutation;
}

void set_UD6_edges(cube_cubie_t *cubiecube, int idx) {
    edge_t slice_edges[6] = {UR, UF, UL, UB, DR, DF};
    edge_t other_edges[6] = {DL, DB, FR, FL, BL, BR};
    int    permutation    = idx % 720;
    int    combination    = idx / 720;

    for (int i = 0; i < N_EDGES; i++) {
        cubiecube->edge_permutations[i] = BR;
    }

    for (int i = 1, j = 0; i < 6; i++) {
        j = permutation % (i + 1);
        permutation /= i + 1;
        while (j-- > 0) {
            rotate_right((int *)slice_edges, 0, i);
        }
    }

    for (int i = BR, seen = 5; i >= 0; i--) {
        if (combination - Cnk(i, seen + 1) >= 0) {
            cubiecube->edge_permutations[i] = slice_edges[seen];
            combination -= Cnk(i, seen + 1);
            seen -= 1;
        }
    }

    for (int i = UR, seen = 0; i <= BR; i++) {
        if (cubiecube->edge_permutations[i] == BR) {
            cubiecube->edge_permutations[i] = other_edges[seen];
            seen += 1;
        }
    }
}

int get_corner_parity(cube_cubie_t *cube) {
    int parity = 0;

    for (int i = DRB; i >= URF + 1; i--) {
        for (int j = i - 1; j >= URF; j--) {
            if (cube->corner_permutations[j] > cube->corner_permutations[i]) {
                parity++;
            }
        }
    }

    return parity % 2;
}

int get_edge_parity(cube_cubie_t *cube) {
    int parity = 0;

    for (int i = BR; i >= UR + 1; i--) {
        for (int j = i - 1; j >= UR; j--) {
            if (cube->edge_permutations[j] > cube->edge_permutations[i]) {
                parity++;
            }
        }
    }

    return parity % 2;
}

int get_corner_permutations(cube_cubie_t *cube) {
    corner_t perm[N_CORNERS] = {0};
    int      permutations    = 0;

    for (int i = 0; i < N_CORNERS; i++)
        perm[i] = cube->corner_permutations[i];

    for (int i = 7; i > 0; i--) {
        int k = 0;

        while (perm[i] != (corner_t)i) {
            rotate_left((int *)perm, 0, i);
            k++;
        }

        permutations = (i + 1) * permutations + k;
    }

    return permutations;
}

void set_corner_permutations(cube_cubie_t *cube, int permutations) {
    corner_t perm[N_CORNERS] = {URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB};

    int k;

    for (int i = 1; i < N_CORNERS; i++) {
        k = permutations % (i + 1);
        permutations /= i + 1;
        while (k > 0) {
            rotate_right((int *)perm, 0, i);
            k--;
        }
    }

    for (int i = 7; i >= 0; i--) {
        cube->corner_permutations[i] = perm[i];
    }
}

void multiply_cube_cubie(cube_cubie_t *cube1, cube_cubie_t *cube2) {
    assert(is_valid(cube1));
    assert(is_valid(cube2));

    multiply_cube_cubie_edges(cube1, cube2);
    multiply_cube_cubie_corners(cube1, cube2);

    assert(is_valid(cube1));
    assert(is_valid(cube2));
}

void multiply_cube_cubie_edges(cube_cubie_t *cube1, cube_cubie_t *cube2) {
    edge_t ep[N_EDGES];
    int    eo[N_EDGES];

    for (int i = 0; i < N_EDGES; i++) {
        ep[i] = cube1->edge_permutations[cube2->edge_permutations[i]];
        eo[i] = (cube2->edge_orientations[i] + cube1->edge_orientations[cube2->edge_permutations[i]]) % 2;
    }

    for (int i = 0; i < N_EDGES; i++) {
        cube1->edge_permutations[i] = ep[i];
        cube1->edge_orientations[i] = eo[i];
    }
}

void multiply_cube_cubie_corners(cube_cubie_t *cube1, cube_cubie_t *cube2) {
    corner_t cp[N_CORNERS];
    int      co[N_CORNERS];

    for (int i = 0; i < N_CORNERS; i++) {
        cp[i] = cube1->corner_permutations[cube2->corner_permutations[i]];

        int orientation;
        int orientation_a = cube1->corner_orientations[cube2->corner_permutations[i]];
        int orientation_b = cube2->corner_orientations[i];

        if (orientation_a < 3 && orientation_b < 3) {
            orientation = orientation_a + orientation_b;

            if (orientation >= 3)
                orientation -= 3;
        } else if (orientation_a < 3 && 3 <= orientation_b) {
            orientation = orientation_a + orientation_b;

            if (orientation >= 6)
                orientation -= 3;
        } else if (orientation_a >= 3 && 3 > orientation_b) {
            orientation = orientation_a - orientation_b;

            if (orientation < 3)
                orientation += 3;
        } else if (orientation_a >= 3 && 3 >= orientation_b) {
            orientation = orientation_a - orientation_b;

            if (orientation < 0)
                orientation += 3;
        } else {
            // This should never happen
            abort();
        }

        co[i] = orientation;
    }

    for (int i = 0; i < N_CORNERS; i++) {
        cube1->corner_permutations[i] = cp[i];
        cube1->corner_orientations[i] = co[i];
    }
}

int are_cubie_equal(cube_cubie_t *cube1, cube_cubie_t *cube2) {
    for (int i = 0; i < N_EDGES; i++) {
        if (cube1->edge_permutations[i] != cube2->edge_permutations[i])
            return 0;
        if (cube1->edge_orientations[i] != cube2->edge_orientations[i])
            return 0;
    }

    for (int i = 0; i < N_CORNERS; i++) {
        if (cube1->corner_permutations[i] != cube2->corner_permutations[i])
            return 0;
        if (cube1->corner_orientations[i] != cube2->corner_orientations[i])
            return 0;
    }

    return 1;
}
