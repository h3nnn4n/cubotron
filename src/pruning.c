#include <assert.h>
#include <stdio.h>

#include "coord_cube.h"
#include "coord_move_tables.h"
#include "definitions.h"
#include "pruning.h"
#include "utils.h"

static int *pruning_phase1_corner = NULL;
static int *pruning_phase1_edge   = NULL;
static int *pruning_phase2_corner = NULL;

void build_pruning_tables() {
    build_phase1_corner_table();
    build_phase1_edge_table();

    /*build_phase2_corner_table();*/
}

int get_phase1_pruning(coord_cube_t *cube) {
    assert(pruning_phase1_corner != NULL);
    assert(pruning_phase1_edge != NULL);

    int value1 = pruning_phase1_corner[cube->corner_orientations * N_SLICES + cube->UD_slice];
    int value2 = pruning_phase1_edge[cube->edge_orientations * N_SLICES + cube->UD_slice];

    // We want the bigger one
    return value1 > value2 ? value1 : value2;
}

void build_phase1_corner_table() {
    if (pruning_phase1_corner != NULL)
        return;

    printf("bulding phase1 corner orientations pruning table\n");

    long start_time       = get_microseconds();
    pruning_phase1_corner = (int *)malloc(sizeof(int) * N_CORNER_ORIENTATIONS * N_SLICES);

    for (int i = 0; i < N_CORNER_ORIENTATIONS * N_SLICES; i++)
        pruning_phase1_corner[i] = -1;

    // The solved phase1 cube has coord zero and can be solved in zero moves
    pruning_phase1_corner[0] = 0;

    int *slice_move_table               = get_move_table_UD_slice();
    int *corner_orientations_move_table = get_move_table_corner_orientations();

    int missing = N_CORNER_ORIENTATIONS * N_SLICES - 1;
    int depth   = 0;

    int depth_dist[20] = {0};

    while (missing > 0) {
        for (int i = 0; i < N_CORNER_ORIENTATIONS * N_SLICES; i++) {
            if (pruning_phase1_corner[i] == depth) {
                int slice               = i % N_SLICES;
                int corner_orientations = i / N_SLICES;

                for (int move = 0; move < N_MOVES; move++) {
                    int next_slice               = slice_move_table[slice * N_MOVES + move];
                    int next_corner_orientations = corner_orientations_move_table[corner_orientations * N_MOVES + move];

                    if (pruning_phase1_corner[next_corner_orientations * N_SLICES + next_slice] == -1) {
                        pruning_phase1_corner[next_corner_orientations * N_SLICES + next_slice] = depth + 1;
                        missing--;
                        depth_dist[depth]++;
                    }
                }
            }
        }
        printf("finished depth %2d: %8d %8d\n", depth, depth_dist[depth], (N_CORNER_ORIENTATIONS * N_SLICES) - missing);

        assert(depth <= 8);
        depth++;
    }

    long end_time = get_microseconds();

    printf("elapsed time: %f seconds - ", (float)(end_time - start_time) / 1000000.0);
    printf("nodes per second : %.2f\n",
           ((float)(N_CORNER_ORIENTATIONS * N_SLICES) / (end_time - start_time)) * 1000000.0);
    printf("\n");
}

void build_phase1_edge_table() {
    if (pruning_phase1_edge != NULL)
        return;

    printf("bulding phase1 edge orientations pruning table\n");

    long start_time     = get_microseconds();
    pruning_phase1_edge = (int *)malloc(sizeof(int) * N_EDGE_ORIENTATIONS * N_SLICES);

    for (int i = 0; i < N_EDGE_ORIENTATIONS * N_SLICES; i++)
        pruning_phase1_edge[i] = -1;

    // The solved phase1 cube has coord zero and can be solved in zero moves
    pruning_phase1_edge[0] = 0;

    int *slice_move_table             = get_move_table_UD_slice();
    int *edge_orientations_move_table = get_move_table_edge_orientations();

    int missing = N_EDGE_ORIENTATIONS * N_SLICES - 1;
    int depth   = 0;

    int depth_dist[20] = {0};

    while (missing > 0) {
        for (int i = 0; i < N_EDGE_ORIENTATIONS * N_SLICES; i++) {
            if (pruning_phase1_edge[i] == depth) {
                int slice             = i % N_SLICES;
                int edge_orientations = i / N_SLICES;

                for (int move = 0; move < N_MOVES; move++) {
                    int next_slice             = slice_move_table[slice * N_MOVES + move];
                    int next_edge_orientations = edge_orientations_move_table[edge_orientations * N_MOVES + move];

                    if (pruning_phase1_edge[next_edge_orientations * N_SLICES + next_slice] == -1) {
                        pruning_phase1_edge[next_edge_orientations * N_SLICES + next_slice] = depth + 1;
                        missing--;
                        depth_dist[depth]++;
                    }
                }
            }
        }
        printf("finished depth %2d: %8d %8d\n", depth, depth_dist[depth], (N_EDGE_ORIENTATIONS * N_SLICES) - missing);

        assert(depth <= 12);
        depth++;
    }

    long end_time = get_microseconds();

    printf("elapsed time: %f seconds - ", (float)(end_time - start_time) / 1000000.0);
    printf("nodes per second : %.2f\n",
           ((float)(N_EDGE_ORIENTATIONS * N_SLICES) / (end_time - start_time)) * 1000000.0);
    printf("\n");
}

