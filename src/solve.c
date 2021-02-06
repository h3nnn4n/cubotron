#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "coord_move_tables.h"
#include "cubie_cube.h"
#include "pruning.h"
#include "solve.h"
#include "utils.h"

#define max_moves 20

move_t *solve(coord_cube_t *original_cube) {
    coord_cube_t *cube = get_coord_cube();
    copy_coord_cube(cube, original_cube);

    int     phase1_move_count = 0;
    int     phase2_move_count = 0;
    move_t *solution          = (move_t *)malloc(sizeof(move_t) * 42);

    /*printf("starting phase1\n");*/
    move_t *phase1_solution = solve_phase1(cube);
    /*printf("finished phase1\n");*/

    if (phase1_solution == NULL) {
        /*printf("failed to solve phase1\n");*/
        free(solution);
        return NULL;
    } else {
        /*printf("phase1 solution:\n");*/
        for (int i = 0; phase1_solution[i] != MOVE_NULL; i++) {
            /*printf(" %s", move_to_str(phase1_solution[i]));*/
            coord_apply_move(cube, phase1_solution[i]);
            solution[i] = phase1_solution[i];
            phase1_move_count++;
        }

        free(phase1_solution);

        // printf("\n coords: %4d %4d %3d %4d %4d %4d\n\n", cube->edge_orientations, cube->corner_orientations,
        // cube->E_slice, cube->E_sorted_slice, cube->UD6_edge_permutations, cube->corner_permutations);

        assert(is_phase1_solved(cube));
    }

    /*printf("starting phase2\n");*/
    move_t *phase2_solution = solve_phase2(cube);

    if (phase2_solution == NULL) {
        /*printf("failed to solve phase2\n");*/
        free(solution);
        return NULL;
    } else {
        /*printf("phase2 solution:\n");*/
        for (int i = 0; phase2_solution[i] != MOVE_NULL; i++) {
            /*printf(" %s", move_to_str(phase2_solution[i]));*/
            coord_apply_move(cube, phase2_solution[i]);
            solution[phase1_move_count + i] = phase2_solution[i];
            phase2_move_count++;
        }
        solution[phase1_move_count + phase2_move_count] = MOVE_NULL;

        free(phase2_solution);

        // printf("\n coords: %4d %4d %3d %4d %4d %4d\n\n", cube->edge_orientations, cube->corner_orientations,
        // cube->E_slice, cube->E_sorted_slice, cube->UD6_edge_permutations, cube->corner_permutations);

        assert(is_phase2_solved(cube));
    }
    /*printf("finished phase2\n");*/

    free(cube);

    return solution;
}

move_t *solve_phase1(coord_cube_t *cube) {
    move_t *solution = NULL;

    move_t            move_stack[max_moves];
    int               pruning_stack[max_moves];
    coord_cube_t *    cube_stack[max_moves];
    unsigned long int move_count = 0;

    int max_depth = 12;

    for (int i = 0; i < max_moves; i++) {
        move_stack[i]    = -1;
        pruning_stack[i] = -1;
        cube_stack[i]    = get_coord_cube();
    }

    /*long start_time    = get_microseconds();*/
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
                solution = malloc(sizeof(move_t) * (pivot + 2));

                for (int i = 0; i <= pivot; i++)
                    solution[i] = move_stack[i];
                solution[pivot + 1] = MOVE_NULL;

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

    for (int i = 0; i < max_moves; i++) {
        free(cube_stack[i]);
    }

    /*long end_time = get_microseconds();*/
    /*printf("elapsed time: %f seconds - ", (float)(end_time - start_time) / 1000000.0);*/
    /*printf("moves: %lu - ", move_count);*/
    /*printf("moves per second : %.2f\n", ((float)move_count / (end_time - start_time)) * 1000000.0);*/

    return solution;
}

move_t *solve_phase2(coord_cube_t *cube) {
    move_t *solution = NULL;
    move_t  moves[]  = {MOVE_U1, MOVE_U2, MOVE_U3, MOVE_D1, MOVE_D2, MOVE_D3, MOVE_R2, MOVE_L2, MOVE_F2, MOVE_B2};
    int     n_moves  = 10;

    move_t        move_stack[max_moves];
    coord_cube_t *cube_stack[max_moves];
    int           pruning_stack[max_moves];
    int           move_count = 0;
    int           max_depth  = 20;

    for (int i = 0; i < max_moves; i++) {
        move_stack[i]    = -1;
        pruning_stack[i] = -1;
        cube_stack[i]    = get_coord_cube();
    }

    copy_coord_cube(cube_stack[0], cube);

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

    for (int i = 0; i < max_moves; i++) {
        free(cube_stack[i]);
    }

    return solution;
}
