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

#ifndef _PUZZLE_TYPES
#define _PUZZLE_TYPES

#define N_CORNER_ORIENTATIONS 2187
#define N_PARITY              2
#define N_CORNER_PERMUTATIONS 40320

#define N_CORNERS 8
#define N_MOVES   18
#define N_COLORS  6

typedef enum {
    U = 0,
    R = 1,
    F = 2,
    D = 3,
    L = 4,
    B = 5,
} color_t;

typedef enum {
    URF = 0,
    UFL = 1,
    ULB = 2,
    UBR = 3,
    DFR = 4,
    DLF = 5,
    DBL = 6,
    DRB = 7,
} corner_t;

typedef enum {
    MOVE_U1   = 0,
    MOVE_U2   = 1,
    MOVE_U3   = 2,
    MOVE_R1   = 3,
    MOVE_R2   = 4,
    MOVE_R3   = 5,
    MOVE_F1   = 6,
    MOVE_F2   = 7,
    MOVE_F3   = 8,
    MOVE_D1   = 9,
    MOVE_D2   = 10,
    MOVE_D3   = 11,
    MOVE_L1   = 12,
    MOVE_L2   = 13,
    MOVE_L3   = 14,
    MOVE_B1   = 15,
    MOVE_B2   = 16,
    MOVE_B3   = 17,
    MOVE_NULL = 18,
} move_t;

#endif
