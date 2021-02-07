#include <assert.h>
#include <stdio.h>

#include "definitions.h"
#include "facelets.h"

int verify_valid_facelets(char facelets[N_FACELETS]) {
    int faces[N_COLORS] = {0};

    for (int i = 0; i < N_FACELETS; i++) {
        switch (facelets[i]) {
            case 'U': faces[U] += 1; break;
            case 'R': faces[R] += 1; break;
            case 'F': faces[F] += 1; break;
            case 'D': faces[D] += 1; break;
            case 'L': faces[L] += 1; break;
            case 'B': faces[B] += 1; break;
        }
    }

    for (int i = 0; i < N_COLORS; i++) {
        if (faces[i] != 9)
            return 0;
    }

    return 1;
}
