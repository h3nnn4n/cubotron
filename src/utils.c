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

#if !defined(__APPLE__)
#define _XOPEN_SOURCE 500
#endif

#include <assert.h>
#include <ftw.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <pcg_variants.h>

#include "coord_cube.h"
#include "coord_move_tables.h"
#include "cubie_cube.h"
#include "cubie_move_table.h"
#include "utils.h"

static char *_move_t_to_str_enum[] = {
    "MOVE_U1", "MOVE_U2", "MOVE_U3", "MOVE_R1", "MOVE_R2", "MOVE_R3", "MOVE_F1", "MOVE_F2", "MOVE_F3",   "MOVE_D1",
    "MOVE_D2", "MOVE_D3", "MOVE_L1", "MOVE_L2", "MOVE_L3", "MOVE_B1", "MOVE_B2", "MOVE_B3", "MOVE_NULL",
};

static char *_move_t_to_str[] = {
    "U ", "U2", "U'", "R ", "R2", "R'", "F ", "F2", "F'", "D ", "D2", "D'", "L ", "L2", "L'", "B ", "B2", "B'", "NULL",
};

static move_t reverse_move[] = {
    /* MOVE_U1 */ MOVE_U3,
    /* MOVE_U2 */ MOVE_U2,
    /* MOVE_U3 */ MOVE_U1,
    /* MOVE_R1 */ MOVE_R3,
    /* MOVE_R2 */ MOVE_R2,
    /* MOVE_R3 */ MOVE_R1,
    /* MOVE_F1 */ MOVE_F3,
    /* MOVE_F2 */ MOVE_F2,
    /* MOVE_F3 */ MOVE_F1,
    /* MOVE_D1 */ MOVE_D3,
    /* MOVE_D2 */ MOVE_D2,
    /* MOVE_D3 */ MOVE_D1,
    /* MOVE_L1 */ MOVE_L3,
    /* MOVE_L2 */ MOVE_L2,
    /* MOVE_L3 */ MOVE_L1,
    /* MOVE_B1 */ MOVE_B3,
    /* MOVE_B2 */ MOVE_B2,
    /* MOVE_B3 */ MOVE_B1,
};

int is_duplicated_or_undoes_move(move_t move1, move_t move2) { return move1 / 3 == move2 / 3; }

int cubie_off_count(const cube_cubie_t *cube) {
    int count = 0;

    for (int i = 0; i < 8; i++) {
        if (i != (int)cube->corner_permutations[i])
            count += 1;

        if (0 != cube->corner_orientations[i])
            count += 1;
    }

    for (int i = 0; i < 12; i++) {
        if (i != (int)cube->edge_permutations[i])
            count += 1;

        if (0 != cube->edge_orientations[i])
            count += 1;
    }

    return count;
}

int is_cubie_solved(const cube_cubie_t *cube) { return cubie_off_count(cube) == 0; }

char *move_to_str_enum(move_t move) {
    assert(move >= 0);
    assert(move <= N_MOVES);

    return _move_t_to_str_enum[move];
}

char *move_to_str(move_t move) {
    assert(move >= 0);
    assert(move <= 18);

    return _move_t_to_str[move];
}

int is_valid(cube_cubie_t *cube) {
    int edge_count[12]     = {0};
    int corner_count[8]    = {0};
    int edge_orientation   = 0;
    int corner_orientation = 0;

    for (int i = 0; i < 12; i++) {
        edge_count[cube->edge_permutations[i]]++;
        edge_orientation += cube->edge_orientations[i];
    }

    for (int i = 0; i < 12; i++)
        if (edge_count[i] != 1)
            return 0;

    if (edge_orientation % 2 != 0)
        return 0;

    for (int i = 0; i < 8; i++) {
        corner_count[cube->corner_permutations[i]]++;
        corner_orientation += cube->corner_orientations[i];
    }

    for (int i = 0; i < 8; i++)
        if (corner_count[i] != 1)
            return 0;

    if (corner_orientation % 3 != 0)
        return 0;

    // FIXME: This cant be used yet, because is causes several contracts to fail
    // Caused due to things like corner permutation being set but not the edges
    // if (get_corner_parity(cube) != get_edge_parity(cube))
    //     return 0;

    return 1;
}

