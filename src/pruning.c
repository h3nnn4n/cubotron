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
#include <string.h>

#include "coord_cube.h"
#include "coord_move_tables.h"
#include "definitions.h"
#include "pruning.h"
#include "pruning_cache.h"
#include "symmetry.h"
#include "utils.h"

/*
With UD6
18.18        solves per second
33214435.58  moves per second

With UD7
33.41        solves per second
42165660.86  moves per second
*/

#define USE_UD7

static int *pruning_phase1_edge     = NULL;
static int *pruning_phase1_corner   = NULL;
static int *pruning_phase1_combined = NULL;
static int *pruning_phase2_UD6_edge = NULL;
static int *pruning_phase2_UD7_edge = NULL;
static int *pruning_phase2_corner   = NULL;

static uint8_t *pruning_phase1_sym = NULL;
static uint8_t *pruning_phase2_sym = NULL;

#define PHASE1_SYM_SIZE (N_FLIPSLICE_CLASSES * N_CORNER_ORIENTATIONS)
#define PHASE2_SYM_SIZE (N_CORNER_CLASSES * N_UD7_PHASE2_PERMUTATIONS)

void build_pruning_tables() {
    build_phase1_sym_table();
    build_phase2_sym_table();
}

int get_phase1_pruning(const coord_cube_t *cube) {
    assert(pruning_phase1_sym != NULL);

    int flipslice = cube->E_slice * N_EDGE_ORIENTATIONS + cube->edge_orientations;
    int classidx  = get_flipslice_classidx()[flipslice];
    int sym       = get_flipslice_sym()[flipslice];
    int twist_adj = get_twist_conj()[cube->corner_orientations * N_SYMMETRIES_D4H + sym];
    int idx       = classidx * N_CORNER_ORIENTATIONS + twist_adj;

    int val = (int)pruning_phase1_sym[idx];
    assert(val != 255);
    return val;
}

int get_phase2_pruning(const coord_cube_t *cube) {
    assert(pruning_phase2_sym != NULL);
    assert(is_phase1_solved(cube));

    int classidx = get_corner_classidx()[cube->corner_permutations];
    int sym      = get_corner_sym()[cube->corner_permutations];
    int ud7_adj  = get_ud_edges_conj()[cube->UD7_edge_permutations * N_SYMMETRIES_D4H + sym];
    int idx      = classidx * N_UD7_PHASE2_PERMUTATIONS + ud7_adj;

    int val = (int)pruning_phase2_sym[idx];
    assert(val != 255);
    return val;
}

void build_phase1_sym_table(void) {
    if (pruning_phase1_sym != NULL)
        return;

    if (pruning_table_cache_load_u8("pruning_tables", "phase1_sym", &pruning_phase1_sym, PHASE1_SYM_SIZE))
        return;

    printf("building phase1 sym pruning table\n");

    uint64_t start_time = get_microseconds();
    pruning_phase1_sym  = (uint8_t *)malloc(PHASE1_SYM_SIZE);
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    memset(pruning_phase1_sym, 255, PHASE1_SYM_SIZE);

    pruning_phase1_sym[0] = 0;

    const int      *flip_table  = get_move_table_edge_orientations();
    const int      *slice_table = get_move_table_E_slice();
    const int      *twist_table = get_move_table_corner_orientations();
    const uint16_t *fs_classidx = get_flipslice_classidx();
    const uint8_t  *fs_sym      = get_flipslice_sym();
    const uint32_t *fs_rep      = get_flipslice_rep();
    const uint16_t *tc          = get_twist_conj();

    int missing        = PHASE1_SYM_SIZE - 1;
    int depth          = 0;
    int depth_dist[20] = {0};

    while (missing > 0) {
        for (int idx = 0; idx < PHASE1_SYM_SIZE; idx++) {
            if (pruning_phase1_sym[idx] != (uint8_t)depth)
                continue;

            int classidx = idx / N_CORNER_ORIENTATIONS;
            int twist    = idx % N_CORNER_ORIENTATIONS;

            int rep   = (int)fs_rep[classidx];
            int flip  = rep % N_EDGE_ORIENTATIONS;
            int slice = rep / N_EDGE_ORIENTATIONS;

            for (int m = 0; m < N_MOVES; m++) {
                int next_flip  = flip_table[flip * N_MOVES + m];
                int next_slice = slice_table[slice * N_MOVES + m];
                int next_twist = twist_table[twist * N_MOVES + m];

                int next_fs        = next_slice * N_EDGE_ORIENTATIONS + next_flip;
                int next_classidx  = fs_classidx[next_fs];
                int next_sym       = fs_sym[next_fs];
                int next_twist_adj = tc[next_twist * N_SYMMETRIES_D4H + next_sym];
                int next_idx       = next_classidx * N_CORNER_ORIENTATIONS + next_twist_adj;

                if (pruning_phase1_sym[next_idx] == 255) {
                    pruning_phase1_sym[next_idx] = (uint8_t)(depth + 1);
                    missing--;
                    depth_dist[depth]++;
                }
            }
        }

        printf("finished depth %2d: %8d %8d\n", depth, depth_dist[depth], PHASE1_SYM_SIZE - missing);
        assert(depth <= 12);
        depth++;
    }

    uint64_t end_time = get_microseconds();
    printf("elapsed time: %f seconds\n", (float)(end_time - start_time) / 1000000.0);
    printf("\n");

    for (int idx = 0; idx < PHASE1_SYM_SIZE; idx++) {
        if (pruning_phase1_sym[idx] == 255) {
            printf("phase1 sym pruning is not correctly populated!\n");
            abort();
        }
    }

    pruning_table_cache_store_u8("pruning_tables", "phase1_sym", pruning_phase1_sym, PHASE1_SYM_SIZE);
}

