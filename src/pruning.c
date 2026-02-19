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
