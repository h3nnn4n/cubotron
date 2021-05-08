/*
 * Copyright <2021> <Renan S Silva, aka h3nnn4n>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

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
