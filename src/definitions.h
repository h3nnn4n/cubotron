#ifndef _DEFINITIONS
#define _DEFINITIONS


typedef enum {
    ROT_URF3 = 0,
    ROT_F2 = 1,
    ROT_U4 = 2,
    MIRR_LR2 = 3,
} base_symmetry;


typedef enum {
    CORNER_URF = 0,
    CORNER_UFL = 1,
    CORNER_ULB = 2,
    CORNER_UBR = 3,
    CORNER_DFR = 4,
    CORNER_DLF = 5,
    CORNER_DBL = 6,
    CORNER_DRB = 7,
} corner;


typedef enum {
    EDGE_UR = 0,
    EDGE_UF = 1,
    EDGE_UL = 2,
    EDGE_UB = 3,
    EDGE_DR = 4,
    EDGE_DF = 5,
    EDGE_DL = 6,
    EDGE_DB = 7,
    EDGE_FR = 8,
    EDGE_FL = 9,
    EDGE_BL = 10,
    EDGE_BR = 11,
} edge;


typedef enum {
    MOVE_U1 = 0,
    MOVE_U2 = 1,
    MOVE_U3 = 2,
    MOVE_R1 = 3,
    MOVE_R2 = 4,
    MOVE_R3 = 5,
    MOVE_F1 = 6,
    MOVE_F2 = 7,
    MOVE_F3 = 8,
    MOVE_D1 = 9,
    MOVE_D2 = 10,
    MOVE_D3 = 11,
    MOVE_L1 = 12,
    MOVE_L2 = 13,
    MOVE_L3 = 14,
    MOVE_B1 = 15,
    MOVE_B2 = 16,
    MOVE_B3 = 17,
} move;


typedef enum {
    FACELET_U1 = 0,
    FACELET_U2 = 1,
    FACELET_U3 = 2,
    FACELET_U4 = 3,
    FACELET_U5 = 4,
    FACELET_U6 = 5,
    FACELET_U7 = 6,
    FACELET_U8 = 7,
    FACELET_U9 = 8,
    FACELET_R1 = 9,
    FACELET_R2 = 10,
    FACELET_R3 = 11,
    FACELET_R4 = 12,
    FACELET_R5 = 13,
    FACELET_R6 = 14,
    FACELET_R7 = 15,
    FACELET_R8 = 16,
    FACELET_R9 = 17,
    FACELET_F1 = 18,
    FACELET_F2 = 19,
    FACELET_F3 = 20,
    FACELET_F4 = 21,
    FACELET_F5 = 22,
    FACELET_F6 = 23,
    FACELET_F7 = 24,
    FACELET_F8 = 25,
    FACELET_F9 = 26,
    FACELET_D1 = 27,
    FACELET_D2 = 28,
    FACELET_D3 = 29,
    FACELET_D4 = 30,
    FACELET_D5 = 31,
    FACELET_D6 = 32,
    FACELET_D7 = 33,
    FACELET_D8 = 34,
    FACELET_D9 = 35,
    FACELET_L1 = 36,
    FACELET_L2 = 37,
    FACELET_L3 = 38,
    FACELET_L4 = 39,
    FACELET_L5 = 40,
    FACELET_L6 = 41,
    FACELET_L7 = 42,
    FACELET_L8 = 43,
    FACELET_L9 = 44,
    FACELET_B1 = 45,
    FACELET_B2 = 46,
    FACELET_B3 = 47,
    FACELET_B4 = 48,
    FACELET_B5 = 49,
    FACELET_B6 = 50,
    FACELET_B7 = 51,
    FACELET_B8 = 52,
    FACELET_B9 = 53,
} facelet;

#endif /* end of include guard */
