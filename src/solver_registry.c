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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "puzzle.h"
#include "puzzles/puzzle_2x2.h"
#include "puzzles/puzzle_3x3.h"
#include "solver.h"
#include "solvers/solver_2x2_ida.h"
#include "solvers/solver_3x3_kociemba.h"

#define MAX_REGISTRATIONS 16

static const puzzle_ops_t *puzzle_registry[MAX_REGISTRATIONS];
static int                 n_puzzles = 0;

static const solver_ops_t *solver_registry[MAX_REGISTRATIONS];
static int                 n_solvers = 0;

void puzzle_register(const puzzle_ops_t *ops) {
    if (n_puzzles >= MAX_REGISTRATIONS)
        return;

    puzzle_registry[n_puzzles++] = ops;
}

const puzzle_ops_t *puzzle_lookup(const char *name) {
    for (int i = 0; i < n_puzzles; i++) {
        if (strcmp(puzzle_registry[i]->name, name) == 0)
            return puzzle_registry[i];
    }

    return NULL;
}

int puzzle_count(void) { return n_puzzles; }

const puzzle_ops_t *puzzle_by_index(int index) {
    if (index < 0 || index >= n_puzzles)
        return NULL;
    return puzzle_registry[index];
}

puzzle_t *puzzle_create(const char *name) {
    const puzzle_ops_t *ops = puzzle_lookup(name);

    if (ops == NULL)
        return NULL;

    puzzle_t *puzzle = (puzzle_t *)malloc(sizeof(puzzle_t));
    puzzle->ops      = ops;
    puzzle->state    = malloc(ops->state_size);

    ops->reset(puzzle->state);

    return puzzle;
}

void puzzle_destroy(puzzle_t *puzzle) {
    free(puzzle->state);
    free(puzzle);
}

void solver_register(const solver_ops_t *ops) {
    if (n_solvers >= MAX_REGISTRATIONS)
        return;

    solver_registry[n_solvers++] = ops;
}

const solver_ops_t *solver_lookup(const char *puzzle_name) {
    for (int i = 0; i < n_solvers; i++) {
        if (strcmp(solver_registry[i]->puzzle_name, puzzle_name) == 0)
            return solver_registry[i];
    }

    return NULL;
}

int solver_count(void) { return n_solvers; }

const solver_ops_t *solver_by_index(int index) {
    if (index < 0 || index >= n_solvers)
        return NULL;
    return solver_registry[index];
}

static int initialized = 0;

void init_registry(void) {
    if (initialized)
        return;

    puzzle_register(&puzzle_2x2_ops);
    puzzle_register(&puzzle_3x3_ops);
    solver_register(&solver_2x2_ida_ops);
    solver_register(&solver_3x3_kociemba_ops);

    initialized = 1;
}

solve_list_t *solve_puzzle(const char *puzzle_name, const char *state_str, const config_t *cfg) {
    init_registry();

    const solver_ops_t *solver = solver_lookup(puzzle_name);

    if (solver == NULL) {
        fprintf(stderr, "Error: no solver found for puzzle '%s'\n", puzzle_name);
        return NULL;
    }

    puzzle_t *puzzle = puzzle_create(puzzle_name);

    if (puzzle == NULL) {
        fprintf(stderr, "Error: unknown puzzle '%s'\n", puzzle_name);
        return NULL;
    }

    puzzle->ops->from_string(puzzle->state, state_str);

    solver->init();

    solve_list_t *solution = solver->solve(puzzle, cfg);

    puzzle_destroy(puzzle);

    return solution;
}
