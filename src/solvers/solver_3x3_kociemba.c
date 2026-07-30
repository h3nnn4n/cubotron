/*
 * Copyright <2026> <Renan S Silva, aka h3nnn4n>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
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

#include "solver_3x3_kociemba.h"
#include "coord_cube.h"
#include "cubie_cube.h"
#include "cubie_move_table.h"
#include "move_tables.h"
#include "pruning.h"
#include "solve.h"

static void solver_3x3_init(void) {
    build_move_tables();
    build_pruning_tables();
}

static solve_list_t *solver_3x3_solve(const puzzle_t *puzzle, const config_t *config) {
    cube_cubie_t *cubie    = (cube_cubie_t *)puzzle->state;
    coord_cube_t *coord    = make_coord_cube(cubie);
    solve_list_t *solution = solve(coord, config);

    free(coord);

    return solution;
}

static void solver_3x3_cleanup(void) { purge_cubie_move_table(); }

const solver_ops_t solver_3x3_kociemba_ops = {
    .name        = "kociemba",
    .puzzle_name = "3x3",

    .init    = solver_3x3_init,
    .solve   = solver_3x3_solve,
    .cleanup = solver_3x3_cleanup,
};
