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

#ifndef _COORD_MOVE_TABLES
#define _COORD_MOVE_TABLES

#include "coord_cube.h"
#include "cubie_cube.h"
#include "definitions.h"

void coord_build_move_tables();
void coord_apply_move(coord_cube_t *cube, move_t move);
void coord_apply_moves(coord_cube_t *cube, const move_t *moves, int n_moves);

int *get_move_table_edge_orientations();
int *get_move_table_corner_orientations();
int *get_move_table_E_slice();
int *get_move_table_E_sorted_slice();
int *get_move_table_UD6_edge_permutations();
int *get_move_table_UD7_edge_permutations();
int *get_move_table_corner_permutations();
int *get_move_table_parity();

void build_UD6_edge_permutations_move_table();
void build_UD7_edge_permutations_move_table();

#endif /* end of include guard */
