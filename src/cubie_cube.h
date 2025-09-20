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

#ifndef _CUBIE_CUBE
#define _CUBIE_CUBE

#include <stdlib.h>

#include "cubie_cube.h"
#include "definitions.h"

// Represents the cube at the cubie level, as a series of permutations and orientations
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
int  get_E_slice(cube_cubie_t *cube);
void set_E_slice(cube_cubie_t *cube, int slice);
int  get_E_sorted_slice(cube_cubie_t *cube);
void set_E_sorted_slice(cube_cubie_t *cube, int slice);
int  get_UD7_edges(cube_cubie_t *cube);
void set_UD7_edges(cube_cubie_t *cube, int slice);
int  get_UD6_edges(cube_cubie_t *cube);
void set_UD6_edges(cube_cubie_t *cube, int slice);
int  get_corner_permutations(cube_cubie_t *cube);
void set_corner_permutations(cube_cubie_t *cube, int permutations);

int get_corner_parity(const cube_cubie_t *cube);
int get_edge_parity(const cube_cubie_t *cube);

int are_cubie_equal(const cube_cubie_t *cube1, const cube_cubie_t *cube2);

#endif /* end of include guard */
