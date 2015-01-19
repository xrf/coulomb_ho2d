/* compile with `-lcoulomb_ho2d` and, if needed, set the `LD_LIBRARY_PATH`
   environment variable appropriately */
#include <math.h>
#include <stdio.h>
#include "am.h"
int main(void) {
    static const double expected     = 0.303537;
    static const double expected_err = 0.000001;
    struct clh2_indices ix;
    double result;

    /* create context; to be efficient, reuse the context as much as possible
       (but only within the same thread) */
    clh2_ctx *ctx = clh2_ctx_create();
    if (!ctx) {
        fprintf(stderr, "can't create context\n");
        return 1;
    }

    /* initialize indices */
    ix.n1  =  0;
    ix.ml1 = -1;
    ix.n2  =  0;
    ix.ml2 =  1;
    ix.n3  =  0;
    ix.ml3 = -2;
    ix.n4  =  0;
    ix.ml4 =  2;

    /* calculate and print matrix element */
    result = clh2_element(ctx, &ix);
    printf("result: %.6f\n", result);

    /* check if it's correct (and not NAN) */
    if (!(fabs(result - expected) < expected_err)) {
        fprintf(stderr, "doesn't match expected result: %.6f\n", expected);
        return 2;
    }

    /* destroy context */
    clh2_ctx_destroy(ctx);
    return 0;
}
