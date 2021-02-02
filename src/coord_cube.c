#include <stdlib.h>

#include "coord_cube.h"

coord_cube_t *get_coord_cube() {
    coord_cube_t *coord_cube = (coord_cube_t *)malloc(sizeof(coord_cube_t));
    reset_coord_cube(coord_cube);
    return coord_cube;
}

void reset_coord_cube(coord_cube_t *cube) {
    cube->edge_orientations   = 0;
    cube->corner_orientations = 0;
    cube->E_slice             = 0;
    cube->E_sorted_slice      = 0;
    cube->parity              = 0;
    cube->corner_permutations = 0;
}

coord_cube_t *make_coord_cube(cube_cubie_t *cubie) {
    coord_cube_t *coord_cube = get_coord_cube();

    coord_cube->edge_orientations   = get_edge_orientations(cubie);
    coord_cube->corner_orientations = get_corner_orientations(cubie);
    coord_cube->E_slice             = get_E_slice(cubie);
    coord_cube->E_sorted_slice      = get_E_sorted_slice(cubie);
    coord_cube->parity              = get_edge_parity(cubie);
    coord_cube->corner_permutations = get_corner_permutations(cubie);

    return coord_cube;
}

void copy_coord_cube(coord_cube_t *dest, coord_cube_t *source) {
    dest->edge_orientations   = source->edge_orientations;
    dest->corner_orientations = source->corner_orientations;
    dest->E_slice             = source->E_slice;
    dest->E_sorted_slice      = source->E_sorted_slice;
    dest->parity              = source->parity;
    dest->corner_permutations = source->corner_permutations;
}

int are_all_coord_equal(coord_cube_t *cube1, coord_cube_t *cube2) {
    if (cube1->edge_orientations != cube2->edge_orientations)
        return 0;

    if (cube1->corner_orientations != cube2->corner_orientations)
        return 0;

    if (cube1->E_slice != cube2->E_slice)
        return 0;

    if (cube1->E_sorted_slice != cube2->E_sorted_slice)
        return 0;

    if (cube1->parity != cube2->parity)
        return 0;

    if (cube1->corner_permutations != cube2->corner_permutations)
        return 0;

    return 1;
}

int are_phase1_coord_equal(coord_cube_t *cube1, coord_cube_t *cube2) {
    if (cube1->edge_orientations != cube2->edge_orientations)
        return 0;

    if (cube1->corner_orientations != cube2->corner_orientations)
        return 0;

    if (cube1->E_slice != cube2->E_slice)
        return 0;

    return 1;
}