void build_phase2_sym_table(void) {
    if (pruning_phase2_sym != NULL)
        return;

    if (pruning_table_cache_load_u8("pruning_tables", "phase2_sym", &pruning_phase2_sym, PHASE2_SYM_SIZE))
        return;

    printf("building phase2 sym pruning table\n");

    uint64_t start_time = get_microseconds();
    pruning_phase2_sym  = (uint8_t *)malloc(PHASE2_SYM_SIZE);
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    memset(pruning_phase2_sym, 255, PHASE2_SYM_SIZE);

    pruning_phase2_sym[0] = 0;

    const int      *cp_table    = get_move_table_corner_permutations();
    const int      *ud7_table   = get_move_table_UD7_edge_permutations();
    const uint16_t *cr_classidx = get_corner_classidx();
    const uint8_t  *cr_sym      = get_corner_sym();
    const uint16_t *cr_rep      = get_corner_rep();
    const uint16_t *uc          = get_ud_edges_conj();

    move_t moves[] = {MOVE_U1, MOVE_U2, MOVE_U3, MOVE_R2, MOVE_F2, MOVE_D1, MOVE_D2, MOVE_D3, MOVE_L2, MOVE_B2};
    int    n_moves = 10;

    int missing        = PHASE2_SYM_SIZE - 1;
    int depth          = 0;
    int depth_dist[20] = {0};

    while (missing > 0) {
        for (int idx = 0; idx < PHASE2_SYM_SIZE; idx++) {
            if (pruning_phase2_sym[idx] != (uint8_t)depth)
                continue;

            int classidx = idx / N_UD7_PHASE2_PERMUTATIONS;
            int ud7      = idx % N_UD7_PHASE2_PERMUTATIONS;
            int cp       = (int)cr_rep[classidx];

            for (int mi = 0; mi < n_moves; mi++) {
                int m = (int)moves[mi];

                int next_cp  = cp_table[cp * N_MOVES + m];
                int next_ud7 = ud7_table[ud7 * N_MOVES + m];

                int next_classidx = cr_classidx[next_cp];
                int next_sym      = cr_sym[next_cp];
                int next_ud7_adj  = uc[next_ud7 * N_SYMMETRIES_D4H + next_sym];
                int next_idx      = next_classidx * N_UD7_PHASE2_PERMUTATIONS + next_ud7_adj;

                if (pruning_phase2_sym[next_idx] == 255) {
                    pruning_phase2_sym[next_idx] = (uint8_t)(depth + 1);
                    missing--;
                    depth_dist[depth]++;
                }
            }
        }

        printf("finished depth %2d: %8d %8d\n", depth, depth_dist[depth], PHASE2_SYM_SIZE - missing);
        assert(depth <= 18);
        depth++;
    }

    uint64_t end_time = get_microseconds();
    printf("elapsed time: %f seconds\n", (float)(end_time - start_time) / 1000000.0);
    printf("\n");

    for (int idx = 0; idx < PHASE2_SYM_SIZE; idx++) {
        if (pruning_phase2_sym[idx] == 255) {
            printf("phase2 sym pruning is not correctly populated!\n");
            abort();
        }
    }

    pruning_table_cache_store_u8("pruning_tables", "phase2_sym", pruning_phase2_sym, PHASE2_SYM_SIZE);
}

