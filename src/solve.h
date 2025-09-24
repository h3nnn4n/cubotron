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

#include <stdint.h>

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
    int           solution_count;
    int           allowed_depth;
} solve_context_t;

typedef struct phase1_solve_s {
    uint8_t depth;
    uint8_t move_count;

    coord_cube_t *cube;

    // Why both if they are the same?
    move_t solution[MAX_MOVES];
    move_t phase1_solution[MAX_MOVES];
} phase1_solve_t;

typedef struct phase2_solve_s {
    uint8_t depth;
    uint8_t move_count;

    coord_cube_t *cube;

    move_t solution[MAX_MOVES];
    move_t phase2_solution[MAX_MOVES];
} phase2_solve_t;

solve_list_t *solve_facelets_single(char facelets[N_FACELETS]);
solve_list_t *solve_facelets(char facelets[N_FACELETS], const config_t *config);
solve_list_t *solve(const coord_cube_t *original_cube, const config_t *config);
solve_list_t *solve_single(const coord_cube_t *original_cube);
phase1_solve_t *get_phase1_solution(solve_context_t *solve_context, const config_t *config);
phase2_solve_t *solve_phase2(solve_context_t *solve_context, const config_t *config);

phase1_solve_t *make_phase1_solve(const coord_cube_t *cube);
void            destroy_phase1_solve(phase1_solve_t *phase1_solve);

phase2_solve_t *make_phase2_solve(const coord_cube_t *cube);
void            destroy_phase2_solve(phase2_solve_t *phase2_solve);

solve_list_t *new_solve_list_node();
void          destroy_solve_list_node(solve_list_t *node);

solve_context_t *make_solve_context(const coord_cube_t *cube);
void             clear_solve_context(solve_context_t *solve_context);
void             destroy_solve_context(solve_context_t *context);

#endif /* end of include guard */
