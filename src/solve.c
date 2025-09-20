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

    free(node);
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

solve_list_t *solve(const coord_cube_t *original_cube, const config_t *config) {
    solve_list_t *solves = new_solve_list_node();
    coord_cube_t *cube   = get_coord_cube();

    solve_context_t *solve_context = make_solve_context(original_cube);

    const move_t *solution = solve_phase1(solve_context, config, solves);

    if (solution == NULL) {
        printf("Failed to solve");
        free(solves);
        return NULL;
    } else {
        copy_coord_cube(cube, original_cube);

        for (int i = 0; solution[i] != MOVE_NULL; i++) {
            coord_apply_move(cube, solution[i]);
        }

        assert(is_phase1_solved(cube));
        assert(is_phase2_solved(cube));
    }

    free(cube);

    destroy_solve_context(solve_context);

    return solves;
}

// FIXME: we need a decent way to get just the phase1 solution
move_t *solve_phase1(solve_context_t *solve_context, const config_t *config, solve_list_t *solves) {
    move_t *solution = NULL;

    const coord_cube_t *cube          = solve_context->cube;
    move_t             *move_stack    = solve_context->move_stack;
    coord_cube_t      **cube_stack    = solve_context->cube_stack;
    int                *pruning_stack = solve_context->pruning_stack;

    int      solution_count = 0;
    uint64_t move_count     = 0;

    uint64_t phase2_time = 0;
    uint64_t start_time  = get_microseconds();
    uint64_t end_time    = 0;
    /*int move_estimate = get_phase1_pruning(cube);*/
    /*printf("estimated number of moves: %d\n", move_estimate);*/

    for (int allowed_depth = 1; allowed_depth <= config->max_depth; allowed_depth++) {
        int pivot = 0;
        /*printf("searching with max depth: %d\n", allowed_depth);*/

        copy_coord_cube(cube_stack[0], cube);

        do {
            do {
                move_stack[pivot]++;
            } while (config->move_black_list[move_stack[pivot]] != MOVE_NULL && move_stack[pivot] < N_MOVES);

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

            if (pivot > 0 && is_bad_move(move_stack[pivot], move_stack[pivot - 1]))
                continue;

            assert(move_stack[pivot] <= N_MOVES);

            coord_apply_move(cube_stack[pivot], move_stack[pivot]);
            pruning_stack[pivot] = get_phase1_pruning(cube_stack[pivot]);
            move_count++;

            /*
            if (move_count % 10000000 == 0) {
                char buffer[512];
                sprintf(buffer, " moves: %14lu pivot: %2d estimated_dist: %2d", move_count, pivot,
                        pruning_stack[pivot]);
                sprintf(buffer, "%s : %4d %4d %3d -> ", buffer, cube_stack[pivot]->edge_orientations,
                        cube_stack[pivot]->corner_orientations, cube_stack[pivot]->E_slice);
                for (int i = 0; i <= pivot; i++)
                    sprintf(buffer, "%s %s", buffer, move_to_str(move_stack[i]));
                printf("%s", buffer);

                uint64_t end_time = get_microseconds();
                printf("  :  elapsed time: %f seconds - ", (float)(end_time - start_time) / 1000000.0);
                printf("moves: %lu - ", move_count);
                printf("moves per second : %.2f", ((float)move_count / (end_time - start_time)) * 1000000.0);
                printf("\n");
            }
            */

            if (is_phase1_solved(cube_stack[pivot])) {
                end_time = get_microseconds();

                solve_stats_t *stats = get_current_stat();
                if (stats != NULL) {
                    stats->phase1_depth      = pivot + 1;
                    stats->phase1_move_count = move_count;
                    stats->phase1_solve_time = (float)(end_time - phase2_time - start_time) / 1000000.0;

                    if (solves != NULL) {
                        solves->stats = stats;
                    }
                }
            }

            if (is_phase1_solved(cube_stack[pivot])) {
                /*printf("phase1 solution found with depth %2d - ", pivot);*/
                solution                = malloc(sizeof(move_t) * (40));
                move_t *phase1_solution = malloc(sizeof(move_t) * (40));

                for (int i = 0; i <= pivot; i++) {
                    solution[i]        = move_stack[i];
                    phase1_solution[i] = move_stack[i];
                    /*printf(" %s", move_to_str(solution[i]));*/
                }
                solution[pivot + 1]        = MOVE_NULL;
                phase1_solution[pivot + 1] = MOVE_NULL;
                /*printf("\n");*/

                // zero means just phase1 solution.
                // -1 is find all solutions
                if (config->n_solutions == 0) {
                    /*printf("doing just phase1!\n");*/
                    goto solution_found;
                }

                copy_coord_cube(solve_context->phase2_context->cube, cube_stack[pivot]);
                coord_cube_t *phase2_cube = solve_context->phase2_context->cube;

                uint64_t phase2_start = get_microseconds();
                move_t  *phase2_solution =
                    solve_phase2(solve_context->phase2_context, config, config->max_depth - pivot - 1);
                uint64_t phase2_end = get_microseconds();
                phase2_time += phase2_end - phase2_start;

                if (phase2_solution == NULL) {
                    free(solution);
                    solution = NULL;

                    free(phase1_solution);
                    phase1_solution = NULL;

                    /*printf("failed to solve phase2\n");*/
                } else {
                    solution_count += 1;

                    int phase2_move_count = 0;
                    for (int i = 0; phase2_solution[i] != MOVE_NULL; i++)
                        phase2_move_count++;

                    for (int i = 0; phase2_solution[i] != MOVE_NULL; i++) {
                        coord_apply_move(phase2_cube, phase2_solution[i]);
                        solution[pivot + i + 1] = phase2_solution[i];
                    }
                    solution[pivot + phase2_move_count + 1] = MOVE_NULL;

                    /*printf("phase1 solution found with depth %2d - ", pivot + 1);*/
                    /*for (int i = 0; i <= pivot; i++) {*/
                    /*printf(" %s", move_to_str(solution[i]));*/
                    /*}*/
                    /*printf("\n");*/

                    /*printf("phase2 solution found with depth %2d - ", phase2_move_count);*/
                    /*for (int i = 0; phase2_solution[i] != MOVE_NULL; i++) {*/
                    /*printf(" %s", move_to_str(phase2_solution[i]));*/
                    /*}*/
                    /*printf("\n");*/

                    /*int length = 0;*/
                    /*printf("solution:\n");*/
                    /*for (int i = 0; solution[i] != MOVE_NULL; i++, length++) {*/
                    /*printf(" %s", move_to_str(solution[i]));*/
                    /*}*/
                    /*printf("\n");*/
                    /*printf("length: %d\n", length);*/
                    /*printf("\n");*/

                    if (solves != NULL) {
                        solves->phase1_solution = phase1_solution;
                        solves->phase2_solution = phase2_solution;
                        solves->solution        = solution;

                        solves->next = new_solve_list_node();
                        solves       = solves->next;
                    } else {
                        free(phase1_solution);
                        free(phase2_solution);
                    }

                    assert(is_coord_solved(phase2_cube));

                    finish_stats();
                }

                if (config->n_solutions != -1 && solution_count >= config->n_solutions) {
                    goto solution_found;
                } else {
                    /*free(solution);*/
                    /*solution = NULL;*/
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

    /*printf("elapsed time: %f seconds - ", (float)(end_time - start_time) / 1000000.0);*/
    /*printf("moves: %lu - ", move_count);*/
    /*printf("moves per second : %.2f\n", ((float)move_count / (end_time - start_time)) * 1000000.0);*/

    // We allocate a new node eargely, but the last to be allocated will always
    // have to be discarted, otherwise we would have an empty node at the end.
    if (solves != NULL && solves->next != NULL) {
        destroy_solve_list_node(solves->next);
        solves->next = NULL;
    }

    return solution;
}

move_t *solve_phase2(solve_context_t *solve_context, __attribute__((unused)) const config_t *config, int max_depth) {
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

    uint64_t start_time = get_microseconds();
    uint64_t end_time   = 0;

    for (int allowed_depth = 1; allowed_depth <= max_depth; allowed_depth++) {
        int pivot = 0;
        copy_coord_cube(cube_stack[0], cube);

        /*printf("searching with max depth: %d\n", allowed_depth);*/

        do {
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

            if (pivot > 0 && is_bad_move(move_stack[pivot], move_stack[pivot - 1]))
                continue;

            coord_apply_move(cube_stack[pivot], moves[move_stack[pivot]]);
            pruning_stack[pivot] = get_phase2_pruning(cube_stack[pivot]);
            move_count++;

            /*
            if (move_count % 1000000 == 0) {
                char buffer[512];
                sprintf(buffer, " moves: %4d pivot: %2d", move_count, pivot);
                sprintf(buffer, "%s : %4d -> ", buffer, cube_stack[pivot]->corner_permutations);
                for (int i = 0; i <= pivot; i++)
                    sprintf(buffer, "%s %s", buffer, move_to_str(move_stack[i]));
                printf("%s\n", buffer);
            }
            */

            if (is_phase2_solved(cube_stack[pivot])) {
                solution = malloc(sizeof(move_t) * (pivot + 2));

                for (int i = 0; i <= pivot; i++)
                    solution[i] = moves[move_stack[i]];
                solution[pivot + 1] = MOVE_NULL;

                end_time = get_microseconds();

                solve_stats_t *stats = get_current_stat();
                if (stats != NULL) {
                    stats->phase2_depth      = pivot + 1;
                    stats->phase2_move_count = move_count;
                    stats->phase2_solve_time = (float)(end_time - start_time) / 1000000.0;
                }

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
