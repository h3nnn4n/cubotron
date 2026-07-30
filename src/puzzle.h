/*
 * Copyright <2021> <Renan S Silva, aka h3nnn4n>
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

#ifndef _PUZZLE_H
#define _PUZZLE_H

#include <stddef.h>

#include "definitions.h"

typedef struct puzzle_ops_s puzzle_ops_t;
typedef struct puzzle_s puzzle_t;

struct puzzle_ops_s {
    const char *name;
    int        n_moves;
    size_t     state_size;

    void  (*reset)(void *state);
    int   (*is_solved)(const void *state);
    void  (*apply_move)(void *state, move_t move);
    void  (*copy)(void *dst, const void *src);

    int   (*from_string)(void *state, const char *str);
    void  (*to_string)(const void *state, char *buf, size_t bufsz);
};

struct puzzle_s {
    const puzzle_ops_t *ops;
    void               *state;
};

void                puzzle_register(const puzzle_ops_t *ops);
const puzzle_ops_t *puzzle_lookup(const char *name);
puzzle_t           *puzzle_create(const char *name);
void                puzzle_destroy(puzzle_t *puzzle);

#endif
