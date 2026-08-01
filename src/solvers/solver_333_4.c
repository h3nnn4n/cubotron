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

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "coord_cube.h"
#include "coord_move_tables.h"
#include "cubie_cube.h"
#include "cubie_move_table.h"
#include "definitions.h"
#include "move_tables.h"
#include "pruning.h"
#include "puzzle_333_4.h"
#include "solution.h"
#include "solver_333_4.h"
#include "stats.h"
#include "utils.h"

#define MAX_MOVES 50

typedef struct solver_ctx_s {
    const coord_cube_t *original_cube;

    coord_cube_t *cube;
    move_t        move_stack[MAX_MOVES];
    coord_cube_t *cube_stack[MAX_MOVES];
    int           pruning_stack[MAX_MOVES];
    int           move_count;
    move_t        prep_moves[MAX_MOVES];
    uint8_t       prep_move_count;

    // TODO: extras coords and extras coord stack

    struct solver_ctx_s *phase2_context;
} solver_ctx_t;

typedef struct {
    solver_ctx_t  *solve_context;
    solve_list_t  *solves;
    solve_stats_t *stats;
} thread_ctx_t;

// ---- search (copied from solve.c, made static) ----

__attribute__((unused)) static solve_list_t *make_trivial_solution(void) {
    // TODO: build empty solution for an already-solved cube
    return NULL;
}

__attribute__((unused)) static solve_list_t *collect_results(thread_ctx_t *thread_contexts, int thread_count,
                                                             solve_stats_t **all_thread_stats, int *is_winner,
                                                             int *all_lengths, int *n_lengths) {
    // TODO: merge per-thread solution lists, pick winners
    (void)thread_contexts;
    (void)thread_count;
    (void)all_thread_stats;
    (void)is_winner;
    (void)all_lengths;
    (void)n_lengths;
    return NULL;
}

__attribute__((unused)) static solve_list_t *solve(const coord_cube_t *original_cube, const config_t *config) {
    // TODO: threaded two-phase IDA* over base coords + extras coords
    (void)original_cube;
    (void)config;
    return NULL;
}

__attribute__((unused)) static solve_list_t *solve_thread(void *arg) {
    // TODO: run solve_phase1 on one thread context, patch + verify solutions
    (void)arg;
    return NULL;
}

__attribute__((unused)) static void prep_phase1(solver_ctx_t *solve_context, uint16_t move_count,
                                                const move_t *move_list) {
    // TODO: apply prep moves through full move (base + extras)
    (void)solve_context;
    (void)move_count;
    (void)move_list;
}

__attribute__((unused)) static move_t *patch_solution(solver_ctx_t *solve_context, solve_list_t *solution) {
    // TODO: prepend prep moves to the solution
    (void)solve_context;
    (void)solution;
    return NULL;
}

__attribute__((unused)) static void build_phase1_solution(const move_t *move_stack, int pivot, move_t **solution,
                                                          move_t **phase1_solution) {
    // TODO: copy the phase1 move sequence into the result buffers
    (void)move_stack;
    (void)pivot;
    (void)solution;
    (void)phase1_solution;
}

__attribute__((unused)) static int assemble_full_solution(move_t *solution, int pivot, const move_t *phase2_solution,
                                                          coord_cube_t *phase2_cube) {
    // TODO: append phase2 moves, tracking extras coords on phase2_cube
    (void)solution;
    (void)pivot;
    (void)phase2_solution;
    (void)phase2_cube;
    return 0;
}

__attribute__((unused)) static void record_solution_stats(solve_stats_t *stats, int pivot, int phase2_move_count,
                                                          uint64_t move_count, uint8_t prep_move_count) {
    // TODO: record depths, lengths, move counts for the first solution
    (void)stats;
    (void)pivot;
    (void)phase2_move_count;
    (void)move_count;
    (void)prep_move_count;
}

__attribute__((unused)) static void store_solution_in_list(solve_list_t **solves_ptr, move_t *phase1_solution,
                                                           move_t *phase2_solution, move_t *solution) {
    // TODO: append the solution node to the solve list
    (void)solves_ptr;
    (void)phase1_solution;
    (void)phase2_solution;
    (void)solution;
}

__attribute__((unused)) static move_t *solve_phase1(solver_ctx_t *solve_context, solve_list_t *solves,
                                                    solve_stats_t *stats) {
    // TODO: iterative-deepening DFS; apply base move + extras move;
    //       pruning = max(get_phase1_pruning, extras_pruning);
    //       solved = is_phase1_solved && extras_is_solved
    (void)solve_context;
    (void)solves;
    (void)stats;
    return NULL;
}

__attribute__((unused)) static move_t *solve_phase2(solver_ctx_t *solve_context, const config_t *config, int max_depth,
                                                    solve_stats_t *stats) {
    // TODO: same pattern with phase2 base coords + extras coords
    (void)solve_context;
    (void)config;
    (void)max_depth;
    (void)stats;
    return NULL;
}

__attribute__((unused)) static move_t *build_phase2_solution(const move_t *moves, const move_t *move_stack, int pivot) {
    // TODO: copy the phase2 move sequence into the result buffer
    (void)moves;
    (void)move_stack;
    (void)pivot;
    return NULL;
}

__attribute__((unused)) static solver_ctx_t *make_solve_context(const coord_cube_t *cube) {
    // TODO: allocate phase1/phase2 contexts, coords stacks, extras stacks
    (void)cube;
    return NULL;
}

__attribute__((unused)) static void clear_solve_context(solver_ctx_t *solve_context) {
    // TODO: reset move/pruning stacks
    (void)solve_context;
}

__attribute__((unused)) static void destroy_solve_context(solver_ctx_t *context) {
    // TODO: free coords stacks, extras stacks, phase2 context
    (void)context;
}

// ---- solver entry ----

static solve_list_t *solver_333_4_solve(const puzzle_t *puzzle, const config_t *config) {
    // TODO: encode state (base coords + extras coords) and run threaded search
    (void)puzzle;
    (void)config;
    return NULL;
}

static void init(void) {
    build_move_tables();
    build_pruning_tables();

    // TODO: 333_4 extras build move tables
    // TODO: 333_4 extras build pruning tables
}

static void cleanup(void) { purge_cubie_move_table(); }

const solver_ops_t solver_333_4_ops = {
    .name        = "333-4",
    .puzzle_name = "333_4",

    .init    = init,
    .solve   = solver_333_4_solve,
    .cleanup = cleanup,
};
