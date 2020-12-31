#ifndef _CUBIE_CUBE
#define _CUBIE_CUBE

#include <stdlib.h>

#include "definitions.h"

// Represents the cube at the cubie level, as a series of permutations
typedef struct {
    corner_t corner_permutations[8];
    edge_t   edge_permutations[12];

    int corner_orientations[8];
    int edge_orientations[12];
} cube_cubie_t;

cube_cubie_t *init_cubie_cube();
void          multiply_cube_cubie(cube_cubie_t *cube1, cube_cubie_t *cube2);
void          multiply_cube_cubie_edges(cube_cubie_t *cube1, cube_cubie_t *cube2);
void          multiply_cube_cubie_corners(cube_cubie_t *cube1, cube_cubie_t *cube2);

void set_corner_orientations(cube_cubie_t *cube, int orientations);
int  get_corner_orientations(cube_cubie_t *cube);
void set_edge_orientations(cube_cubie_t *cube, int orientations);
int  get_edge_orientations(cube_cubie_t *cube);

#endif /* end of include guard */
