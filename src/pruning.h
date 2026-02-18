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

#ifndef _PRINING
#define _PRINING

#include "coord_cube.h"

void build_pruning_tables();
void build_phase1_sym_table(void);
void build_phase2_sym_table(void);
void build_phase1_corner_table();
void build_phase1_edge_table();
void build_phase1_combined_table();
void build_phase2_UD6_edge_table();
void build_phase2_UD7_edge_table();
void build_phase2_corner_table();
int  get_phase1_pruning(const coord_cube_t *cube);
int  get_phase2_pruning(const coord_cube_t *cube);

#endif /* end of include guard */
