#ifndef _COORD_CUBE
#define _COORD_CUBE

#include "cubie_cube.h"

typedef struct {
    int edge_orientations;
    int corner_orientations;
} coord_cube_t;

coord_cube_t *get_coord_cube();
coord_cube_t *make_coord_cube(cube_cubie_t *);
void          copy_coord_cube(coord_cube_t *dest, coord_cube_t *source);

#endif /* end of include guard */
