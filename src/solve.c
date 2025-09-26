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

    printf("\n");
    printf("Solving\n");
    printf("Making phase1 context\n");
    solve_context_t *phase1_context = make_solve_context(original_cube);

    // FIXME: Only handles a single solution
    while (solves->solution == NULL) {
        phase1_solve_t *phase1_solution = NULL;

        printf("Getting phase1 solution\n");
        while (phase1_solution == NULL) {
            phase1_solution = get_phase1_solution(phase1_context, config);
            if (phase1_solution == NULL) {
                printf("No phase1 solution found\n");
                return NULL;
            }

            printf("Phase1 solution depth: %d\n", phase1_solution->depth);
        }

        printf("Phase1 solution: ");
        for (int i = 0; phase1_solution->solution[i] != MOVE_NULL; i++) {
            printf("%s ", move_to_str(phase1_solution->solution[i]));
        }
        printf("\n");

        assert(is_phase1_solved(phase1_context->cube));

        ////////////////////////////////////////////

        printf("Making phase2 context\n");

        solve_context_t *phase2_context = make_phase2_solve_context(phase1_solution, config);

        printf("Solving phase2\n");

        // we could find multipl solutions for phase2, but the first one is always the shortest
        phase2_solve_t *phase2_solution = solve_phase2(phase2_context, config);

        if (phase2_solution == NULL) {
            printf("No phase2 solution found\n");
            destroy_phase1_solve(phase1_solution);
            continue;
        }

        printf("Phase2 solution: ");
        for (int i = 0; phase2_solution->solution[i] != MOVE_NULL; i++) {
            printf("%s ", move_to_str(phase2_solution->solution[i]));
        }
        printf("\n");

        assert(is_phase2_solved(phase2_solution->cube));

        update_solve_list_node(solves, phase1_solution, phase2_solution);

        printf("Solution: ");
        for (int i = 0; solves->solution[i] != MOVE_NULL; i++) {
            printf("%s ", move_to_str(solves->solution[i]));
        }
        printf("\n");
        uint8_t solution_length = 0;
        for (int i = 0; solves->solution[i] != MOVE_NULL; i++) {
            solution_length++;
        }
        printf("Solution length: %d\n", solution_length);

        assert(solves->solution != NULL);

        assert(is_move_sequence_a_solution_for_cube(original_cube, solves->solution));
    }

    return solves;
}

phase1_solve_t *get_phase1_solution(solve_context_t *solve_context, const config_t *config) {
    const coord_cube_t *cube          = solve_context->cube;
    move_t             *move_stack    = solve_context->move_stack;
    coord_cube_t      **cube_stack    = solve_context->cube_stack;
    int                *pruning_stack = solve_context->pruning_stack;

    assert(solve_context->allowed_depth >= 0);
    assert(solve_context->allowed_depth < config->max_depth);

    for (; solve_context->allowed_depth < config->max_depth; solve_context->allowed_depth++) {
        if (solve_context->depth == 0) {
            copy_coord_cube(cube_stack[0], cube);
        }

        do {
            do {
                move_stack[solve_context->depth]++;
            } while (config->move_black_list[move_stack[solve_context->depth]] != MOVE_NULL &&
                     move_stack[solve_context->depth] < N_MOVES);

            if (move_stack[solve_context->depth] >= N_MOVES) {
                pruning_stack[solve_context->depth] = -1;
                move_stack[solve_context->depth]    = -1;
                solve_context->depth--;

                if (solve_context->depth < 0) {
                    solve_context->depth = 0;
                    break;
                } else if (solve_context->depth == 0) {
                    copy_coord_cube(cube_stack[0], cube);
                } else {
                    copy_coord_cube(cube_stack[solve_context->depth], cube_stack[solve_context->depth - 1]);
                }

                continue;
            }

            if (solve_context->depth > 0 &&
                is_duplicated_or_undoes_move(move_stack[solve_context->depth], move_stack[solve_context->depth - 1]))
                continue;

            assert(cube_stack[solve_context->depth] != NULL);
            assert(move_stack[solve_context->depth] <= N_MOVES);

            coord_apply_move(cube_stack[solve_context->depth], move_stack[solve_context->depth]);
            pruning_stack[solve_context->depth] = get_phase1_pruning(cube_stack[solve_context->depth]);

            if (is_phase1_solved(cube_stack[solve_context->depth])) {
                phase1_solve_t *phase1_solution = make_phase1_solve(cube_stack[solve_context->depth]);
                phase1_solution->depth          = solve_context->depth + 1;

                for (int i = 0; i <= solve_context->depth; i++) {
                    phase1_solution->solution[i] = move_stack[i];
                }
                phase1_solution->solution[solve_context->depth + 1] = MOVE_NULL;

                copy_coord_cube(phase1_solution->cube, cube_stack[solve_context->depth]);
                copy_coord_cube(solve_context->cube, cube_stack[solve_context->depth]);

                assert(is_phase1_solved(cube_stack[solve_context->depth]));
                assert(is_phase1_solved(phase1_solution->cube));
                assert(is_phase1_solved(solve_context->cube));

                return phase1_solution;
            }

            if (pruning_stack[solve_context->depth] + solve_context->depth < solve_context->allowed_depth) {
                copy_coord_cube(cube_stack[solve_context->depth + 1], cube_stack[solve_context->depth]);
                solve_context->depth++;
            } else {
                if (solve_context->depth > 0) {
                    copy_coord_cube(cube_stack[solve_context->depth], cube_stack[solve_context->depth - 1]);
                } else {
                    copy_coord_cube(cube_stack[solve_context->depth], cube);
                }
            }
        } while (1);
    }

    return NULL;
}

