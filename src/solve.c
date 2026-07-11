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

#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "coord_move_tables.h"
#include "cubie_cube.h"
#include "facelets.h"
#include "pruning.h"
#include "solve.h"
#include "stats.h"
#include "utils.h"

solve_list_t *new_solve_list_node() {
    solve_list_t *node = (solve_list_t *)malloc(sizeof(solve_list_t));

    node->next            = NULL;
    node->stats           = NULL;
    node->aggregate       = NULL;
    node->phase1_solution = NULL;
    node->phase2_solution = NULL;
    node->solution        = NULL;

    return node;
}

void destroy_solve_list_node(solve_list_t *node) {
    if (node->phase1_solution != NULL) {
        free(node->phase1_solution);
        node->phase1_solution = NULL;
    }

    if (node->phase2_solution != NULL) {
        free(node->phase2_solution);
        node->phase2_solution = NULL;
    }

    if (node->solution != NULL) {
        free(node->solution);
        node->solution = NULL;
    }

    free(node->aggregate);
    node->aggregate = NULL;

    free(node);
}

void destroy_solve_list(solve_list_t *solves) {
    const solve_stats_t *prev_stats = NULL;

    while (solves != NULL) {
        solve_list_t *next = solves->next;

        if (solves->stats != NULL && solves->stats != prev_stats) {
            prev_stats = solves->stats;
            free(solves->stats);
            solves->stats = NULL;
        }

        destroy_solve_list_node(solves);
        solves = next;
    }
}

solve_list_t *solve_facelets_single(char facelets[N_FACELETS]) {
    const config_t *config = get_config();
    return solve_facelets(facelets, config);
}

solve_list_t *solve_facelets(char facelets[N_FACELETS], const config_t *config) {
    cube_cubie_t *cubie_cube = build_cubie_cube_from_str(facelets);
    coord_cube_t *cube       = make_coord_cube(cubie_cube);
    solve_list_t *solution   = solve(cube, config);

    free(cubie_cube);
    free(cube);

    return solution;
}

solve_list_t *solve_single(const coord_cube_t *original_cube) {
    const config_t *config   = get_config();
    solve_list_t   *solution = solve(original_cube, config);

    return solution;
}

static solve_list_t *make_trivial_solution() {
    solve_list_t *solves       = new_solve_list_node();
    solves->solution           = malloc(sizeof(move_t));
    solves->solution[0]        = MOVE_NULL;
    solves->phase1_solution    = malloc(sizeof(move_t));
    solves->phase1_solution[0] = MOVE_NULL;
    solves->phase2_solution    = malloc(sizeof(move_t));
    solves->phase2_solution[0] = MOVE_NULL;

    solve_stats_t *stats     = get_solve_stats();
    stats->phase1_depth      = 0;
    stats->phase2_depth      = 0;
    stats->solution_length   = 0;
    stats->phase1_solve_time = 0.0f;
    stats->total_phase2_time = 0.0f;
    stats->wall_time         = 0.0f;
    stats->phase1_move_count = 0;
    stats->phase2_move_count = 0;
    stats->total_moves       = 0;
    solves->stats            = stats;

    return solves;
}

#define MAX_SOLUTION_LENGTHS 1024

static solve_list_t *collect_results(thread_context_t *thread_contexts, int thread_count,
                                     solve_stats_t **all_thread_stats, int *is_winner, int *all_lengths,
                                     int *n_lengths) {
    solve_list_t *solves  = NULL;
    solve_list_t *current = NULL;

    for (int i = 0; i < thread_count; i++) {
        all_thread_stats[i] = thread_contexts[i].stats;
        is_winner[i]        = 0;
    }

    for (int i = 0; i < thread_count; i++) {
        solve_list_t *thread_solves = thread_contexts[i].solves;
        if (thread_solves->solution != NULL) {
            is_winner[i]       = 1;
            solve_list_t *node = thread_solves;
            while (node != NULL && node->solution != NULL) {
                int len = 0;
                while (node->solution[len] != MOVE_NULL)
                    len++;
                if (*n_lengths < MAX_SOLUTION_LENGTHS) {
                    all_lengths[(*n_lengths)++] = len;
                }
                solve_list_t *next = node->next;
                node->next         = NULL;
                if (solves == NULL) {
                    solves  = node;
                    current = solves;
                } else {
                    current->next = node;
                    current       = node;
                }
                node = next;
            }
            if (node != NULL) {
                destroy_solve_list_node(node);
            }
        } else {
            thread_solves->stats = NULL;
            destroy_solve_list(thread_solves);
        }
    }

    return solves;
}

