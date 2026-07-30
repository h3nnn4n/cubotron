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

#ifndef _SOLUTION
#define _SOLUTION

#include "moves.h"
#include "stats.h"

typedef struct solve_list_s solve_list_t;

typedef struct solve_list_s {
    solve_list_t *next;

    move_t *phase1_solution;
    move_t *phase2_solution;
    move_t *solution;

    solve_stats_t     *stats;
    aggregate_stats_t *aggregate;
} solve_list_t;

solve_list_t *new_solve_list_node();
void          destroy_solve_list_node(solve_list_t *node);
void          destroy_solve_list(solve_list_t *solves);

int  are_solutions_equal(const move_t *a, const move_t *b);
int  is_duplicate_solution(solve_list_t *solves_head, const move_t *solution);
void truncate_solutions(solve_list_t *solves, int n_solutions);

#endif /* end of include guard */
