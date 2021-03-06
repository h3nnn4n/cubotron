#ifndef _COORD_CUBE
#define _COORD_CUBE

#include "cubie_cube.h"

typedef struct {
    int edge_orientations;
    int corner_orientations;
    int E_slice;
    int E_sorted_slice;
    int parity;
    int UD6_edge_permutations;
    int UD7_edge_permutations;
    int corner_permutations;
} coord_cube_t;

coord_cube_t *get_coord_cube();
void          reset_coord_cube(coord_cube_t *cube);
coord_cube_t *make_coord_cube(cube_cubie_t *);
void          copy_coord_cube(coord_cube_t *dest, coord_cube_t *source);
int           are_all_coord_equal(coord_cube_t *cube1, coord_cube_t *cube2);
int           are_phase1_coord_equal(coord_cube_t *cube1, coord_cube_t *cube2);
int           is_phase1_solved(coord_cube_t *cube);
int           is_phase2_solved(coord_cube_t *cube);
int           is_coord_solved(coord_cube_t *cube);

#endif /* end of include guard */