void build_phase1_corner_table() {
    if (pruning_phase1_corner != NULL)
        return;

    if (pruning_table_cache_load("pruning_tables", "phase1_corner", &pruning_phase1_corner,
                                 N_CORNER_ORIENTATIONS * N_SLICES))
        return;

    printf("bulding phase1 corner orientations pruning table\n");

    uint64_t start_time   = get_microseconds();
    pruning_phase1_corner = (int *)malloc(sizeof(int) * N_CORNER_ORIENTATIONS * N_SLICES);

    for (int i = 0; i < N_CORNER_ORIENTATIONS * N_SLICES; i++)
        pruning_phase1_corner[i] = -1;

    // The solved phase1 cube has coord zero and can be solved in zero moves
    pruning_phase1_corner[0] = 0;

    const int *slice_move_table               = get_move_table_E_slice();
    const int *corner_orientations_move_table = get_move_table_corner_orientations();

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

    uint64_t end_time = get_microseconds();

    printf("elapsed time: %f seconds - ", (float)(end_time - start_time) / 1000000.0);
    printf("nodes per second : %.2f\n",
           ((float)(N_CORNER_ORIENTATIONS * N_SLICES) / (end_time - start_time)) * 1000000.0);
    printf("\n");

    for (int i = 0; i < N_CORNER_ORIENTATIONS * N_SLICES; i++) {
        if (pruning_phase1_corner[i] == -1) {
            printf("phase1 corner pruning is not correctly populated!\n");
            abort();
        }
    }

    pruning_table_cache_store("pruning_tables", "phase1_corner", pruning_phase1_corner,
                              N_CORNER_ORIENTATIONS * N_SLICES);
}

void build_phase1_edge_table() {
    if (pruning_phase1_edge != NULL)
        return;

    if (pruning_table_cache_load("pruning_tables", "phase1_edge", &pruning_phase1_edge, N_EDGE_ORIENTATIONS * N_SLICES))
        return;

    printf("bulding phase1 edge orientations pruning table\n");

    uint64_t start_time = get_microseconds();
    pruning_phase1_edge = (int *)malloc(sizeof(int) * N_EDGE_ORIENTATIONS * N_SLICES);

    for (int i = 0; i < N_EDGE_ORIENTATIONS * N_SLICES; i++)
        pruning_phase1_edge[i] = -1;

    // The solved phase1 cube has coord zero and can be solved in zero moves
    pruning_phase1_edge[0] = 0;

    const int *slice_move_table             = get_move_table_E_slice();
    const int *edge_orientations_move_table = get_move_table_edge_orientations();

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

    uint64_t end_time = get_microseconds();

    printf("elapsed time: %f seconds - ", (float)(end_time - start_time) / 1000000.0);
    printf("nodes per second : %.2f\n",
           ((float)(N_EDGE_ORIENTATIONS * N_SLICES) / (end_time - start_time)) * 1000000.0);
    printf("\n");

    for (int i = 0; i < N_EDGE_ORIENTATIONS * N_SLICES; i++) {
        if (pruning_phase1_edge[i] == -1) {
            printf("phase1 edge pruning is not correctly populated!\n");
            abort();
        }
    }

    pruning_table_cache_store("pruning_tables", "phase1_edge", pruning_phase1_edge, N_EDGE_ORIENTATIONS * N_SLICES);
}

