#include <stdlib.h>

#include "coord_cube.h"

coord_cube_t *get_coord_cube() {
    coord_cube_t *coord_cube = (coord_cube_t *)malloc(sizeof(coord_cube_t));

    coord_cube->edge_orientations   = 0;
    coord_cube->corner_orientations = 0;

    return coord_cube;
}

coord_cube_t *make_coord_cube(cube_cubie *cubie) {
    coord_cube_t *coord_cube = get_coord_cube();

    coord_cube->edge_orientations   = get_edge_orientations(cubie);
    coord_cube->corner_orientations = get_corner_orientations(cubie);

    return coord_cube;
}
