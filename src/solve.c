#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "coord_move_tables.h"
#include "cubie_cube.h"
#include "facelets.h"
#include "pruning.h"
#include "solve.h"
#include "stats.h"
#include "utils.h"

#define MAX_MOVES 30

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

solve_list_t *solve_facelets_single(char facelets[N_FACELETS]) { return solve_facelets(facelets, 30, 0, 1); }

solve_list_t *solve_facelets(char facelets[N_FACELETS], int max_depth, float timeout, int max_solutions) {
    cube_cubie_t *cubie_cube = build_cubie_cube_from_str(facelets);
    coord_cube_t *cube       = make_coord_cube(cubie_cube);
    solve_list_t *solution   = solve(cube, max_depth, timeout, max_solutions);

    free(cubie_cube);

    return solution;
}

solve_list_t *solve_single(coord_cube_t *original_cube) {
    solve_list_t *solution = solve(original_cube, 40, 0, 1);

    return solution;
}

solve_list_t *solve(coord_cube_t *original_cube, int max_depth, float timeout, int max_solutions) {
    solve_list_t *solves = new_solve_list_node();
    coord_cube_t *cube   = get_coord_cube();
    copy_coord_cube(cube, original_cube);

    move_t *solution = solve_phase1(cube, max_depth, timeout, max_solutions, solves);

    if (solution == NULL) {
        free(solution);
        return NULL;
    } else {
        copy_coord_cube(cube, original_cube);

        int solution_length = 0;

        for (int i = 0; solution[i] != MOVE_NULL; i++) {
            solution_length += 1;
            coord_apply_move(cube, solution[i]);
        }

        assert(is_phase1_solved(cube));
        assert(is_phase2_solved(cube));
    }

    free(cube);

    return solves;
}

// FIXME: we need a decent way to get just the phase1 solution
move_t *solve_phase1(coord_cube_t *cube, int max_depth, __attribute__((unused)) float timeout, int max_solutions,
                     solve_list_t *solves) {
    move_t *solution = NULL;

    move_t            move_stack[MAX_MOVES];
    int               pruning_stack[MAX_MOVES];
    coord_cube_t *    cube_stack[MAX_MOVES];
    unsigned long int move_count = 0;

    int solution_count = 0;

    for (int i = 0; i < MAX_MOVES; i++) {
        move_stack[i]    = -1;
        pruning_stack[i] = -1;
        cube_stack[i]    = get_coord_cube();
    }

    long phase2_time = 0;
    long start_time  = get_microseconds();
    long end_time    = 0;
    /*int move_estimate = get_phase1_pruning(cube);*/
    /*printf("estimated number of moves: %d\n", move_estimate);*/

    for (int allowed_depth = 1; allowed_depth <= max_depth; allowed_depth++) {
        int pivot = 0;
        /*printf("searching with max depth: %d\n", allowed_depth);*/

        copy_coord_cube(cube_stack[0], cube);

        do {
            move_stack[pivot]++;

            if (move_stack[pivot] >= N_MOVES) {
                pruning_stack[pivot] = -1; // ?
                move_stack[pivot]    = -1;
                /*printf("\n");*/
                pivot--;

                if (pivot < 0) {
                    break;
                } else if (pivot == 0) {
                    copy_coord_cube(cube_stack[0], cube);
                } else if (pivot > 0) {
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

                long end_time = get_microseconds();
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
                if (max_solutions == 0) {
                    /*printf("doing just phase1!\n");*/
                    goto solution_found;
                }

                coord_cube_t *phase2_cube = get_coord_cube();
                copy_coord_cube(phase2_cube, cube_stack[pivot]);

                long    phase2_start    = get_microseconds();
                move_t *phase2_solution = solve_phase2(phase2_cube, max_depth - pivot - 1, 0);
                long    phase2_end      = get_microseconds();
                phase2_time += phase2_end - phase2_start;

                if (phase2_solution == NULL) {
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

                free(phase2_cube);

                if (max_solutions != -1 && solution_count >= max_solutions) {
                    goto solution_found;
                } else {
                    free(solution);
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

move_t *solve_phase2(coord_cube_t *cube, int max_depth, __attribute__((unused)) float timeout) {
    move_t *solution = NULL;
    move_t  moves[]  = {MOVE_U1, MOVE_U2, MOVE_U3, MOVE_D1, MOVE_D2, MOVE_D3, MOVE_R2, MOVE_L2, MOVE_F2, MOVE_B2};
    int     n_moves  = 10;

    move_t        move_stack[MAX_MOVES];
    coord_cube_t *cube_stack[MAX_MOVES];
    int           pruning_stack[MAX_MOVES];
    int           move_count = 0;

    for (int i = 0; i < MAX_MOVES; i++) {
        move_stack[i]    = -1;
        pruning_stack[i] = -1;
        cube_stack[i]    = get_coord_cube();
    }

    copy_coord_cube(cube_stack[0], cube);

    long start_time = get_microseconds();
    long end_time   = 0;

    for (int allowed_depth = 1; allowed_depth <= max_depth; allowed_depth++) {
        int pivot = 0;
        copy_coord_cube(cube_stack[0], cube);

        /*printf("searching with max depth: %d\n", allowed_depth);*/

        do {
            move_stack[pivot]++;

            if ((int)move_stack[pivot] >= n_moves) {
                pruning_stack[pivot] = -1; // ?
                move_stack[pivot]    = -1;
                /*printf("\n");*/
                pivot--;

                if (pivot < 0) {
                    break;
                } else if (pivot == 0) {
                    copy_coord_cube(cube_stack[0], cube);
                } else if (pivot > 0) {
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
