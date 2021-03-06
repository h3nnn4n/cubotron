#if !defined(__APPLE__)
#define _XOPEN_SOURCE 500
#endif

#include <assert.h>
#include <ftw.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <pcg_variants.h>

#include "coord_cube.h"
#include "coord_move_tables.h"
#include "cubie_cube.h"
#include "cubie_move_table.h"

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

int is_bad_move(move_t move1, move_t move2) { return move1 / 3 == move2 / 3; }

int cubie_off_count(cube_cubie_t *cube) {
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

int is_cubie_solved(cube_cubie_t *cube) { return cubie_off_count(cube) == 0; }

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

long get_microseconds(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (long)(ts.tv_sec * 1000000000L + ts.tv_nsec) / 1000;
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

move_t str_to_move(char *move_str) {
    for (move_t move = 0; move < N_MOVES; move++) {
        if (strncmp(move_str, move_to_str(move), 2) == 0) {
            return move;
        }
    }

    return MOVE_NULL;
}
