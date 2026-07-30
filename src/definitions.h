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

#ifndef _DEFINITIONS
#define _DEFINITIONS

#include "puzzle_types.h"

#define N_EDGE_ORIENTATIONS       2048
#define N_SLICES                  495
#define N_SORTED_SLICES           11880
#define N_SORTED_SLICES_PHASE2    24
#define N_UD6_PHASE1_PERMUTATIONS 665280
#define N_UD6_PHASE2_PERMUTATIONS 20160
#define N_UD7_PHASE1_PERMUTATIONS 3991680
#define N_UD7_PHASE2_PERMUTATIONS 40320

#define N_EDGES    12
#define N_FACELETS 54

typedef enum {
    UR = 0,
    UF = 1,
    UL = 2,
    UB = 3,
    DR = 4,
    DF = 5,
    DL = 6,
    DB = 7,
    FR = 8,
    FL = 9,
    BL = 10,
    BR = 11,
} edge_t;

typedef enum {
    U1 = 0,
    U2 = 1,
    U3 = 2,
    U4 = 3,
    U5 = 4,
    U6 = 5,
    U7 = 6,
    U8 = 7,
    U9 = 8,
    R1 = 9,
    R2 = 10,
    R3 = 11,
    R4 = 12,
    R5 = 13,
    R6 = 14,
    R7 = 15,
    R8 = 16,
    R9 = 17,
    F1 = 18,
    F2 = 19,
    F3 = 20,
    F4 = 21,
    F5 = 22,
    F6 = 23,
    F7 = 24,
    F8 = 25,
    F9 = 26,
    D1 = 27,
    D2 = 28,
    D3 = 29,
    D4 = 30,
    D5 = 31,
    D6 = 32,
    D7 = 33,
    D8 = 34,
    D9 = 35,
    L1 = 36,
    L2 = 37,
    L3 = 38,
    L4 = 39,
    L5 = 40,
    L6 = 41,
    L7 = 42,
    L8 = 43,
    L9 = 44,
    B1 = 45,
    B2 = 46,
    B3 = 47,
    B4 = 48,
    B5 = 49,
    B6 = 50,
    B7 = 51,
    B8 = 52,
    B9 = 53,
} facelet_t;

#endif