void build_phase2_corner_table() {
    if (pruning_phase2_corner != NULL)
        return;

    printf("bulding phase2 corner orientations pruning table\n");

    long start_time       = get_microseconds();
    pruning_phase2_corner = (int *)malloc(sizeof(int) * N_SORTED_SLICES * N_SORTED_SLICES_PHASE2 * N_PARITY);

    for (int i = 0; i < N_SORTED_SLICES * N_SORTED_SLICES_PHASE2 * N_PARITY; i++)
        pruning_phase2_corner[i] = -1;

    // The solved phase2 cube has coord zero and can be solved in zero moves
    pruning_phase2_corner[0] = 0;

    int *parity_move_table              = get_move_table_parity();
    int *sorted_slice_move_table        = get_move_table_UD_sorted_slice();
    int *corner_permutations_move_table = get_move_table_corner_permutations();

    int missing = N_SORTED_SLICES * N_SORTED_SLICES_PHASE2 * N_PARITY - 1;
    int depth   = 0;

    int depth_dist[20] = {0};

    move_t moves[] = {MOVE_U1, MOVE_U2, MOVE_U3, MOVE_R2, MOVE_F2, MOVE_D1, MOVE_D2, MOVE_D3, MOVE_L2, MOVE_B2};
    int    n_moves = 10;

    printf("finished depth %2d: %9d %9d %9d\n", depth, depth_dist[depth],
           (N_SORTED_SLICES * N_SORTED_SLICES_PHASE2 * N_PARITY) - missing, missing);

    while (missing > 0) {
        for (int i = 0; i < N_SORTED_SLICES * N_SORTED_SLICES_PHASE2 * N_PARITY; i++) {
            if (pruning_phase2_corner[i] != depth)
                continue;

            int parity             = i % N_PARITY;
            int corner_permutation = (i / N_PARITY) / N_SORTED_SLICES_PHASE2;
            int sorted_slice       = (i / N_PARITY) % N_SORTED_SLICES_PHASE2;

            if (depth == 0) {
                printf("%2d %8d %8d %2d :\n", i, sorted_slice, corner_permutation, parity);
            }

            for (int move_index = 0; move_index < n_moves; move_index++) {
                int move = moves[move_index];

                int next_parity             = parity_move_table[parity * N_MOVES + move];
                int next_corner_permutation = corner_permutations_move_table[corner_permutation * N_MOVES + move];
                int next_sorted_slice       = sorted_slice_move_table[sorted_slice * N_MOVES + move];

                if (depth == 0) {
                    printf("%2d %8d %8d %2d\n", move, next_sorted_slice, next_corner_permutation, next_parity);
                }

                int index =
                    (next_corner_permutation * N_SORTED_SLICES_PHASE2 + next_sorted_slice) * N_PARITY + next_parity;

                if (pruning_phase2_corner[index] == -1) {
                    pruning_phase2_corner[index] = depth + 1;
                    missing--;
                    depth_dist[depth]++;
                }
            }
        }

        /*printf("finished depth %2d: %8d %8d %8d\n", depth, depth_dist[depth],*/
        /*(N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2 * N_PARITY) - missing, missing);*/
        printf("finished depth %2d: %9d %9d %9d - ", depth, depth_dist[depth],
               (N_SORTED_SLICES * N_SORTED_SLICES_PHASE2 * N_PARITY) - missing, missing);

        long end_time = get_microseconds();
        printf("elapsed time: %f seconds - ", (float)(end_time - start_time) / 1000000.0);
        printf("nodes per second : %3.2f\n",
               ((float)(N_SORTED_SLICES * N_SORTED_SLICES_PHASE2 * N_PARITY) / (end_time - start_time)) * 1000000.0);

        assert(depth <= 20);
        if (depth > 20)
            abort();
        depth++;
    }

    long end_time = get_microseconds();

    printf("elapsed time: %f seconds - ", (float)(end_time - start_time) / 1000000.0);
    printf("nodes per second : %.2f\n",
           ((float)(N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2 * N_PARITY) / (end_time - start_time)) * 1000000.0);
    printf("\n");
}
