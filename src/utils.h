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

#ifndef _UTILS
#define _UTILS

#include <stdint.h>

#include "coord_cube.h"
#include "cubie_cube.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

int is_bad_move(move_t move1, move_t move2);

int   is_cubie_solved(const cube_cubie_t *cube);
int   cubie_off_count(const cube_cubie_t *cube);
char *move_to_str_enum(move_t move);
char *move_to_str(move_t move);
int   is_valid(cube_cubie_t *cube);

cube_cubie_t *random_cubie_cube();
coord_cube_t *random_coord_cube();

move_t get_reverse_move(move_t move);

uint64_t get_microseconds(void);

int  Cnk(int n, int k);
void rotate_left(int *pieces, int l, int r);
void rotate_right(int *pieces, int l, int r);

move_t str_to_move(const char *);

int rmrf(char *path);

#endif /* end of include guard */
