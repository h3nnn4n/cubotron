/*
 * Copyright <2026> <Renan S Silva, aka h3nnn4n>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
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
#include <string.h>

#include "cubie_move_table.h"
#include "definitions.h"
#include "pruning_cache.h"
#include "puzzle_2x2.h"
#include "solver_2x2_ida.h"
#include "stats.h"
#include "utils.h"

#define N_CORNER_ORIENTATION 2187
#define N_CORNER_PERMUTATION 40320
#define MAX_DEPTH            15
#define N_MOVES_2X2          9
#define MAX_THREADS          9

typedef struct {
    int corner_orientation;
    int corner_permutation;
} coord_t;

static const move_t moves[N_MOVES_2X2] = {MOVE_U1, MOVE_U2, MOVE_U3, MOVE_R1, MOVE_R2,
                                          MOVE_R3, MOVE_F1, MOVE_F2, MOVE_F3};

static int  corner_orientation_move_table[N_CORNER_ORIENTATION][N_MOVES_2X2];
static int  corner_permutation_move_table[N_CORNER_PERMUTATION][N_MOVES_2X2];
static int *corner_orientation_pruning = NULL;
static int *corner_permutation_pruning = NULL;
static int  tables_built               = 0;

// ---- coordinate encoding ----

static int encode_corner_orientation(const cube_2x2_t *cube) {
    int v = 0;

    for (int i = 0; i < 7; i++)
        v = 3 * v + cube->corner_orientations[i];

    return v;
}

static void decode_corner_orientation(cube_2x2_t *cube, int v) {
    int sum = 0;

    for (int i = 6; i >= 0; i--) {
        cube->corner_orientations[i] = v % 3;
        sum += v % 3;
        v /= 3;
    }

    cube->corner_orientations[7] = (3 - sum % 3) % 3;
}

static int encode_corner_permutation(const cube_2x2_t *cube) {
    corner_t perm[8];
    int      v = 0;

    for (int i = 0; i < 8; i++)
        perm[i] = cube->corner_permutations[i];

    for (int i = 7; i > 0; i--) {
        int k = 0;

        while (perm[i] != (corner_t)i) {
            rotate_left((int *)perm, 0, i);
            k++;
        }

        v = (i + 1) * v + k;
    }

    return v;
}

static void decode_corner_permutation(cube_2x2_t *cube, int v) {
    corner_t perm[8] = {URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB};

    for (int i = 1; i < 8; i++) {
        int k = v % (i + 1);

        v /= (i + 1);

        while (k > 0) {
            rotate_right((int *)perm, 0, i);
            k--;
        }
    }

    for (int i = 0; i < 8; i++)
        cube->corner_permutations[i] = perm[i];
}

// ---- move application on cube_2x2_t ----

static cube_2x2_t move_cubes[N_MOVES_2X2];

static const corner_t *face_corner_permutation[6] = {corner_permutation_U, corner_permutation_R, corner_permutation_F,
                                                     corner_permutation_D, corner_permutation_L, corner_permutation_B};

static const int *face_corner_orientation[6] = {corner_orientation_U, corner_orientation_R, corner_orientation_F,
                                                corner_orientation_D, corner_orientation_L, corner_orientation_B};

static void compose_cube(cube_2x2_t *dst, const cube_2x2_t *src) {
    cube_2x2_t result;

    for (int i = 0; i < 8; i++) {
        result.corner_permutations[i] = dst->corner_permutations[src->corner_permutations[i]];
        result.corner_orientations[i] =
            (dst->corner_orientations[src->corner_permutations[i]] + src->corner_orientations[i]) % 3;
    }

    *dst = result;
}

static void build_move_cubes(void) {
    int idx = 0;

    for (int face = 0; face < 3; face++) {
        cube_2x2_t base;

        for (int i = 0; i < 8; i++) {
            base.corner_permutations[i] = face_corner_permutation[face][i];
            base.corner_orientations[i] = face_corner_orientation[face][i];
        }

        cube_2x2_t cube;
        for (int turn = 0; turn < 3; turn++) {
            if (turn == 0) {
                cube = base;
            } else {
                compose_cube(&cube, &base);
            }

            move_cubes[idx] = cube;
            idx++;
        }
    }
}

// ---- move table building ----

static void build_corner_orientation_move_table(void) {
    cube_2x2_t cube;

    for (int state = 0; state < N_CORNER_ORIENTATION; state++) {
        decode_corner_orientation(&cube, state);

        for (int m = 0; m < N_MOVES_2X2; m++) {
            cube_2x2_t next = cube;
            compose_cube(&next, &move_cubes[m]);
            corner_orientation_move_table[state][m] = encode_corner_orientation(&next);
        }
    }
}

static void build_corner_permutation_move_table(void) {
    cube_2x2_t cube;

    for (int state = 0; state < N_CORNER_PERMUTATION; state++) {
        decode_corner_permutation(&cube, state);

        for (int m = 0; m < N_MOVES_2X2; m++) {
            cube_2x2_t next = cube;
            compose_cube(&next, &move_cubes[m]);
            corner_permutation_move_table[state][m] = encode_corner_permutation(&next);
        }
    }
}

// ---- pruning table building ----

static int *build_pruning_table(int n_states, int (*move_table)[N_MOVES_2X2]) {
    int *pruning = (int *)malloc(sizeof(int) * n_states);

    for (int i = 0; i < n_states; i++)
        pruning[i] = -1;

    int *queue = (int *)malloc(sizeof(int) * n_states);
    int  head  = 0;
    int  tail  = 0;

    pruning[0]    = 0;
    queue[tail++] = 0;

    while (head < tail) {
        int state = queue[head++];

        for (int m = 0; m < N_MOVES_2X2; m++) {
            int child = move_table[state][m];

            if (pruning[child] == -1) {
                pruning[child] = pruning[state] + 1;
                queue[tail++]  = child;
            }
        }
    }

    free(queue);
    return pruning;
}

// ---- IDA* search ----

typedef struct {
    coord_t cube_stack[MAX_DEPTH];
    int     move_stack[MAX_DEPTH];
    int     pruning_stack[MAX_DEPTH];

    coord_t initial;
    move_t  prep_move;
} solver_ctx_t;

typedef struct {
    solver_ctx_t  *ctx;
    solve_list_t  *solves;
    solve_stats_t *stats;
} thread_ctx_t;

static int is_duplicated_or_undoes_move_2x2(int move_idx, int prev_idx) {
    move_t m  = moves[move_idx];
    move_t pm = moves[prev_idx];

    if (m == pm)
        return 1;

    int face_m  = m / 3;
    int face_pm = pm / 3;

    if (face_m != face_pm)
        return 0;

    return 1;
}

static int is_solved(const coord_t *state) { return state->corner_orientation == 0 && state->corner_permutation == 0; }

static void search(solver_ctx_t *ctx, solve_list_t *solves, solve_stats_t *stats) {
    const config_t *config    = get_config();
    int             max_depth = config->max_depth;

    if (max_depth > MAX_DEPTH)
        max_depth = MAX_DEPTH;

    uint64_t start_time = get_microseconds();

    if (is_solved(&ctx->initial)) {
        move_t *solution = (move_t *)malloc(sizeof(move_t) * 2);
        solution[0]      = ctx->prep_move;
        solution[1]      = MOVE_NULL;

        stats->solution_length = ctx->prep_move != MOVE_NULL ? 1 : 0;
        stats->phase1_depth    = 1;

        solves->solution = solution;
        solves->stats    = stats;

        get_config()->die = true;
        return;
    }

    for (int allowed_depth = 1; allowed_depth <= max_depth; allowed_depth++) {
        int pivot = 0;

        ctx->cube_stack[0] = ctx->initial;

        do {
            if (get_config()->die) {
                finalize_solve_stats(stats, start_time, start_time, 0, 1);
                return;
            }

            do {
                ctx->move_stack[pivot]++;
            } while (ctx->move_stack[pivot] < N_MOVES_2X2 &&
                     config->move_black_list[moves[ctx->move_stack[pivot]]] != MOVE_NULL);

            if (ctx->move_stack[pivot] >= N_MOVES_2X2) {
                ctx->pruning_stack[pivot] = -1;
                ctx->move_stack[pivot]    = -1;
                pivot--;

                if (pivot < 0) {
                    break;
                } else if (pivot == 0) {
                    ctx->cube_stack[0] = ctx->initial;
                }

                continue;
            }

            if (pivot > 0 && is_duplicated_or_undoes_move_2x2(ctx->move_stack[pivot], ctx->move_stack[pivot - 1]))
                continue;

            coord_t *cur      = &ctx->cube_stack[pivot];
            int      move_idx = ctx->move_stack[pivot];

            cur->corner_orientation = corner_orientation_move_table[cur->corner_orientation][move_idx];
            cur->corner_permutation = corner_permutation_move_table[cur->corner_permutation][move_idx];

            int orientation_pruning = corner_orientation_pruning[cur->corner_orientation];
            int permutation_pruning = corner_permutation_pruning[cur->corner_permutation];
            ctx->pruning_stack[pivot] =
                orientation_pruning > permutation_pruning ? orientation_pruning : permutation_pruning;

            stats->total_moves++;

            if (is_solved(cur)) {
                uint64_t end_time = get_microseconds();
                stats->wall_time  = (float)(end_time - start_time) / 1000000.0f;

                int found_len          = pivot + 1;
                stats->phase1_depth    = found_len;
                stats->solution_length = found_len + (ctx->prep_move != MOVE_NULL ? 1 : 0);

                move_t *solution = (move_t *)malloc(sizeof(move_t) * (found_len + 2));
                int     idx      = 0;

                if (ctx->prep_move != MOVE_NULL)
                    solution[idx++] = ctx->prep_move;

                for (int i = 0; i < found_len; i++)
                    solution[idx++] = moves[ctx->move_stack[i]];

                solution[idx] = MOVE_NULL;

                solves->solution = solution;
                solves->stats    = stats;

                get_config()->die = true;
                return;
            }

            if (ctx->pruning_stack[pivot] + pivot < allowed_depth) {
                ctx->cube_stack[pivot + 1] = ctx->cube_stack[pivot];
                pivot++;
            } else {
                if (pivot > 0) {
                    ctx->cube_stack[pivot] = ctx->cube_stack[pivot - 1];
                } else {
                    ctx->cube_stack[pivot] = ctx->initial;
                }
            }
        } while (1);
    }

    finalize_solve_stats(stats, start_time, start_time, 0, 0);
}

static void *solve_thread(void *arg) {
    thread_ctx_t *tc = (thread_ctx_t *)arg;

    if (tc->ctx->prep_move != MOVE_NULL) {
        int move_idx = tc->ctx->prep_move == MOVE_U1   ? 0
                       : tc->ctx->prep_move == MOVE_U2 ? 1
                       : tc->ctx->prep_move == MOVE_U3 ? 2
                       : tc->ctx->prep_move == MOVE_R1 ? 3
                       : tc->ctx->prep_move == MOVE_R2 ? 4
                       : tc->ctx->prep_move == MOVE_R3 ? 5
                       : tc->ctx->prep_move == MOVE_F1 ? 6
                       : tc->ctx->prep_move == MOVE_F2 ? 7
                                                       : 8;

        tc->ctx->initial.corner_orientation =
            corner_orientation_move_table[tc->ctx->initial.corner_orientation][move_idx];
        tc->ctx->initial.corner_permutation =
            corner_permutation_move_table[tc->ctx->initial.corner_permutation][move_idx];
        tc->ctx->move_stack[0] = -1;
    }

    search(tc->ctx, tc->solves, tc->stats);

    return tc->solves;
}

static solve_list_t *solve(const puzzle_t *puzzle, const config_t *config) {
    const cube_2x2_t *cubie = (const cube_2x2_t *)puzzle->state;
    coord_t           initial;

    initial.corner_orientation = encode_corner_orientation(cubie);
    initial.corner_permutation = encode_corner_permutation(cubie);

    if (initial.corner_orientation == 0 && initial.corner_permutation == 0) {
        solve_list_t *trivial       = new_solve_list_node();
        trivial->solution           = (move_t *)malloc(sizeof(move_t));
        trivial->solution[0]        = MOVE_NULL;
        trivial->phase1_solution    = (move_t *)malloc(sizeof(move_t));
        trivial->phase1_solution[0] = MOVE_NULL;
        trivial->phase2_solution    = (move_t *)malloc(sizeof(move_t));
        trivial->phase2_solution[0] = MOVE_NULL;

        solve_stats_t *stats   = get_solve_stats();
        stats->solution_length = 0;
        trivial->stats         = stats;

        return trivial;
    }

    get_config()->die = false;
    atomic_store(&get_config()->solutions_found, 0);

    int            n_threads = N_MOVES_2X2 < config->thread_count ? N_MOVES_2X2 : config->thread_count;
    solver_ctx_t   contexts[MAX_THREADS];
    thread_ctx_t   thread_contexts[MAX_THREADS];
    solve_stats_t *all_stats[MAX_THREADS];
    pthread_t      threads[MAX_THREADS];

    for (int i = 0; i < n_threads; i++) {
        contexts[i].initial   = initial;
        contexts[i].prep_move = moves[i];

        for (int j = 0; j < MAX_DEPTH; j++) {
            contexts[i].move_stack[j]    = -1;
            contexts[i].pruning_stack[j] = -1;
        }

        thread_contexts[i].ctx    = &contexts[i];
        thread_contexts[i].solves = new_solve_list_node();
        thread_contexts[i].stats  = get_solve_stats();

        all_stats[i] = thread_contexts[i].stats;
    }

    for (int i = 0; i < n_threads; i++)
        pthread_create(&threads[i], NULL, solve_thread, &thread_contexts[i]);

    for (int i = 0; i < n_threads; i++)
        pthread_join(threads[i], NULL);

    solve_list_t *solves       = NULL;
    int           shortest_len = MAX_DEPTH + 1;
    int           winner_idx   = -1;

    for (int i = 0; i < n_threads; i++) {
        solve_list_t *ts = thread_contexts[i].solves;

        if (ts->solution != NULL && ts->solution[0] != MOVE_NULL) {
            int len = 0;
            while (ts->solution[len] != MOVE_NULL)
                len++;

            if (len < shortest_len) {
                shortest_len = len;
                winner_idx   = i;
            }
        }
    }

    int is_winner[MAX_THREADS];

    for (int i = 0; i < n_threads; i++) {
        is_winner[i] = 0;

        solve_list_t *ts = thread_contexts[i].solves;

        if (i == winner_idx) {
            is_winner[i] = 1;
            solves       = ts;
            solves->next = NULL;
        } else {
            ts->stats = NULL;
            destroy_solve_list(ts);
        }
    }

    if (solves != NULL) {
        int all_lengths[MAX_DEPTH] = {0};
        int n_lengths              = 0;

        for (const solve_list_t *n = solves; n != NULL && n->solution != NULL; n = n->next) {
            int len = 0;
            while (n->solution[len] != MOVE_NULL)
                len++;
            if (n_lengths < MAX_DEPTH)
                all_lengths[n_lengths++] = len;
        }

        solves->aggregate = compute_aggregate_stats(all_stats, n_threads, all_lengths, n_lengths);
    }

    for (int i = 0; i < n_threads; i++) {
        if (!is_winner[i])
            free(all_stats[i]);
    }

    return solves;
}

static void init(void) {
    if (tables_built)
        return;

    build_move_cubes();
    build_corner_orientation_move_table();
    build_corner_permutation_move_table();

    int loaded_orientation = pruning_table_cache_load("pruning_tables", "2x2_corner_orientation",
                                                      &corner_orientation_pruning, N_CORNER_ORIENTATION);
    int loaded_permutation = pruning_table_cache_load("pruning_tables", "2x2_corner_permutation",
                                                      &corner_permutation_pruning, N_CORNER_PERMUTATION);

    if (!loaded_orientation) {
        corner_orientation_pruning = build_pruning_table(N_CORNER_ORIENTATION, corner_orientation_move_table);
        pruning_table_cache_store("pruning_tables", "2x2_corner_orientation", corner_orientation_pruning,
                                  N_CORNER_ORIENTATION);
    }

    if (!loaded_permutation) {
        corner_permutation_pruning = build_pruning_table(N_CORNER_PERMUTATION, corner_permutation_move_table);
        pruning_table_cache_store("pruning_tables", "2x2_corner_permutation", corner_permutation_pruning,
                                  N_CORNER_PERMUTATION);
    }

    tables_built = 1;
}

static void cleanup(void) {
    if (corner_orientation_pruning != NULL) {
        free(corner_orientation_pruning);
        corner_orientation_pruning = NULL;
    }

    if (corner_permutation_pruning != NULL) {
        free(corner_permutation_pruning);
        corner_permutation_pruning = NULL;
    }

    tables_built = 0;
}

const solver_ops_t solver_2x2_ida_ops = {
    .name        = "ida-star",
    .puzzle_name = "2x2",

    .init    = init,
    .solve   = solve,
    .cleanup = cleanup,
};
