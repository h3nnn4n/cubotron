#ifndef _SYMMETRY_H
#define _SYMMETRY_H

#include <stdint.h>

#include "cubie_cube.h"
#include "definitions.h"

void build_symmetry_tables(void);

// All 48 symmetry cubie cubes
const cube_cubie_t *get_sym_cubes(void);
// sym_inv[s] = index of the inverse of symmetry s
const uint8_t *get_sym_inv(void);
// conj_move[N_MOVES * s + m] = s * m * s^-1
const uint8_t *get_conj_move(void);

// Flipslice orbit tables (D4H acting on 2048 * 495 flip/slice pairs)
// classidx[N_EDGE_ORIENTATIONS * slice + flip] = equivalence class index
const uint16_t *get_flipslice_classidx(void);
// sym[N_EDGE_ORIENTATIONS * slice + flip] = s such that sym_inv[s] * rep * sym[s] = state
const uint8_t *get_flipslice_sym(void);
// rep[classidx] = representative state (encoded as N_EDGE_ORIENTATIONS * slice + flip)
const uint32_t *get_flipslice_rep(void);

// twist_conj[twist * N_SYMMETRIES_D4H + s] = s * twist * s^-1
const uint16_t *get_twist_conj(void);

// Corner permutation orbit tables (D4H acting on 40320 corner permutations)
const uint16_t *get_corner_classidx(void);
const uint8_t  *get_corner_sym(void);
const uint16_t *get_corner_rep(void);

// ud_edges_conj[ud * N_SYMMETRIES_D4H + s] = s * ud * s^-1  (phase-2 range 0..40319)
const uint16_t *get_ud_edges_conj(void);

#endif
