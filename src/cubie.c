#include <assert.h>

#include "cubie.h"
#include "definitions.h"
#include "move_tables.h"
#include "utils.h"

cube_cubie *init_cubie_cube() {
    cube_cubie *cube = (cube_cubie *)malloc(sizeof(cube_cubie));

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

void set_corner_orientations(cube_cubie *cube, int orientations) {
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

int get_corner_orientations(cube_cubie *cube) {
    int orientations = 0;

    // This intentionaly skips the last one (DRB), since it is a funcion of the other 7 corners
    for (int i = URF; i < DRB; i++)
        orientations = 3 * orientations + cube->corner_orientations[i];

    return orientations;
}

void multiply_cube_cubie(cube_cubie *cube1, cube_cubie *cube2) {
    assert(is_valid(cube1));
    assert(is_valid(cube2));

    multiply_cube_cubie_edges(cube1, cube2);
    multiply_cube_cubie_corners(cube1, cube2);

    assert(is_valid(cube1));
}

void multiply_cube_cubie_edges(cube_cubie *cube1, cube_cubie *cube2) {
    edge_t ep[N_EDGES];
    int    eo[N_EDGES];

    for (int i = 0; i < N_EDGES; i++) {
        ep[i] = cube1->edge_permutations[cube2->edge_permutations[i]];
        eo[i] = (cube2->edge_orientations[i] + cube1->edge_orientations[cube2->edge_permutations[i]]) % 2;
    }

    for (int i = 0; i < N_EDGES; i++) {
        cube1->edge_permutations[i] = ep[i];
        cube2->edge_orientations[i] = eo[i];
    }
}

void multiply_cube_cubie_corners(cube_cubie *cube1, cube_cubie *cube2) {
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
        cube2->corner_orientations[i] = co[i];
    }
}
