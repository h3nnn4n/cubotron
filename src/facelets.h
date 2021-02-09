#ifndef _FACELETS
#define _FACELETS

#include "cubie_cube.h"
#include "definitions.h"

int           verify_valid_facelets(char facelets[N_FACELETS]);
color_t *     build_facelet(char facelets[N_FACELETS]);
cube_cubie_t *build_cubie_cube_from_str(char facelets[N_FACELETS]);

#endif /* end of include guard */