void build_phase1_combined_table() {
    if (pruning_phase1_combined != NULL)
        return;

    if (pruning_table_cache_load("pruning_tables", "phase1_combined", &pruning_phase1_combined,
                                 N_CORNER_ORIENTATIONS * N_EDGE_ORIENTATIONS))
        return;

    printf("bulding phase1 combined corner/edge orientations pruning table\n");

    uint64_t start_time     = get_microseconds();
    pruning_phase1_combined = (int *)malloc(sizeof(int) * N_CORNER_ORIENTATIONS * N_EDGE_ORIENTATIONS);

    for (int i = 0; i < N_CORNER_ORIENTATIONS * N_EDGE_ORIENTATIONS; i++)
        pruning_phase1_combined[i] = -1;

    pruning_phase1_combined[0] = 0;

    const int *corner_orientations_move_table = get_move_table_corner_orientations();
    const int *edge_orientations_move_table   = get_move_table_edge_orientations();

    int missing = N_CORNER_ORIENTATIONS * N_EDGE_ORIENTATIONS - 1;
    int depth   = 0;

    int depth_dist[20] = {0};

    while (missing > 0) {
        for (int i = 0; i < N_CORNER_ORIENTATIONS * N_EDGE_ORIENTATIONS; i++) {
            if (pruning_phase1_combined[i] != depth)
                continue;

            int corner_orientations = i / N_EDGE_ORIENTATIONS;
            int edge_orientations   = i % N_EDGE_ORIENTATIONS;

            for (int move = 0; move < N_MOVES; move++) {
                int next_corner_orientations = corner_orientations_move_table[corner_orientations * N_MOVES + move];
                int next_edge_orientations   = edge_orientations_move_table[edge_orientations * N_MOVES + move];
                int next_index               = next_corner_orientations * N_EDGE_ORIENTATIONS + next_edge_orientations;

                if (pruning_phase1_combined[next_index] == -1) {
                    pruning_phase1_combined[next_index] = depth + 1;
                    missing--;
                    depth_dist[depth]++;
                }
            }
        }

        printf("finished depth %2d: %8d %8d\n", depth, depth_dist[depth],
               (N_CORNER_ORIENTATIONS * N_EDGE_ORIENTATIONS) - missing);

        assert(depth <= 13);
        depth++;
    }

    uint64_t end_time = get_microseconds();

    printf("elapsed time: %f seconds - ", (float)(end_time - start_time) / 1000000.0);
    printf("nodes per second : %.2f\n",
           ((float)(N_CORNER_ORIENTATIONS * N_EDGE_ORIENTATIONS) / (end_time - start_time)) * 1000000.0);
    printf("\n");

    for (int i = 0; i < N_CORNER_ORIENTATIONS * N_EDGE_ORIENTATIONS; i++) {
        if (pruning_phase1_combined[i] == -1) {
            printf("phase1 combined pruning is not correctly populated!\n");
            abort();
        }
    }

    pruning_table_cache_store("pruning_tables", "phase1_combined", pruning_phase1_combined,
                              N_CORNER_ORIENTATIONS * N_EDGE_ORIENTATIONS);
}

