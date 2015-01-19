#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "am.h"

/* calculate the maximum `n` that lies within the required shells */
unsigned n_max(unsigned num_shells, int ml) {
    if ((unsigned) abs(ml) >= num_shells)
        return 0;
    return (num_shells - (unsigned) abs(ml) + 1) / 2;
}

int main(int argc, char **argv) {
    const char *prog = "table";
    double result;
    unsigned n1, n2, n3, n4;
    int num_shells, ml1, ml2, ml3, ml4;
    clh2_ctx *ctx;
    struct clh2_indices ix;

    /* get program name */
    if (argc && strlen(argv[0]))
        prog = argv[0];

    /* print usage info if arguments aren't provided */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s NUM_SHELLS\n"
                        "  where NUM_SHELLS is the number of shells\n", prog);
        return EXIT_FAILURE;
    }

    /* obtain the number of shells (equivalent to maximum `ml`) */
    if (sscanf(argv[1], "%d", &num_shells) != 1 || num_shells < 0) {
        fprintf(stderr, "%s: invalid argument: %s\n", prog, argv[1]);
        return EXIT_FAILURE;
    }

    /* create context */
    ctx = clh2_ctx_create();
    if (!ctx) {
        fprintf(stderr, "can't create context\n");
        return EXIT_FAILURE;
    }

    /* print header */
    printf("# Coulomb matrix elements for up to %d shells\n"
           "# %3s %3s %3s %3s %3s %3s %3s %3s %22s\n", num_shells,
           "n1", "ml1", "n2", "ml2",
           "n3", "ml3", "n4", "ml4", "value");

    /* calculate matrix elements */
    for (ml1 = 1 - num_shells; ml1 < num_shells; ++ml1)
    for (ml2 = 1 - num_shells; ml2 < num_shells; ++ml2)
    for (ml3 = 1 - num_shells; ml3 < num_shells; ++ml3) {
        ml4 = ml1 + ml2 - ml3;
        if (ml4 < 1 - num_shells || ml4 >= num_shells)
            continue;
        for (n1 = 0; n1 < n_max(num_shells, ml1); ++n1)
        for (n2 = 0; n2 < n_max(num_shells, ml2); ++n2)
        for (n3 = 0; n3 < n_max(num_shells, ml3); ++n3)
        for (n4 = 0; n4 < n_max(num_shells, ml4); ++n4) {
            ix.n1  =  n1;
            ix.ml1 = ml1;
            ix.n2  =  n2;
            ix.ml2 = ml2;
            ix.n3  =  n3;
            ix.ml3 = ml3;
            ix.n4  =  n4;
            ix.ml4 = ml4;
            result = clh2_element(ctx, &ix);
            printf("  %3d %3d %3d %3d %3d %3d %3d %3d %22.14e\n",
                   n1, ml1, n2, ml2, n3, ml3, n4, ml4, result);
        }
    }

    /* cleanup */
    clh2_ctx_destroy(ctx);
    return 0;
}