phase2_solve_t *solve_phase2(solve_context_t *solve_context, const config_t *config) {
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

    for (int allowed_depth = 1; allowed_depth <= solve_context->allowed_depth; allowed_depth++) {
        int pivot = 0;
        copy_coord_cube(cube_stack[0], cube);

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

            if (pivot > 0 && is_duplicated_or_undoes_move(move_stack[pivot], move_stack[pivot - 1]))
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
                end_time = get_microseconds();

                solve_stats_t *stats = get_current_stat();
                if (stats != NULL) {
                    stats->phase2_depth      = pivot + 1;
                    stats->phase2_move_count = move_count;
                    stats->phase2_solve_time = (float)(end_time - start_time) / 1000000.0;
                }

                phase2_solve_t *phase2_solution = make_phase2_solve(cube_stack[pivot]);
                phase2_solution->depth          = pivot + 1;
                phase2_solution->move_count     = move_count;

                for (int i = 0; i <= pivot; i++) {
                    phase2_solution->solution[i] = moves[move_stack[i]];
                }

                phase2_solution->solution[pivot + 1] = MOVE_NULL;

                copy_coord_cube(phase2_solution->cube, cube_stack[pivot]);

                assert(is_phase2_solved(cube_stack[pivot]));
                assert(is_phase2_solved(phase2_solution->cube));

                return phase2_solution;
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

    printf("Phase 2 solution not found\n");

    return NULL;
}

move_t *make_solution(const phase1_solve_t *phase1_solution, const phase2_solve_t *phase2_solution) {
    move_t *solution = (move_t *)malloc(sizeof(move_t) * (phase1_solution->depth + phase2_solution->depth + 1));
    for (int i = 0; i < phase1_solution->depth; i++) {
        solution[i] = phase1_solution->solution[i];
    }

    for (int i = 0; i < phase2_solution->depth; i++) {
        solution[phase1_solution->depth + i] = phase2_solution->solution[i];
    }

    solution[phase1_solution->depth + phase2_solution->depth] = MOVE_NULL;
    return solution;
}

phase1_solve_t *make_phase1_solve(const coord_cube_t *cube) {
    phase1_solve_t *phase1_solution = (phase1_solve_t *)malloc(sizeof(phase1_solve_t));
    phase1_solution->cube           = get_coord_cube();
    copy_coord_cube(phase1_solution->cube, cube);
    return phase1_solution;
}

void destroy_phase1_solve(phase1_solve_t *phase1_solution) {
    free(phase1_solution->cube);
    free(phase1_solution);
}

phase2_solve_t *make_phase2_solve(const coord_cube_t *cube) {
    phase2_solve_t *phase2_solution = (phase2_solve_t *)malloc(sizeof(phase2_solve_t));
    phase2_solution->cube           = get_coord_cube();
    copy_coord_cube(phase2_solution->cube, cube);
    return phase2_solution;
}

void destroy_phase2_solve(phase2_solve_t *phase2_solution) {
    free(phase2_solution->cube);
    free(phase2_solution);
}

solve_context_t *make_solve_context(const coord_cube_t *cube) {
    assert(cube != NULL);

    solve_context_t *phase1_context = (solve_context_t *)malloc(sizeof(solve_context_t));

    for (int i = 0; i < MAX_MOVES; i++) {
        phase1_context->cube_stack[i] = get_coord_cube();
    }

    clear_solve_context(phase1_context);

    phase1_context->cube = get_coord_cube();

    copy_coord_cube(phase1_context->cube, cube);

    phase1_context->allowed_depth = 0;
    phase1_context->depth         = 0;

    return phase1_context;
}

solve_context_t *make_phase2_solve_context(const phase1_solve_t *phase1_solution, const config_t *config) {
    solve_context_t *context = make_solve_context(phase1_solution->cube);
    context->allowed_depth   = config->max_depth - phase1_solution->depth;
    return context;
}

void clear_solve_context(solve_context_t *solve_context) {
    solve_context->depth = 0;

    for (int i = 0; i < MAX_MOVES; i++) {
        solve_context->move_stack[i]    = -1;
        solve_context->pruning_stack[i] = -1;
    }
}

void destroy_solve_context(solve_context_t *context) {
    assert(context != NULL);

    for (int i = 0; i < MAX_MOVES; i++) {
        free(context->cube_stack[i]);
    }

    free(context->cube);
    free(context);
}

void update_solve_list_node(solve_list_t *solves, const phase1_solve_t *phase1_solution,
                            const phase2_solve_t *phase2_solution) {

    move_t *phase1_solution_copy = (move_t *)malloc(sizeof(move_t) * (phase1_solution->depth + 1));
    move_t *phase2_solution_copy = (move_t *)malloc(sizeof(move_t) * (phase2_solution->depth + 1));
    for (int i = 0; i < phase1_solution->depth; i++) {
        phase1_solution_copy[i] = phase1_solution->solution[i];
    }
    for (int i = 0; i < phase2_solution->depth; i++) {
        phase2_solution_copy[i] = phase2_solution->solution[i];
    }
    phase1_solution_copy[phase1_solution->depth] = MOVE_NULL;
    phase2_solution_copy[phase2_solution->depth] = MOVE_NULL;

    solves->phase1_solution = phase1_solution_copy;
    solves->phase2_solution = phase2_solution_copy;

    solves->solution = make_solution(phase1_solution, phase2_solution);

    solves->stats = NULL; // FIXME
    solves->next  = NULL; // FIXME
}