void build_phase2_UD6_edge_table() {
    if (pruning_phase2_UD6_edge != NULL)
        return;

    if (pruning_table_cache_load("pruning_tables", "phase2_UD6_edge", &pruning_phase2_UD6_edge,
                                 N_UD6_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2))
        return;

    printf("bulding phase2 UD6_edge permutations pruning table\n");

    uint64_t start_time     = get_microseconds();
    pruning_phase2_UD6_edge = (int *)malloc(sizeof(int) * N_UD6_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2);

    for (int i = 0; i < N_UD6_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2; i++)
        pruning_phase2_UD6_edge[i] = -1;

    // The solved phase2 cube has coord zero and can be solved in zero moves
    pruning_phase2_UD6_edge[0] = 0;

    const int *sorted_slice_move_table          = get_move_table_E_sorted_slice();
    const int *UD6_edge_permutations_move_table = get_move_table_UD6_edge_permutations();

    int missing = N_UD6_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2 - 1;
    int depth   = 0;

    int depth_dist[20] = {0};

    move_t moves[] = {MOVE_U1, MOVE_U2, MOVE_U3, MOVE_R2, MOVE_F2, MOVE_D1, MOVE_D2, MOVE_D3, MOVE_L2, MOVE_B2};
    int    n_moves = 10;

    while (missing > 0) {
        for (int i = 0; i < N_UD6_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2; i++) {
            if (pruning_phase2_UD6_edge[i] != depth)
                continue;

            int UD6_edge_permutation = i / N_SORTED_SLICES_PHASE2;
            int sorted_slice         = i % N_SORTED_SLICES_PHASE2;

            for (int move_index = 0; move_index < n_moves; move_index++) {
                int move = moves[move_index];

                int next_UD6_edge_permutation = UD6_edge_permutations_move_table[UD6_edge_permutation * N_MOVES + move];
                int next_sorted_slice         = sorted_slice_move_table[sorted_slice * N_MOVES + move];

                int index = next_UD6_edge_permutation * N_SORTED_SLICES_PHASE2 + next_sorted_slice;

                assert(index >= 0);
                assert(index < N_UD6_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2);

                if (pruning_phase2_UD6_edge[index] == -1) {
                    pruning_phase2_UD6_edge[index] = depth + 1;

                    missing--;
                    depth_dist[depth]++;
                }
            }
        }

        printf("finished depth %2d: %8d %8d %8d\n", depth, depth_dist[depth],
               (N_UD6_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2) - missing, missing);
        assert(depth <= 9);
        depth++;
    }

    uint64_t end_time = get_microseconds();

    printf("elapsed time: %f seconds - ", (float)(end_time - start_time) / 1000000.0);
    printf("nodes per second : %.2f\n",
           ((float)(N_UD6_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2) / (end_time - start_time)) * 1000000.0);
    printf("\n");

    for (int i = 0; i < N_UD6_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2; i++) {
        if (pruning_phase2_UD6_edge[i] == -1) {
            printf("phase2 edge pruning is not correctly populated!\n");
            abort();
        }
    }

    pruning_table_cache_store("pruning_tables", "phase2_UD6_edge", pruning_phase2_UD6_edge,
                              N_UD6_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2);
}

void build_phase2_UD7_edge_table() {
    if (pruning_phase2_UD7_edge != NULL)
        return;

    if (pruning_table_cache_load("pruning_tables", "phase2_UD7_edge", &pruning_phase2_UD7_edge,
                                 N_UD7_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2))
        return;

    printf("bulding phase2 UD7_edge permutations pruning table\n");

    uint64_t start_time     = get_microseconds();
    pruning_phase2_UD7_edge = (int *)malloc(sizeof(int) * N_UD7_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2);

    for (int i = 0; i < N_UD7_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2; i++)
        pruning_phase2_UD7_edge[i] = -1;

    // The solved phase2 cube has coord zero and can be solved in zero moves
    pruning_phase2_UD7_edge[0] = 0;

    const int *sorted_slice_move_table          = get_move_table_E_sorted_slice();
    const int *UD7_edge_permutations_move_table = get_move_table_UD7_edge_permutations();

    int missing = N_UD7_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2 - 1;
    int depth   = 0;

    int depth_dist[20] = {0};

    move_t moves[] = {MOVE_U1, MOVE_U2, MOVE_U3, MOVE_R2, MOVE_F2, MOVE_D1, MOVE_D2, MOVE_D3, MOVE_L2, MOVE_B2};
    int    n_moves = 10;

    while (missing > 0) {
        for (int i = 0; i < N_UD7_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2; i++) {
            if (pruning_phase2_UD7_edge[i] != depth)
                continue;

            int UD7_edge_permutation = i / N_SORTED_SLICES_PHASE2;
            int sorted_slice         = i % N_SORTED_SLICES_PHASE2;

            for (int move_index = 0; move_index < n_moves; move_index++) {
                int move = moves[move_index];

                int next_UD7_edge_permutation = UD7_edge_permutations_move_table[UD7_edge_permutation * N_MOVES + move];
                int next_sorted_slice         = sorted_slice_move_table[sorted_slice * N_MOVES + move];

                int index = next_UD7_edge_permutation * N_SORTED_SLICES_PHASE2 + next_sorted_slice;

                assert(index >= 0);
                assert(index < N_UD7_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2);

                if (pruning_phase2_UD7_edge[index] == -1) {
                    pruning_phase2_UD7_edge[index] = depth + 1;

                    missing--;
                    depth_dist[depth]++;
                }
            }
        }

        printf("finished depth %2d: %8d %8d %8d\n", depth, depth_dist[depth],
               (N_UD7_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2) - missing, missing);
        assert(depth <= 11);
        depth++;
    }

    uint64_t end_time = get_microseconds();

    printf("elapsed time: %f seconds - ", (float)(end_time - start_time) / 1000000.0);
    printf("nodes per second : %.2f\n",
           ((float)(N_UD7_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2) / (end_time - start_time)) * 1000000.0);
    printf("\n");

    for (int i = 0; i < N_UD7_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2; i++) {
        if (pruning_phase2_UD7_edge[i] == -1) {
            printf("phase2 edge pruning is not correctly populated!\n");
            abort();
        }
    }

    pruning_table_cache_store("pruning_tables", "phase2_UD7_edge", pruning_phase2_UD7_edge,
                              N_UD7_PHASE2_PERMUTATIONS * N_SORTED_SLICES_PHASE2);
}

