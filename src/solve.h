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

#ifndef _SOLVE
#define _SOLVE

#include "config.h"
#include "coord_cube.h"
#include "stats.h"

#define MAX_MOVES 30

typedef struct solve_list_s solve_list_t;

typedef struct solve_list_s {
    solve_list_t *next;

    move_t *phase1_solution;
    move_t *phase2_solution;
    move_t *solution;

    solve_stats_t *stats;
} solve_list_t;

typedef struct solve_context_s solve_context_t;

typedef struct solve_context_s {
    coord_cube_t *cube;
    move_t        move_stack[MAX_MOVES];
    coord_cube_t *cube_stack[MAX_MOVES];
    int           pruning_stack[MAX_MOVES];
    int           move_count;

    solve_context_t *phase2_context;
} solve_context_t;

solve_list_t *solve_facelets_single(char facelets[N_FACELETS]);
solve_list_t *solve_facelets(char facelets[N_FACELETS], const config_t *config);
solve_list_t *solve(const coord_cube_t *original_cube, const config_t *config);
solve_list_t *solve_single(const coord_cube_t *original_cube);
move_t       *solve_phase1(solve_context_t *solve_context, const config_t *config, solve_list_t *solves);
move_t       *solve_phase2(solve_context_t *solve_context, const config_t *config, int current_depth);

solve_list_t *new_solve_list_node();
void          destroy_solve_list_node(solve_list_t *node);

solve_context_t *make_solve_context(const coord_cube_t *cube);
void             clear_solve_context(solve_context_t *solve_context);
void             destroy_solve_context(solve_context_t *context);

// Utility functions for testing
int is_phase1_moves_solved(const move_t *solution, const coord_cube_t *original_cube);

#endif /* end of include guard */