cube_cubie_t *random_cubie_cube() {
    cube_cubie_t *cube = init_cubie_cube();

    for (int i = 0; i < 40; i++) {
        move_t move = pcg32_boundedrand(N_MOVES);
        cubie_apply_move(cube, move);
    }

    return cube;
}

coord_cube_t *random_coord_cube() {
    coord_cube_t *cube = get_coord_cube();

    for (int i = 0; i < 40; i++) {
        move_t move = pcg32_boundedrand(N_MOVES);
        coord_apply_move(cube, move);
    }

    return cube;
}

move_t get_reverse_move(move_t move) {
    assert(move >= 0);
    assert(move < N_MOVES);

    return reverse_move[move];
}

uint64_t get_microseconds(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (uint64_t)(ts.tv_sec * 1000000000L + ts.tv_nsec) / 1000;
}

int Cnk(int n, int k) {
    int i, j, s;

    if (n < k)
        return 0;

    if (k > n / 2)
        k = n - k;

    for (s = 1, i = n, j = 1; i != n - k; i--, j++) {
        s *= i;
        s /= j;
    }

    return s;
}

void rotate_left(int *pieces, int l, int r) {
    int t = pieces[l];

    for (int i = l; i < r; i++)
        pieces[i] = pieces[i + 1];

    pieces[r] = t;
}

void rotate_right(int *pieces, int l, int r) {
    int t = pieces[r];

    for (int i = r; i > l; i--)
        pieces[i] = pieces[i - 1];

    pieces[l] = t;
}

// Copy paste from https://stackoverflow.com/a/5467788
int unlink_cb(const char *fpath, __attribute__((unused)) const struct stat *sb, __attribute__((unused)) int typeflag,
              __attribute__((unused)) struct FTW *ftwbuf) {
    int rv = remove(fpath);

    if (rv)
        perror(fpath);

    return rv;
}

int rmrf(char *path) { return nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS); }

move_t *move_sequence_str_to_moves(const char *move_sequence_str) {
    size_t  max_moves = strlen(move_sequence_str);
    move_t *moves     = malloc(sizeof(move_t) * max_moves);

    char *input_copy = strdup(move_sequence_str);
    // NOLINTNEXTLINE(runtime/threadsafe_fn)
    const char *token    = strtok(input_copy, " ");
    size_t      move_idx = 0;

    while (token != NULL) {
        moves[move_idx++] = str_to_move(token);
        // NOLINTNEXTLINE(runtime/threadsafe_fn)
        token = strtok(NULL, " ");
    }

    moves[move_idx] = MOVE_NULL;
    free(input_copy);

    return moves;
}

move_t str_to_move(const char *move_str) {
    for (move_t move = 0; move < N_MOVES; move++) {
        const char *target_str = move_to_str(move);
        // Handle the case where input is shorter than target (e.g., "U" vs "U ")
        if (strncmp(move_str, target_str, strlen(move_str)) == 0 && strlen(move_str) <= strlen(target_str)) {
            return move;
        }
    }

    return MOVE_NULL;
}

void print_move_sequence(const move_t *moves) {
    for (int i = 0; moves[i] != MOVE_NULL && moves[i] != -1; i++) {
        printf("%s ", move_to_str(moves[i]));
    }
    printf("\n");
}

int are_move_sequences_equal(const move_t *moves1, const move_t *moves2) {
    for (int i = 0; moves1[i] != MOVE_NULL && moves2[i] != MOVE_NULL; i++) {
        if (moves1[i] != moves2[i])
            return 0;
    }

    return 1;
}