void build_phase2_corner_table() {
    if (pruning_phase2_corner != NULL)
        return;

    if (pruning_table_cache_load("pruning_tables", "phase2_corner", &pruning_phase2_corner,
                                 N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2))
        return;

    printf("bulding phase2 corner orientations pruning table\n");

    uint64_t start_time   = get_microseconds();
    pruning_phase2_corner = (int *)malloc(sizeof(int) * N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2);

    for (int i = 0; i < N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2; i++)
        pruning_phase2_corner[i] = -1;

    // The solved phase2 cube has coord zero and can be solved in zero moves
    pruning_phase2_corner[0] = 0;

    const int *sorted_slice_move_table        = get_move_table_E_sorted_slice();
    const int *corner_permutations_move_table = get_move_table_corner_permutations();

    int missing = N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2 - 1;
    int depth   = 0;

    int depth_dist[20] = {0};

    move_t moves[] = {MOVE_U1, MOVE_U2, MOVE_U3, MOVE_R2, MOVE_F2, MOVE_D1, MOVE_D2, MOVE_D3, MOVE_L2, MOVE_B2};
    int    n_moves = 10;

    while (missing > 0) {
        for (int i = 0; i < N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2; i++) {
            if (pruning_phase2_corner[i] != depth)
                continue;

            int corner_permutation = i / N_SORTED_SLICES_PHASE2;
            int sorted_slice       = i % N_SORTED_SLICES_PHASE2;

            for (int move_index = 0; move_index < n_moves; move_index++) {
                int move = moves[move_index];

                int next_corner_permutation = corner_permutations_move_table[corner_permutation * N_MOVES + move];
                int next_sorted_slice       = sorted_slice_move_table[sorted_slice * N_MOVES + move];

                int index = next_corner_permutation * N_SORTED_SLICES_PHASE2 + next_sorted_slice;

                assert(index >= 0);
                assert(index < N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2);

                if (pruning_phase2_corner[index] == -1) {
                    pruning_phase2_corner[index] = depth + 1;

                    missing--;
                    depth_dist[depth]++;
                }
            }
        }

        printf("finished depth %2d: %8d %8d %8d\n", depth, depth_dist[depth],
               (N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2) - missing, missing);
        assert(depth <= 13);
        depth++;
    }

    uint64_t end_time = get_microseconds();

    printf("elapsed time: %f seconds - ", (float)(end_time - start_time) / 1000000.0);
    printf("nodes per second : %.2f\n",
           ((float)(N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2) / (end_time - start_time)) * 1000000.0);
    printf("\n");

    for (int i = 0; i < N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2; i++) {
        if (pruning_phase2_corner[i] == -1) {
            printf("phase2 corner pruning is not correctly populated!\n");
            abort();
        }
    }

    pruning_table_cache_store("pruning_tables", "phase2_corner", pruning_phase2_corner,
                              N_CORNER_PERMUTATIONS * N_SORTED_SLICES_PHASE2);
}
