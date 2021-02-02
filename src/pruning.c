#include <assert.h>
#include <stdio.h>

#include "coord_cube.h"
#include "coord_move_tables.h"
#include "definitions.h"
#include "pruning.h"
#include "pruning_cache.h"
#include "utils.h"

static int *pruning_phase1_corner = NULL;
static int *pruning_phase1_edge   = NULL;
static int *pruning_phase2_corner = NULL;

void build_pruning_tables() {
    build_phase1_corner_table();
    build_phase1_edge_table();

    build_phase2_corner_table();
}

int get_phase1_pruning(coord_cube_t *cube) {
    assert(pruning_phase1_corner != NULL);
    assert(pruning_phase1_edge != NULL);

    int value1 = pruning_phase1_corner[cube->corner_orientations * N_SLICES + cube->E_slice];
    int value2 = pruning_phase1_edge[cube->edge_orientations * N_SLICES + cube->E_slice];

    assert(value1 >= 0);
    assert(value2 >= 0);

    assert(value1 < N_SLICES * N_CORNER_ORIENTATIONS);
    assert(value2 < N_SLICES * N_EDGE_ORIENTATIONS);

    // We want the bigger one
    return value1 > value2 ? value1 : value2;
}

int get_phase2_pruning(coord_cube_t *cube) {
    assert(pruning_phase2_corner != NULL);

    int index1 = (cube->corner_permutations * N_SORTED_SLICES_PHASE2 + cube->E_sorted_slice) * N_PARITY + cube->parity;

    assert(index1 >= 0);
    assert(index1 < N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2 * N_PARITY);

    int value1 = pruning_phase2_corner[index1];

    return value1;
}

void build_phase1_corner_table() {
    if (pruning_phase1_corner != NULL)
        return;

    if (pruning_table_cache_load("pruning_tables", "phase1_corner", &pruning_phase1_corner,
                                 N_CORNER_ORIENTATIONS * N_SLICES))
        return;

    printf("bulding phase1 corner orientations pruning table\n");

    long start_time       = get_microseconds();
    pruning_phase1_corner = (int *)malloc(sizeof(int) * N_CORNER_ORIENTATIONS * N_SLICES);

    for (int i = 0; i < N_CORNER_ORIENTATIONS * N_SLICES; i++)
        pruning_phase1_corner[i] = -1;

    // The solved phase1 cube has coord zero and can be solved in zero moves
    pruning_phase1_corner[0] = 0;

    int *slice_move_table               = get_move_table_E_slice();
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

    pruning_table_cache_store("pruning_tables", "phase1_corner", pruning_phase1_corner,
                              N_CORNER_ORIENTATIONS * N_SLICES);
}

void build_phase1_edge_table() {
    if (pruning_phase1_edge != NULL)
        return;

    if (pruning_table_cache_load("pruning_tables", "phase1_edge", &pruning_phase1_edge, N_EDGE_ORIENTATIONS * N_SLICES))
        return;

    printf("bulding phase1 edge orientations pruning table\n");

    long start_time     = get_microseconds();
    pruning_phase1_edge = (int *)malloc(sizeof(int) * N_EDGE_ORIENTATIONS * N_SLICES);

    for (int i = 0; i < N_EDGE_ORIENTATIONS * N_SLICES; i++)
        pruning_phase1_edge[i] = -1;

    // The solved phase1 cube has coord zero and can be solved in zero moves
    pruning_phase1_edge[0] = 0;

    int *slice_move_table             = get_move_table_E_slice();
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

    pruning_table_cache_store("pruning_tables", "phase1_edge", pruning_phase1_edge, N_EDGE_ORIENTATIONS * N_SLICES);
}

// FIXME: Seems a bit too low for my taste, even when considering just the space is used in phase2
void build_phase2_corner_table() {
    if (pruning_phase2_corner != NULL)
        return;

    if (pruning_table_cache_load("pruning_tables", "phase2_corner", &pruning_phase2_corner,
                                 N_CORNER_PERMUTATIONS * N_SORTED_SLICES * N_PARITY))
        return;

    printf("bulding phase2 corner orientations pruning table\n");

    long start_time       = get_microseconds();
    pruning_phase2_corner = (int *)malloc(sizeof(int) * N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2 * N_PARITY);

    for (int i = 0; i < N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2 * N_PARITY; i++)
        pruning_phase2_corner[i] = -1;

    // The solved phase2 cube has coord zero and can be solved in zero moves
    pruning_phase2_corner[0] = 0;

    int *parity_move_table              = get_move_table_parity();
    int *sorted_slice_move_table        = get_move_table_E_sorted_slice();
    int *corner_permutations_move_table = get_move_table_corner_permutations();

    /*int missing = N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2 * N_PARITY - 1;*/
    int missing = N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2 * 1 - 1;
    int depth   = 0;

    int depth_dist[20] = {0};

    move_t moves[] = {MOVE_U1, MOVE_U2, MOVE_U3, MOVE_R2, MOVE_F2, MOVE_D1, MOVE_D2, MOVE_D3, MOVE_L2, MOVE_B2};
    int    n_moves = 10;

    while (missing > 0) {
        for (int i = 0; i < N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2 * N_PARITY; i++) {
            if (pruning_phase2_corner[i] != depth)
                continue;

            int parity             = i % N_PARITY;
            int corner_permutation = (i / N_PARITY) / N_SORTED_SLICES_PHASE2;
            int sorted_slice       = (i / N_PARITY) % N_SORTED_SLICES_PHASE2;

            for (int move_index = 0; move_index < n_moves; move_index++) {
                int move = moves[move_index];

                int next_parity             = parity_move_table[parity * N_MOVES + move];
                int next_corner_permutation = corner_permutations_move_table[corner_permutation * N_MOVES + move];
                int next_sorted_slice       = sorted_slice_move_table[sorted_slice * N_MOVES + move];

                int index =
                    (next_corner_permutation * N_SORTED_SLICES_PHASE2 + next_sorted_slice) * N_PARITY + next_parity;

                if (pruning_phase2_corner[index] == -1) {
                    pruning_phase2_corner[index] = depth + 1;

                    missing--;
                    depth_dist[depth]++;
                }
            }
        }

        printf("finished depth %2d: %8d %8d %8d\n", depth, depth_dist[depth],
               (N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2 * N_PARITY) - missing, missing);
        assert(depth <= 13);
        depth++;
    }

    long end_time = get_microseconds();

    printf("elapsed time: %f seconds - ", (float)(end_time - start_time) / 1000000.0);
    printf("nodes per second : %.2f\n",
           ((float)(N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2 * N_PARITY) / (end_time - start_time)) * 1000000.0);
    printf("\n");

    pruning_table_cache_store("pruning_tables", "phase2_corner", pruning_phase2_corner,
                              N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2 * N_PARITY);
}
