#ifndef _COORD_CUBE
#define _COORD_CUBE

#include "cubie_cube.h"

typedef struct {
    int edge_orientations;
    int corner_orientations;
    int UD_slice;
    int corner_permutations;
} coord_cube_t;

coord_cube_t *get_coord_cube();
coord_cube_t *make_coord_cube(cube_cubie_t *);
void          copy_coord_cube(coord_cube_t *dest, coord_cube_t *source);
int           are_all_coord_equal(coord_cube_t *cube1, coord_cube_t *cube2);
int           are_phase1_coord_equal(coord_cube_t *cube1, coord_cube_t *cube2);

#endif /* end of include guard */
