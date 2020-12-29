#include "cubie.h"

#include "definitions.h"
#include "move_tables.h"

cube_cubie *init_cubie_cube() {
    cube_cubie *cube = (cube_cubie *)malloc(sizeof(cube_cubie));

    for (int i = 0; i < 8; i++) {
        cube->corner_permutations[i] = i;
        cube->corner_orientations[i] = 0;
    }

    for (int i = 0; i < 12; i++) {
        cube->edge_permutations[i] = i;
        cube->edge_orientations[i] = 0;
    }

    return cube;
}

void multiply_cube_cubie(cube_cubie *cube1, cube_cubie *cube2) {
    multiply_cube_cubie_edges(cube1, cube2);
    multiply_cube_cubie_corners(cube1, cube2);
}

void multiply_cube_cubie_edges(cube_cubie *cube1, cube_cubie *cube2) {
    edge ep[12];
    int  eo[12];

    for (int i = 0; i < 12; i++) {
        ep[i] = cube1->edge_permutations[cube2->edge_permutations[i]];
        eo[i] = (cube2->edge_orientations[i] + cube1->edge_orientations[cube2->edge_permutations[i]]) % 2;
    }

    for (int i = 0; i < 12; i++) {
        cube1->edge_permutations[i] = ep[i];
        cube2->edge_orientations[i] = eo[i];
    }
}

void multiply_cube_cubie_corners(cube_cubie *cube1, cube_cubie *cube2) {
    corner cp[8];
    int    co[8];

    for (int i = 0; i < 8; i++) {
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

    for (int i = 0; i < 8; i++) {
        cube1->corner_permutations[i] = cp[i];
        cube2->corner_orientations[i] = co[i];
    }
}
