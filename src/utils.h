#ifndef _UTILS
#define _UTILS

#include "coord_cube.h"
#include "cubie_cube.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

int is_bad_move(move_t move1, move_t move2);

int   is_cubie_solved(cube_cubie_t *cube);
int   cubie_off_count(cube_cubie_t *cube);
char *move_to_str_enum(move_t move);
char *move_to_str(move_t move);
int   is_valid(cube_cubie_t *cube);

cube_cubie_t *random_cubie_cube();
coord_cube_t *random_coord_cube();

move_t get_reverse_move(move_t move);

long get_microseconds(void);

int  Cnk(int n, int k);
void rotate_left(int *pieces, int l, int r);
void rotate_right(int *pieces, int l, int r);

int rmrf(char *path);

#endif /* end of include guard */
