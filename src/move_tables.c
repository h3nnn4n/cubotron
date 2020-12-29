#include "definitions.h"

// Up
corner corner_permutation_U[] = {CORNER_UBR, CORNER_URF, CORNER_UFL, CORNER_ULB, CORNER_DFR, CORNER_DLF, CORNER_DBL, CORNER_DRB};
int corner_orientation_U[] = {0, 0, 0, 0, 0, 0, 0, 0};
edge edge_permutation_U[] = {EDGE_UB, EDGE_UR, EDGE_UF, EDGE_UL, EDGE_DR, EDGE_DF, EDGE_DL, EDGE_DB, EDGE_FR, EDGE_FL, EDGE_BL, EDGE_BR};
int edge_orientation_U[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Right;
corner corner_permutation_R[] = {CORNER_DFR, CORNER_UFL, CORNER_ULB, CORNER_URF, CORNER_DRB, CORNER_DLF, CORNER_DBL, CORNER_UBR};
int corner_orientation_R[] = {2, 0, 0, 1, 1, 0, 0, 2};
edge edge_permutation_R[] = {EDGE_FR, EDGE_UF, EDGE_UL, EDGE_UB, EDGE_BR, EDGE_DF, EDGE_DL, EDGE_DB, EDGE_DR, EDGE_FL, EDGE_BL, EDGE_UR};
int edge_orientation_R[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Front;
corner corner_permutation_F[] = {CORNER_UFL, CORNER_DLF, CORNER_ULB, CORNER_UBR, CORNER_URF, CORNER_DFR, CORNER_DBL, CORNER_DRB};
int corner_orientation_F[] = {1, 2, 0, 0, 2, 1, 0, 0};
edge edge_permutation_F[] = {EDGE_UR, EDGE_FL, EDGE_UL, EDGE_UB, EDGE_DR, EDGE_FR, EDGE_DL, EDGE_DB, EDGE_UF, EDGE_DF, EDGE_BL, EDGE_BR};
int edge_orientation_F[] = {0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0};

// Down;
corner corner_permutation_D[] = {CORNER_URF, CORNER_UFL, CORNER_ULB, CORNER_UBR, CORNER_DLF, CORNER_DBL, CORNER_DRB, CORNER_DFR};
int corner_orientation_D[] = {0, 0, 0, 0, 0, 0, 0, 0};
edge edge_permutation_D[] = {EDGE_UR, EDGE_UF, EDGE_UL, EDGE_UB, EDGE_DF, EDGE_DL, EDGE_DB, EDGE_DR, EDGE_FR, EDGE_FL, EDGE_BL, EDGE_BR};
int edge_orientation_D[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Left;
corner corner_permutation_L[] = {CORNER_URF, CORNER_ULB, CORNER_DBL, CORNER_UBR, CORNER_DFR, CORNER_UFL, CORNER_DLF, CORNER_DRB};
int corner_orientation_L[] = {0, 1, 2, 0, 0, 2, 1, 0};
edge edge_permutation_L[] = {EDGE_UR, EDGE_UF, EDGE_BL, EDGE_UB, EDGE_DR, EDGE_DF, EDGE_FL, EDGE_DB, EDGE_FR, EDGE_UL, EDGE_DL, EDGE_BR};
int edge_orientation_L[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Back;
corner corner_permutation_B[] = {CORNER_URF, CORNER_UFL, CORNER_UBR, CORNER_DRB, CORNER_DFR, CORNER_DLF, CORNER_ULB, CORNER_DBL};
int corner_orientation_B[] = {0, 0, 1, 2, 0, 0, 2, 1};
edge edge_permutation_B[] = {EDGE_UR, EDGE_UF, EDGE_UL, EDGE_BR, EDGE_DR, EDGE_DF, EDGE_DL, EDGE_BL, EDGE_FR, EDGE_FL, EDGE_UB, EDGE_DB};
int corner_orientation_B[] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1};
