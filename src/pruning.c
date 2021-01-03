#include <assert.h>
#include <stdio.h>

#include "coord_cube.h"
#include "coord_move_tables.h"
#include "definitions.h"
#include "pruning.h"
#include "utils.h"

static int *pruning_phase1_corner = NULL;
static int *pruning_phase1_edge   = NULL;

void build_pruning_tables() {
    printf("building pruning tables\n\n");
    build_phase1_corner_table();
    build_phase1_edge_table();
    printf("finished pruning tables\n");
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