void truncate_solutions(solve_list_t *solves, int n_solutions) {
    int           n   = 0;
    solve_list_t *cur = solves;
    while (cur != NULL && cur->solution != NULL) {
        n++;
        if (n == n_solutions) {
            solve_list_t *tail = cur->next;
            cur->next          = NULL;
            while (tail != NULL) {
                solve_list_t *next = tail->next;
                tail->stats        = NULL;
                destroy_solve_list_node(tail);
                tail = next;
            }
            break;
        }
        cur = cur->next;
    }
}

solve_list_t *solve(const coord_cube_t *original_cube, const config_t *config) {
    if (is_coord_solved(original_cube)) {
        return make_trivial_solution();
    }

    get_config()->die = false;
    atomic_store(&get_config()->solutions_found, 0);

    const int        thread_count = config->thread_count;
    thread_context_t thread_contexts[thread_count];
    move_t           move_list[thread_count];

    for (int i = 0; i < thread_count; i++) {
        move_list[i] = MOVE_NULL;
    }

    for (int i = 0; i < thread_count; i++) {
        thread_contexts[i].solve_context = make_solve_context(original_cube);
        thread_contexts[i].solves        = new_solve_list_node();
        thread_contexts[i].stats         = get_solve_stats();
        move_list[0]                     = i;
        prep_phase1(thread_contexts[i].solve_context, 1, move_list);
    }

    pthread_t threads[thread_count];
    for (int i = 0; i < thread_count; i++) {
        pthread_create(&threads[i], NULL, (void *(*)(void *))solve_thread, (void *)&thread_contexts[i]);
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    int all_lengths[MAX_SOLUTION_LENGTHS];
    int n_lengths = 0;

    solve_stats_t *all_thread_stats[thread_count];
    int            is_winner[thread_count];

    solve_list_t *solves =
        collect_results(thread_contexts, thread_count, all_thread_stats, is_winner, all_lengths, &n_lengths);

    if (config->n_solutions > 0) {
        truncate_solutions(solves, config->n_solutions);
    }

    aggregate_stats_t *aggregate = compute_aggregate_stats(all_thread_stats, thread_count, all_lengths, n_lengths);
    if (solves != NULL) {
        solves->aggregate = aggregate;
    } else {
        free(aggregate);
    }

    for (int i = 0; i < thread_count; i++) {
        if (!is_winner[i]) {
            free(thread_contexts[i].stats);
        }
    }

    if (config->n_solutions > 0 && solves != NULL && solves->solution != NULL) {
        for (solve_list_t *outer = solves; outer != NULL && outer->solution != NULL; outer = outer->next) {
            for (solve_list_t *inner = outer->next; inner != NULL && inner->solution != NULL; inner = inner->next) {
                assert(!are_solutions_equal(outer->solution, inner->solution));
            }
        }
    }

    for (int i = 0; i < thread_count; i++) {
        destroy_solve_context(thread_contexts[i].solve_context);
    }

    return solves;
}

solve_list_t *solve_thread(void *arg) {
    thread_context_t *thread_context = (thread_context_t *)arg;
    solve_context_t  *solve_context  = thread_context->solve_context;
    solve_list_t     *solves         = thread_context->solves;

    solve_phase1(solve_context, solves, thread_context->stats);

    if (solves->solution != NULL) {
        for (solve_list_t *cur = solves; cur != NULL && cur->solution != NULL; cur = cur->next) {
            patch_solution(solve_context, cur);
        }

        coord_cube_t *cube = get_coord_cube();
        copy_coord_cube(cube, solve_context->original_cube);

        for (int i = 0; solves->solution[i] != MOVE_NULL; i++) {
            coord_apply_move(cube, solves->solution[i]);
        }

        assert(is_phase1_solved(cube));

        if (solves->phase2_solution != NULL) {
            assert(is_phase2_solved(cube));
        }

        free(cube);
    }

    return solves;
}

void prep_phase1(solve_context_t *solve_context, uint16_t move_count, const move_t *move_list) {
    for (int i = 0; i < MAX_MOVES; i++) {
        solve_context->prep_moves[i] = MOVE_NULL;
    }

    for (int i = 0; i < move_count; i++) {
        solve_context->prep_moves[i] = move_list[i];
    }

    for (int i = 0; i < move_count; i++) {
        coord_apply_move(solve_context->cube, solve_context->prep_moves[i]);
    }

    solve_context->prep_move_count = move_count;
}

move_t *patch_solution(solve_context_t *solve_context, solve_list_t *solution) {
    uint16_t solution_length = 0;

    for (int i = 0; solution->solution[i] != MOVE_NULL; i++) {
        solution_length++;
    }

    move_t  *partial_solution = solution->solution;
    uint16_t total_length     = solution_length + solve_context->prep_move_count;
    solution->solution        = malloc(sizeof(move_t) * (total_length + 1));

    for (int i = 0; i < total_length; i++) {
        solution->solution[i] = MOVE_NULL;
    }

    for (int i = 0; i < solve_context->prep_move_count; i++) {
        solution->solution[i] = solve_context->prep_moves[i];
    }

    for (int i = 0; partial_solution[i] != MOVE_NULL; i++) {
        solution->solution[i + solve_context->prep_move_count] = partial_solution[i];
    }

    solution->solution[solve_context->prep_move_count + solution_length] = MOVE_NULL;

    free(partial_solution);

    move_t *phase1_solution   = solution->phase1_solution;
    solution->phase1_solution = malloc(sizeof(move_t) * (solution_length + solve_context->prep_move_count + 1));

    for (int i = 0; i < solution_length + solve_context->prep_move_count; i++) {
        solution->phase1_solution[i] = solution->solution[i];
    }

    solution->phase1_solution[solution_length + solve_context->prep_move_count] = MOVE_NULL;

    free(phase1_solution);

    return solution->solution;
}

static void build_phase1_solution(const move_t *move_stack, int pivot, move_t **solution, move_t **phase1_solution) {
    *solution        = malloc(sizeof(move_t) * 40);
    *phase1_solution = malloc(sizeof(move_t) * 40);

    for (int i = 0; i <= pivot; i++) {
        (*solution)[i]        = move_stack[i];
        (*phase1_solution)[i] = move_stack[i];
    }
    (*solution)[pivot + 1]        = MOVE_NULL;
    (*phase1_solution)[pivot + 1] = MOVE_NULL;
}

static int assemble_full_solution(move_t *solution, int pivot, const move_t *phase2_solution,
                                  coord_cube_t *phase2_cube) {
    int phase2_move_count = 0;
    for (int i = 0; phase2_solution[i] != MOVE_NULL; i++)
        phase2_move_count++;

    for (int i = 0; phase2_solution[i] != MOVE_NULL; i++) {
        coord_apply_move(phase2_cube, phase2_solution[i]);
        solution[pivot + i + 1] = phase2_solution[i];
    }
    solution[pivot + phase2_move_count + 1] = MOVE_NULL;

    return phase2_move_count;
}

static void record_solution_stats(solve_stats_t *stats, int pivot, int phase2_move_count, uint64_t move_count,
                                  uint8_t prep_move_count) {
    if (stats->solutions_found == 0) {
        stats->phase1_depth    = pivot + 1;
        stats->phase2_depth    = phase2_move_count;
        stats->solution_length = pivot + phase2_move_count + 1 + prep_move_count;
    }
    stats->phase1_move_count = move_count;
}

static void store_solution_in_list(solve_list_t **solves_ptr, move_t *phase1_solution, move_t *phase2_solution,
                                   move_t *solution) {
    solve_list_t *solves = *solves_ptr;

    if (solves != NULL) {
        if (solves->solution != NULL) {
            solves->next = new_solve_list_node();
            solves       = solves->next;
        }

        solves->phase1_solution = phase1_solution;
        solves->phase2_solution = phase2_solution;
        solves->solution        = solution;
    } else {
        free(phase1_solution);
        free(phase2_solution);
        free(solution);
    }

    *solves_ptr = solves;
}

int are_solutions_equal(const move_t *a, const move_t *b) {
    int i = 0;
    while (a[i] != MOVE_NULL && b[i] != MOVE_NULL) {
        if (a[i] != b[i])
            return 0;
        i++;
    }
    return a[i] == b[i];
}

int is_duplicate_solution(solve_list_t *solves_head, const move_t *solution) {
    for (solve_list_t *n = solves_head; n != NULL && n->solution != NULL; n = n->next) {
        if (are_solutions_equal(solution, n->solution))
            return 1;
    }
    return 0;
}

move_t *solve_phase1(solve_context_t *solve_context, solve_list_t *solves, solve_stats_t *stats) {
    move_t *solution = NULL;

    const config_t *config = get_config();

    const coord_cube_t *cube          = solve_context->cube;
    move_t             *move_stack    = solve_context->move_stack;
    coord_cube_t      **cube_stack    = solve_context->cube_stack;
    int                *pruning_stack = solve_context->pruning_stack;

    uint64_t move_count = 0;

    uint64_t phase2_time = 0;
    uint64_t start_time  = get_microseconds();
    uint64_t end_time    = 0;
    /*int move_estimate = get_phase1_pruning(cube);*/
    /*printf("estimated number of moves: %d\n", move_estimate);*/

    // printf("solving phase1 with prep moves: ");
    // print_move_sequence(solve_context->prep_moves);

    solve_list_t *solves_head = solves;

    for (int allowed_depth = 1; allowed_depth <= config->max_depth; allowed_depth++) {
        int pivot = 0;
        /*printf("searching with max depth: %d\n", allowed_depth);*/

        copy_coord_cube(cube_stack[0], cube);

        do {
            if (get_config()->die) {
                uint64_t die_end = get_microseconds();
                finalize_solve_stats(stats, start_time, die_end, phase2_time, 1);
                return NULL;
            }

            do {
                move_stack[pivot]++;
            } while (move_stack[pivot] < N_MOVES && config->move_black_list[move_stack[pivot]] != MOVE_NULL);

            if (move_stack[pivot] >= N_MOVES) {
                pruning_stack[pivot] = -1; // ?
                move_stack[pivot]    = -1;
                /*printf("\n");*/
                pivot--;

                if (pivot < 0) {
                    break;
                } else if (pivot == 0) {
                    copy_coord_cube(cube_stack[0], cube);
                } else {
                    copy_coord_cube(cube_stack[pivot], cube_stack[pivot - 1]);
                }

                continue;
            }

            if (pivot > 0 && is_duplicated_or_undoes_move(move_stack[pivot], move_stack[pivot - 1]))
                continue;

            assert(move_stack[pivot] <= N_MOVES);

            coord_apply_move_phase1(cube_stack[pivot], move_stack[pivot]);
            pruning_stack[pivot] = get_phase1_pruning(cube_stack[pivot]);
            move_count++;

            if (is_phase1_solved(cube_stack[pivot])) {
                end_time = get_microseconds();

                stats->phase1_depth      = pivot + 1;
                stats->phase1_move_count = move_count;
                stats->phase1_solve_time = (float)(end_time - phase2_time - start_time) / 1000000.0;

                if (solves != NULL) {
                    solves->stats = stats;
                }

                move_t *phase1_solution;
                build_phase1_solution(move_stack, pivot, &solution, &phase1_solution);

                if (config->n_solutions == 0) {
                    if (solves != NULL) {
                        solves->solution        = solution;
                        solves->phase1_solution = phase1_solution;
                    }
                    get_config()->die = true;
                    goto solution_found;
                }

                // Replay phase1 moves through full coord_apply_move to reconstruct
                // phase2 coordinates (phase1-only DFS skipped them for performance).
                // solve_context->cube has prep_moves applied via full coord_apply_move
                // and is never modified by the DFS — safe to replay from here.
                coord_cube_t *temp_cube = get_coord_cube();
                copy_coord_cube(temp_cube, solve_context->cube);
                for (int i = 0; i <= pivot; i++) {
                    coord_apply_move(temp_cube, move_stack[i]);
                }
                copy_coord_cube(solve_context->phase2_context->cube, temp_cube);
                free(temp_cube);

                coord_cube_t *phase2_cube = solve_context->phase2_context->cube;

                uint64_t phase2_start = get_microseconds();
                move_t  *phase2_solution =
                    solve_phase2(solve_context->phase2_context, config, config->max_depth - pivot - 1, stats);
                uint64_t phase2_end = get_microseconds();
                phase2_time += phase2_end - phase2_start;
                stats->phase2_attempts++;

                if (phase2_solution == NULL) {
                    free(solution);
                    solution = NULL;

                    free(phase1_solution);
                    phase1_solution = NULL;
                } else {
                    int phase2_move_count = assemble_full_solution(solution, pivot, phase2_solution, phase2_cube);

                    int is_duplicate = (config->n_solutions > 0) && is_duplicate_solution(solves_head, solution);

                    if (is_duplicate) {
                        free(solution);
                        solution = NULL;

                        free(phase1_solution);
                        phase1_solution = NULL;

                        free(phase2_solution);
                        phase2_solution = NULL;
                    } else {
                        stats->phase2_successes++;
                        int global_count = atomic_fetch_add(&get_config()->solutions_found, 1) + 1;

                        if (config->n_solutions != -1 && global_count > config->n_solutions) {
                            free(solution);
                            free(phase1_solution);
                            free(phase2_solution);
                            get_config()->die = true;
                            goto solution_found;
                        }

                        record_solution_stats(stats, pivot, phase2_move_count, move_count,
                                              solve_context->prep_move_count);
                        store_solution_in_list(&solves, phase1_solution, phase2_solution, solution);
                        stats->solutions_found++;

                        assert(is_coord_solved(phase2_cube));

                        if (config->n_solutions != -1 && global_count >= config->n_solutions) {
                            get_config()->die = true;
                            goto solution_found;
                        }
                    }
                }
            }

            if (pruning_stack[pivot] + pivot < allowed_depth) {
                copy_coord_cube(cube_stack[pivot + 1], cube_stack[pivot]);
                pivot++;
            } else {
                if (pivot > 0) {
                    copy_coord_cube(cube_stack[pivot], cube_stack[pivot - 1]);
                } else {
                    copy_coord_cube(cube_stack[pivot], cube);
                }
            }
        } while (1);
    }

solution_found:

    // We allocate a new node eargely, but the last to be allocated will always
    // have to be discarted, otherwise we would have an empty node at the end.
    if (solves != NULL && solves->next != NULL) {
        destroy_solve_list_node(solves->next);
        solves->next = NULL;
    }

    finalize_solve_stats(stats, start_time, end_time, phase2_time, 0);

    return solution;
}

// Utility functions for testing
int is_phase1_moves_solved(const move_t *solution, const coord_cube_t *original_cube) {
    coord_cube_t *cube = get_coord_cube();
    copy_coord_cube(cube, original_cube);

    for (int i = 0; solution[i] != MOVE_NULL; i++) {
        coord_apply_move(cube, solution[i]);
    }

    int result = is_phase1_solved(cube);
    free(cube);
    return result;
}

static move_t *build_phase2_solution(const move_t *moves, const move_t *move_stack, int pivot) {
    move_t *solution = malloc(sizeof(move_t) * (pivot + 2));
    for (int i = 0; i <= pivot; i++)
        solution[i] = moves[move_stack[i]];
    solution[pivot + 1] = MOVE_NULL;
    return solution;
}

move_t *solve_phase2(solve_context_t *solve_context, __attribute__((unused)) const config_t *config, int max_depth,
                     solve_stats_t *stats) {
    move_t *solution = NULL;
    move_t  moves[]  = {MOVE_U1, MOVE_U2, MOVE_U3, MOVE_D1, MOVE_D2, MOVE_D3, MOVE_R2, MOVE_L2, MOVE_F2, MOVE_B2};
    int     n_moves  = 10;

    for (int i = 0; i < MAX_MOVES; i++) {
        solve_context->move_stack[i]    = -1;
        solve_context->pruning_stack[i] = -1;
        reset_coord_cube(solve_context->cube_stack[i]);
    }

    copy_coord_cube(solve_context->cube_stack[0], solve_context->cube);

    uint64_t move_count = 0;

    const coord_cube_t *cube          = solve_context->cube;
    move_t             *move_stack    = solve_context->move_stack;
    coord_cube_t      **cube_stack    = solve_context->cube_stack;
    int                *pruning_stack = solve_context->pruning_stack;

    for (int allowed_depth = 1; allowed_depth <= max_depth; allowed_depth++) {
        int pivot = 0;
        copy_coord_cube(cube_stack[0], cube);

        /*printf("searching with max depth: %d\n", allowed_depth);*/

        do {
            if (get_config()->die) {
                return NULL;
            }

            do {
                move_stack[pivot]++;
            } while ((int)move_stack[pivot] < n_moves &&
                     config->move_black_list[moves[move_stack[pivot]]] != MOVE_NULL);

            if ((int)move_stack[pivot] >= n_moves) {
                pruning_stack[pivot] = -1; // ?
                move_stack[pivot]    = -1;
                /*printf("\n");*/
                pivot--;

                if (pivot < 0) {
                    break;
                } else if (pivot == 0) {
                    copy_coord_cube(cube_stack[0], cube);
                } else {
                    copy_coord_cube(cube_stack[pivot], cube_stack[pivot - 1]);
                }

                continue;
            }

            if (pivot > 0 && is_duplicated_or_undoes_move(moves[move_stack[pivot]], moves[move_stack[pivot - 1]]))
                continue;

            coord_apply_move(cube_stack[pivot], moves[move_stack[pivot]]);
            pruning_stack[pivot] = get_phase2_pruning(cube_stack[pivot]);
            move_count++;

            if (is_phase2_solved(cube_stack[pivot])) {
                solution = build_phase2_solution(moves, move_stack, pivot);
                stats->phase2_move_count += move_count;
                goto solution_found;
            }

            if (pruning_stack[pivot] + pivot < allowed_depth) {
                copy_coord_cube(cube_stack[pivot + 1], cube_stack[pivot]);
                pivot++;
            } else {
                if (pivot > 0) {
                    copy_coord_cube(cube_stack[pivot], cube_stack[pivot - 1]);
                } else {
                    copy_coord_cube(cube_stack[pivot], cube);
                }
            }
        } while (1);
    }

    stats->phase2_move_count += move_count;

solution_found:

    return solution;
}

solve_context_t *make_solve_context(const coord_cube_t *cube) {
    solve_context_t *phase1_context = (solve_context_t *)malloc(sizeof(solve_context_t));
    solve_context_t *phase2_context = (solve_context_t *)malloc(sizeof(solve_context_t));

    for (int i = 0; i < MAX_MOVES; i++) {
        phase1_context->cube_stack[i] = get_coord_cube();
        phase2_context->cube_stack[i] = get_coord_cube();
    }

    clear_solve_context(phase1_context);
    clear_solve_context(phase2_context);

    phase1_context->cube = get_coord_cube();
    phase2_context->cube = get_coord_cube();

    copy_coord_cube(phase1_context->cube, cube);

    phase1_context->phase2_context = phase2_context;
    phase2_context->phase2_context = NULL;

    phase1_context->original_cube = cube;

    return phase1_context;
}

void clear_solve_context(solve_context_t *solve_context) {
    for (int i = 0; i < MAX_MOVES; i++) {
        solve_context->move_stack[i]    = -1;
        solve_context->pruning_stack[i] = -1;
    }
}

void destroy_solve_context(solve_context_t *context) {
    assert(context != NULL);

    if (context->phase2_context != NULL)
        destroy_solve_context(context->phase2_context);

    for (int i = 0; i < MAX_MOVES; i++) {
        free(context->cube_stack[i]);
    }

    free(context->cube);
    free(context);
}
