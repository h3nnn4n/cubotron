#include "cubie.h"

int cubie_off_count(cube_cubie *cube) {
    int count = 0;

    for (int i = 0; i < 8; i++) {
        if (i != (int)cube->corner_permutations[i])
            count += 1;

        if (0 != cube->corner_orientations[i])
            count += 1;
    }

    for (int i = 0; i < 12; i++) {
        if (i != (int)cube->edge_permutations[i])
            count += 1;

        if (0 != cube->edge_orientations[i])
            count += 1;
    }

    return count;
}

int is_solved(cube_cubie *cube) { return cubie_off_count(cube) == 0; }
