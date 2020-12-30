#ifndef _CUBIE
#define _CUBIE

#include <stdlib.h>

#include "definitions.h"

// Represents the cube at the cubie level, as a series of permutations
typedef struct {
    corner corner_permutations[8];
    edge   edge_permutations[12];

    int corner_orientations[8];
    int edge_orientations[12];
} cube_cubie;

cube_cubie *init_cubie_cube();
void        multiply_cube_cubie(cube_cubie *cube1, cube_cubie *cube2);
void        multiply_cube_cubie_edges(cube_cubie *cube1, cube_cubie *cube2);
void        multiply_cube_cubie_corners(cube_cubie *cube1, cube_cubie *cube2);

#endif /* end of include guard */
