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
#include <stdlib.h>
#include <string.h>

#include "coord_move_tables.h"
#include "cubie_cube.h"
#include "cubie_move_table.h"
#include "definitions.h"
#include "file_utils.h"
#include "symmetry.h"
#include "utils.h"

// ---------------------------------------------------------------------------
// The four generators of the full Oh symmetry group (48 elements).
// Each is expressed as a cubie cube: corner_permutations[i] = which corner
// is at position i, edge_permutations[i] = which edge is at position i.
// Mirror orientations (values 3-5) are used for MIRR_LR2 corners because
// reflections reverse chirality; the multiply_cube_cubie_corners arithmetic
// handles all six orientation values correctly.
// ---------------------------------------------------------------------------

// 120-degree clockwise rotation around the long diagonal through URF and DBL
static const cube_cubie_t GEN_ROT_URF3 = {
    .corner_permutations = {URF, DFR, DLF, UFL, UBR, DRB, DBL, ULB},
    .corner_orientations = {1, 2, 1, 2, 2, 1, 2, 1},
    .edge_permutations   = {UF, FR, DF, FL, UB, BR, DB, BL, UR, DR, DL, UL},
    .edge_orientations   = {1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1},
};

// 180-degree rotation around the axis through the F and B face centers
static const cube_cubie_t GEN_ROT_F2 = {
    .corner_permutations = {DLF, DFR, DRB, DBL, UFL, URF, UBR, ULB},
    .corner_orientations = {0, 0, 0, 0, 0, 0, 0, 0},
    .edge_permutations   = {DL, DF, DR, DB, UL, UF, UR, UB, FL, FR, BR, BL},
    .edge_orientations   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

// 90-degree clockwise rotation around the axis through the U and D face centers
static const cube_cubie_t GEN_ROT_U4 = {
    .corner_permutations = {UBR, URF, UFL, ULB, DRB, DFR, DLF, DBL},
    .corner_orientations = {0, 0, 0, 0, 0, 0, 0, 0},
    .edge_permutations   = {UB, UR, UF, UL, DB, DR, DF, DL, BR, FR, FL, BL},
    .edge_orientations   = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
};

// Reflection through the plane containing the U, D, F, B face centers
static const cube_cubie_t GEN_MIRR_LR2 = {
    .corner_permutations = {UFL, URF, UBR, ULB, DLF, DFR, DRB, DBL},
    .corner_orientations = {3, 3, 3, 3, 3, 3, 3, 3},
    .edge_permutations   = {UL, UF, UR, UB, DL, DF, DR, DB, FL, FR, BR, BL},
    .edge_orientations   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

// ---------------------------------------------------------------------------
// Static table storage
// ---------------------------------------------------------------------------

static cube_cubie_t sym_cubes[N_SYMMETRIES];
static uint8_t      sym_inv[N_SYMMETRIES];
static uint8_t      conj_move[N_MOVES * N_SYMMETRIES];

#define N_FLIPSLICE (N_EDGE_ORIENTATIONS * N_SLICES)

static uint16_t flipslice_classidx[N_FLIPSLICE];
static uint8_t  flipslice_sym[N_FLIPSLICE];
static uint32_t flipslice_rep[N_FLIPSLICE_CLASSES];

static uint16_t twist_conj[N_CORNER_ORIENTATIONS * N_SYMMETRIES_D4H];

static uint16_t corner_classidx[N_CORNER_PERMUTATIONS];
static uint8_t  corner_sym[N_CORNER_PERMUTATIONS];
static uint16_t corner_rep[N_CORNER_CLASSES];

static uint16_t ud_edges_conj[N_UD7_PHASE2_PERMUTATIONS * N_SYMMETRIES_D4H];

static int tables_built = 0;

// ---------------------------------------------------------------------------
// Raw binary cache helpers
// ---------------------------------------------------------------------------

static void cache_store(const char *name, const void *data, size_t bytes) {
    char path[512];
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    snprintf(path, sizeof(path), "cache/symmetry_tables/%s", name);
    ensure_directory_exists("cache/symmetry_tables");
    FILE *f = fopen(path, "wb");
    assert(f != NULL);
    size_t written = fwrite(data, 1, bytes, f);
    fclose(f);
    assert(written == bytes);
}

static int cache_load(const char *name, void *data, size_t bytes) {
    char path[512];
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    snprintf(path, sizeof(path), "cache/symmetry_tables/%s", name);
    if (!file_exists(path))
        return 0;
    FILE *f = fopen(path, "rb");
    assert(f != NULL);
    size_t n = fread(data, 1, bytes, f);
    fclose(f);
    return (int)(n == bytes);
}

// ---------------------------------------------------------------------------
// Build all 48 symmetry cubes and their inverses
// ---------------------------------------------------------------------------

static void build_sym_cubes_and_inv(void) {
    // Start from the identity cube and compose generators in the same order
    // as the kociemba reference implementation, producing the same 48 indices.
    // The D4H subgroup (indices 0-15) is generated by GEN_ROT_F2, GEN_ROT_U4,
    // and GEN_MIRR_LR2 alone (the innermost three loops, urf3 = 0).
    cube_cubie_t cc;
    memset(&cc, 0, sizeof(cc));

    for (int i = 0; i < N_CORNERS; i++) {
        cc.corner_permutations[i] = (corner_t)i;
    }

    for (int i = 0; i < N_EDGES; i++) {
        cc.edge_permutations[i] = (edge_t)i;
    }

    int s = 0;

    for (int urf3 = 0; urf3 < 3; urf3++) {
        for (int f2 = 0; f2 < 2; f2++) {
            for (int u4 = 0; u4 < 4; u4++) {
                for (int lr2 = 0; lr2 < 2; lr2++) {
                    sym_cubes[s++] = cc;
                    multiply_cube_cubie(&cc, (cube_cubie_t *)&GEN_MIRR_LR2);
                }

                multiply_cube_cubie(&cc, (cube_cubie_t *)&GEN_ROT_U4);
            }

            multiply_cube_cubie(&cc, (cube_cubie_t *)&GEN_ROT_F2);
        }

        multiply_cube_cubie(&cc, (cube_cubie_t *)&GEN_ROT_URF3);
    }

    assert(s == N_SYMMETRIES);

    // Find the inverse of each symmetry: sym_inv[s] = j where sym[s] * sym[j] = identity
    cube_cubie_t identity;
    memset(&identity, 0, sizeof(identity));

    for (int i = 0; i < N_CORNERS; i++) {
        identity.corner_permutations[i] = (corner_t)i;
    }

    for (int i = 0; i < N_EDGES; i++) {
        identity.edge_permutations[i] = (edge_t)i;
    }

    for (int si = 0; si < N_SYMMETRIES; si++) {
        int found = 0;

        for (int j = 0; j < N_SYMMETRIES; j++) {
            cube_cubie_t tmp = sym_cubes[si];
            multiply_cube_cubie(&tmp, &sym_cubes[j]);

            if (are_cubie_equal(&tmp, &identity)) {
                sym_inv[si] = (uint8_t)j;
                found       = 1;
                break;
            }
        }

        assert(found);
    }
}

// ---------------------------------------------------------------------------
// Build conj_move: conj_move[N_MOVES * s + m] = index of move s * m * s^-1
// ---------------------------------------------------------------------------

static void build_conj_move(void) {
    // Build all 18 move cubie cubes locally.
    // Quarter-turn cubes are built from the existing basic-move data;
    // double and inverse turns are derived by composition.
    cube_cubie_t move_cubes[N_MOVES];

    for (int face = 0; face < N_COLORS; face++) {
        cube_cubie_t *q      = cubie_build_basic_move((move_t)(3 * face));
        move_cubes[3 * face] = *q;
        free(q);

        // Double turn
        move_cubes[3 * face + 1] = move_cubes[3 * face];
        multiply_cube_cubie(&move_cubes[3 * face + 1], &move_cubes[3 * face]);

        // Inverse (triple turn = double then one more quarter)
        move_cubes[3 * face + 2] = move_cubes[3 * face + 1];
        multiply_cube_cubie(&move_cubes[3 * face + 2], &move_cubes[3 * face]);
    }

    for (int s = 0; s < N_SYMMETRIES; s++) {
        for (int m = 0; m < N_MOVES; m++) {
            cube_cubie_t tmp = sym_cubes[s];
            multiply_cube_cubie(&tmp, &move_cubes[m]);
            multiply_cube_cubie(&tmp, &sym_cubes[sym_inv[s]]);

            int found = 0;

            for (int m2 = 0; m2 < N_MOVES; m2++) {
                if (are_cubie_equal(&tmp, &move_cubes[m2])) {
                    conj_move[N_MOVES * s + m] = (uint8_t)m2;
                    found                      = 1;
                    break;
                }
            }

            assert(found);
        }
    }
}

// ---------------------------------------------------------------------------
// Build flipslice orbit tables
// ---------------------------------------------------------------------------

static void build_flipslice_tables(void) {
    memset(flipslice_classidx, 0xff, sizeof(flipslice_classidx)); // 0xffff = unvisited
    memset(flipslice_sym, 0, sizeof(flipslice_sym));
    memset(flipslice_rep, 0, sizeof(flipslice_rep));

    int classidx = 0;

    for (int slice = 0; slice < N_SLICES; slice++) {
        for (int flip = 0; flip < N_EDGE_ORIENTATIONS; flip++) {
            int idx = N_EDGE_ORIENTATIONS * slice + flip;

            if (flipslice_classidx[idx] != 0xffff)
                continue;

            // This state is a new representative
            assert(classidx < N_FLIPSLICE_CLASSES);
            flipslice_classidx[idx] = (uint16_t)classidx;
            flipslice_sym[idx]      = 0;
            flipslice_rep[classidx] = (uint32_t)idx;

            // Apply all 16 D4H symmetries to generate the full orbit.
            // The orbit member is sym_inv[s] * representative * sym[s] on the edge part.
            cube_cubie_t rep_cube;
            memset(&rep_cube, 0, sizeof(rep_cube));

            for (int i = 0; i < N_CORNERS; i++) {
                rep_cube.corner_permutations[i] = (corner_t)i;
            }

            for (int i = 0; i < N_EDGES; i++) {
                rep_cube.edge_permutations[i] = (edge_t)i;
            }

            set_E_slice(&rep_cube, slice);
            set_edge_orientations(&rep_cube, flip);

            for (int s = 1; s < N_SYMMETRIES_D4H; s++) {
                cube_cubie_t tmp = sym_cubes[sym_inv[s]];
                multiply_cube_cubie_edges(&tmp, &rep_cube);
                multiply_cube_cubie_edges(&tmp, &sym_cubes[s]);

                int new_flip  = get_edge_orientations(&tmp);
                int new_slice = get_E_slice(&tmp);
                int new_idx   = N_EDGE_ORIENTATIONS * new_slice + new_flip;

                assert(new_flip >= 0 && new_flip < N_EDGE_ORIENTATIONS);
                assert(new_slice >= 0 && new_slice < N_SLICES);

                if (flipslice_classidx[new_idx] == 0xffff) {
                    flipslice_classidx[new_idx] = (uint16_t)classidx;
                    flipslice_sym[new_idx]      = (uint8_t)s;
                }
            }

            classidx++;
        }
    }

    assert(classidx == N_FLIPSLICE_CLASSES);
}

// ---------------------------------------------------------------------------
// Build twist_conj: twist_conj[twist * N_SYMMETRIES_D4H + s] = s * twist * s^-1
// ---------------------------------------------------------------------------

static void build_twist_conj(void) {
    for (int t = 0; t < N_CORNER_ORIENTATIONS; t++) {
        cube_cubie_t twist_cube;
        memset(&twist_cube, 0, sizeof(twist_cube));

        for (int i = 0; i < N_CORNERS; i++) {
            twist_cube.corner_permutations[i] = (corner_t)i;
        }

        for (int i = 0; i < N_EDGES; i++) {
            twist_cube.edge_permutations[i] = (edge_t)i;
        }

        set_corner_orientations(&twist_cube, t);

        for (int s = 0; s < N_SYMMETRIES_D4H; s++) {
            cube_cubie_t tmp = sym_cubes[s];
            multiply_cube_cubie_corners(&tmp, &twist_cube);
            multiply_cube_cubie_corners(&tmp, &sym_cubes[sym_inv[s]]);

            int conj = get_corner_orientations(&tmp);
            assert(conj >= 0 && conj < N_CORNER_ORIENTATIONS);
            twist_conj[t * N_SYMMETRIES_D4H + s] = (uint16_t)conj;
        }
    }
}

// ---------------------------------------------------------------------------
// Build corner permutation orbit tables
// ---------------------------------------------------------------------------

static void build_corner_tables(void) {
    memset(corner_classidx, 0xff, sizeof(corner_classidx)); // 0xffff = unvisited
    memset(corner_sym, 0, sizeof(corner_sym));
    memset(corner_rep, 0, sizeof(corner_rep));

    int classidx = 0;

    for (int cp = 0; cp < N_CORNER_PERMUTATIONS; cp++) {
        if (corner_classidx[cp] != 0xffff)
            continue;

        assert(classidx < N_CORNER_CLASSES);
        corner_classidx[cp]  = (uint16_t)classidx;
        corner_sym[cp]       = 0;
        corner_rep[classidx] = (uint16_t)cp;

        cube_cubie_t rep_cube;
        memset(&rep_cube, 0, sizeof(rep_cube));

        for (int i = 0; i < N_CORNERS; i++) {
            rep_cube.corner_permutations[i] = (corner_t)i;
        }

        for (int i = 0; i < N_EDGES; i++) {
            rep_cube.edge_permutations[i] = (edge_t)i;
        }

        set_corner_permutations(&rep_cube, cp);

        for (int s = 1; s < N_SYMMETRIES_D4H; s++) {
            cube_cubie_t tmp = sym_cubes[sym_inv[s]];
            multiply_cube_cubie_corners(&tmp, &rep_cube);
            multiply_cube_cubie_corners(&tmp, &sym_cubes[s]);

            int new_cp = get_corner_permutations(&tmp);
            assert(new_cp >= 0 && new_cp < N_CORNER_PERMUTATIONS);

            if (corner_classidx[new_cp] == 0xffff) {
                corner_classidx[new_cp] = (uint16_t)classidx;
                corner_sym[new_cp]      = (uint8_t)s;
            }
        }

        classidx++;
    }

    assert(classidx == N_CORNER_CLASSES);
}

// ---------------------------------------------------------------------------
// Build ud_edges_conj: ud_edges_conj[ud * N_SYMMETRIES_D4H + s] = s * ud * s^-1
// ---------------------------------------------------------------------------

static void build_ud_edges_conj(void) {
    for (int ud = 0; ud < N_UD7_PHASE2_PERMUTATIONS; ud++) {
        cube_cubie_t ud_cube;
        memset(&ud_cube, 0, sizeof(ud_cube));

        for (int i = 0; i < N_CORNERS; i++) {
            ud_cube.corner_permutations[i] = (corner_t)i;
        }

        for (int i = 0; i < N_EDGES; i++) {
            ud_cube.edge_permutations[i] = (edge_t)i;
        }

        set_UD7_edges(&ud_cube, ud);

        for (int s = 0; s < N_SYMMETRIES_D4H; s++) {
            cube_cubie_t tmp = sym_cubes[sym_inv[s]];
            multiply_cube_cubie_edges(&tmp, &ud_cube);
            multiply_cube_cubie_edges(&tmp, &sym_cubes[s]);

            int conj = get_UD7_edges(&tmp);

            // D4H preserves the UD-slice structure, so the result must stay
            // in the phase-2 range (all 8 UD edges remain in UD positions).
            assert(conj >= 0 && conj < N_UD7_PHASE2_PERMUTATIONS);
            ud_edges_conj[ud * N_SYMMETRIES_D4H + s] = (uint16_t)conj;
        }
    }
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void build_symmetry_tables(void) {
    if (tables_built)
        return;

    // The sym cubes and inverses are cheap to recompute; no caching needed.
    build_sym_cubes_and_inv();
    build_conj_move();

    // Attempt to load cached tables.  If any are missing, regenerate all.
    int loaded = cache_load("flipslice_classidx", flipslice_classidx, sizeof(flipslice_classidx)) &&
                 cache_load("flipslice_sym", flipslice_sym, sizeof(flipslice_sym)) &&
                 cache_load("flipslice_rep", flipslice_rep, sizeof(flipslice_rep)) &&
                 cache_load("twist_conj", twist_conj, sizeof(twist_conj)) &&
                 cache_load("corner_classidx", corner_classidx, sizeof(corner_classidx)) &&
                 cache_load("corner_sym", corner_sym, sizeof(corner_sym)) &&
                 cache_load("corner_rep", corner_rep, sizeof(corner_rep)) &&
                 cache_load("ud_edges_conj", ud_edges_conj, sizeof(ud_edges_conj));

    if (!loaded) {
        build_flipslice_tables();
        build_twist_conj();
        build_corner_tables();
        build_ud_edges_conj();

        cache_store("flipslice_classidx", flipslice_classidx, sizeof(flipslice_classidx));
        cache_store("flipslice_sym", flipslice_sym, sizeof(flipslice_sym));
        cache_store("flipslice_rep", flipslice_rep, sizeof(flipslice_rep));
        cache_store("twist_conj", twist_conj, sizeof(twist_conj));
        cache_store("corner_classidx", corner_classidx, sizeof(corner_classidx));
        cache_store("corner_sym", corner_sym, sizeof(corner_sym));
        cache_store("corner_rep", corner_rep, sizeof(corner_rep));
        cache_store("ud_edges_conj", ud_edges_conj, sizeof(ud_edges_conj));
    }

    tables_built = 1;
}

const cube_cubie_t *get_sym_cubes(void) {
    assert(tables_built);
    return sym_cubes;
}

const uint8_t *get_sym_inv(void) {
    assert(tables_built);
    return sym_inv;
}

const uint8_t *get_conj_move(void) {
    assert(tables_built);
    return conj_move;
}

const uint16_t *get_flipslice_classidx(void) {
    assert(tables_built);
    return flipslice_classidx;
}

const uint8_t *get_flipslice_sym(void) {
    assert(tables_built);
    return flipslice_sym;
}

const uint32_t *get_flipslice_rep(void) {
    assert(tables_built);
    return flipslice_rep;
}

const uint16_t *get_twist_conj(void) {
    assert(tables_built);
    return twist_conj;
}

const uint16_t *get_corner_classidx(void) {
    assert(tables_built);
    return corner_classidx;
}

const uint8_t *get_corner_sym(void) {
    assert(tables_built);
    return corner_sym;
}

const uint16_t *get_corner_rep(void) {
    assert(tables_built);
    return corner_rep;
}

const uint16_t *get_ud_edges_conj(void) {
    assert(tables_built);
    return ud_edges_conj;
}
