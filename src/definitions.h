#ifndef _DEFINITIONS
#define _DEFINITIONS

#define N_CORNER_ORIENTATIONS     2187
#define N_EDGE_ORIENTATIONS       2048
#define N_SLICES                  495
#define N_SORTED_SLICES           11880
#define N_SORTED_SLICES_PHASE2    24
#define N_PARITY                  2
#define N_CORNER_PERMUTATIONS     40320
#define N_UD6_PHASE1_PERMUTATIONS 665280
#define N_UD6_PHASE2_PERMUTATIONS 20160
#define N_UD7_PHASE1_PERMUTATIONS 3991680 // 791 * 5040 + 5040 or 12 * 11 * 10 * 9 * 8 * 7 * 6

#define N_CORNERS 8
#define N_EDGES   12

#define N_MOVES 18

#define N_COLORS   6
#define N_FACELETS 54

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

#endif /* end of include guard */
