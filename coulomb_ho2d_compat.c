/* The MIT License (MIT)

   Copyright (c) 2014 Fei Yuan.

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.

 */
#include <stdlib.h>
#include "coulomb_ho2d.h"
#include "coulomb_ho2d_utils.inl"
#ifdef __cplusplus
extern "C" {
#endif

/* Global cache. */
static struct clh2_ctx *global_ctx;

/* Cleans up the global cache. */
static void global_ctx_cleanup(void) {
    clh2_ctx_destroy(global_ctx);
}

/* Calculates the matrix element (thread-safe). */
double coulomb_ho2d_r(struct clh2_cache *cache,
                      unsigned n1, int ml1, unsigned n2, int ml2,
                      unsigned n4, int ml4, unsigned n3, int ml3) {
    struct clh2_indices ix;
    ix.n1 = n1;
    ix.n2 = n2;
    ix.n3 = n3;
    ix.n4 = n4;
    ix.ml1 = ml1;
    ix.ml2 = ml2;
    ix.ml3 = ml3;
    ix.ml4 = ml4;
    return clh2_element((clh2_ctx *) cache, &ix);
}

/* Calculates the matrix element (thread-unsafe). */
double coulomb_ho2d(unsigned n1, int ml1, unsigned n2, int ml2,
                    unsigned n4, int ml4, unsigned n3, int ml3) {
    if (!global_ctx) {
        global_ctx = clh2_ctx_create();
        if (!global_ctx) {
            warn("can't initialize global context");
            return NAN;
        }
        atexit(&global_ctx_cleanup);
    }
    return coulomb_ho2d_r((struct clh2_cache *) global_ctx,
                          n1, ml1, n2, ml2, n4, ml4, n3, ml3);
}

/* Alias of `clh2_ctx_create`. */
struct clh2_cache *clh2_cache_create(void) {
    return (struct clh2_cache *) clh2_ctx_create();
}

/* Alias of `clh2_ctx_destroy`. */
void clh2_cache_destroy(struct clh2_cache *cache) {
    clh2_ctx_destroy((clh2_ctx *) cache);
}

#ifdef __cplusplus
}
#endif
