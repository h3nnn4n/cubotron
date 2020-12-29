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
