#include <stdlib.h>

#include "coord_cube.h"

coord_cube_t *get_coord_cube() {
    coord_cube_t *coord_cube = (coord_cube_t *)malloc(sizeof(coord_cube_t));

    coord_cube->edge_orientations   = 0;
    coord_cube->corner_orientations = 0;
    coord_cube->UD_slice            = 0;

    return coord_cube;
}

coord_cube_t *make_coord_cube(cube_cubie_t *cubie) {
    coord_cube_t *coord_cube = get_coord_cube();

    coord_cube->edge_orientations   = get_edge_orientations(cubie);
    coord_cube->corner_orientations = get_corner_orientations(cubie);
    coord_cube->UD_slice            = get_UD_slice(cubie);

    return coord_cube;
}

void copy_coord_cube(coord_cube_t *dest, coord_cube_t *source) {
    dest->edge_orientations   = source->edge_orientations;
    dest->corner_orientations = source->corner_orientations;
    dest->UD_slice            = source->UD_slice;
}

int are_coord_equal(coord_cube_t *cube1, coord_cube_t *cube2) {
    if (cube1->edge_orientations != cube2->edge_orientations)
        return 0;

    if (cube1->corner_orientations != cube2->corner_orientations)
        return 0;

    if (cube1->UD_slice != cube2->UD_slice)
        return 0;

    return 1;
}
