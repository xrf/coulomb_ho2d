#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <clh2.h>

/* chosen partly to avoid overflow errors */
#define NUM_SHELLS_MAX 100

/* calculate the maximum `n` that lies within the required shells */
static unsigned char n_max(unsigned char num_shells, signed char ml) {
    unsigned char abs_ml = (unsigned char) abs(ml);
    if (abs_ml >= num_shells)
        abort();
    return (unsigned char) (num_shells - abs_ml + 1) / 2;
}

#define ITERATE(block)                                          \
    for (ml1 = ml_min; ml1 < num_shells; ++ml1)                 \
    for (ml2 = ml_min; ml2 < num_shells; ++ml2)                 \
    for (ml3 = ml_min; ml3 < num_shells; ++ml3) {               \
        const int ml4_int = (ml1 + ml2 - ml3);                  \
        if (ml4_int < 1 - num_shells || ml4_int >= num_shells)  \
            continue;                                           \
        ml4 = (signed char) ml4_int;                            \
        for (n1 = 0; n1 < n_max(num_shells, ml1); ++n1)         \
        for (n2 = 0; n2 < n_max(num_shells, ml2); ++n2)         \
        for (n3 = 0; n3 < n_max(num_shells, ml3); ++n3)         \
        for (n4 = 0; n4 < n_max(num_shells, ml4); ++n4)         \
            block                                               \
    }

int main(int argc, char **argv) {
    struct clh2_indicesp *indices;
    const double *results;
    size_t count, size = 0, i = 0;
    long num_shells_long;
    unsigned char num_shells, n1, n2, n3, n4;
    signed char ml1, ml2, ml3, ml4, ml_min;
    char *arg_end;
    int errnum;

    /* print usage info if arguments aren't provided */
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: tabulate NUM_SHELLS [PROVIDER]\n"
                        "  where NUM_SHELLS is the number of shells\n"
                        "    and PROVIDER   is the tabulation provider\n");
        return EXIT_FAILURE;
    }

    /* obtain the number of shells */
    num_shells_long = strtol(argv[1], &arg_end, 10);
    if (argv[1] == arg_end) {
        fprintf(stderr, "tabulate: 1st argument invalid: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    if (num_shells_long < 0 || num_shells_long > NUM_SHELLS_MAX) {
        fprintf(stderr, "tabulate: 1st argument out of range: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    num_shells = (unsigned char) num_shells_long;
    ml_min     = (signed char) (1 - num_shells);

    /* allocate memory for indices */
    ITERATE({
        size += sizeof(*indices);
        if (size < sizeof(*indices)) {
            fprintf(stderr, "tabulate: not enough memory\n");
            return EXIT_FAILURE;
        }
    });
    indices = (struct clh2_indicesp *) malloc(size);
    if (!indices) {
        fprintf(stderr, "tabulate: failed to allocate ~%.8g KiB\n",
                (double) size / 1024.);
        return EXIT_FAILURE;
    }

    /* print header */
    count = size / sizeof(*indices);
    printf("# Coulomb matrix elements for up to %d shell(s)\n"
           "# Total of ~%.8g row(s)\n"
           "# %3s %3s %3s %3s %3s %3s %3s %3s %22s\n",
           num_shells, (double) count,
           "n1", "ml1", "n2", "ml2",
           "n3", "ml3", "n4", "ml4", "value");

    /* calculate matrix elements */
    ITERATE({
        indices[i].n1  = n1;
        indices[i].ml1 = ml1;
        indices[i].n2  = n2;
        indices[i].ml2 = ml2;
        indices[i].n3  = n3;
        indices[i].ml3 = ml3;
        indices[i].n4  = n4;
        indices[i].ml4 = ml4;
        ++i;
    });
    errnum = clh2_request(&results, argv[2], count, indices);
    if (errnum) {
        fprintf(stderr, "tabulate: error: %s\n", strerror(errnum));
        free(indices);
        return EXIT_FAILURE;
    }

    /* print results */
    for (i = 0; i != count; ++i) {
        const struct clh2_indicesp *p = &indices[i];
        printf("  %3d %3d %3d %3d %3d %3d %3d %3d %22.14e\n",
               (int) p->n1, (int) p->ml1, (int) p->n2, (int) p->ml2,
               (int) p->n3, (int) p->ml3, (int) p->n4, (int) p->ml4,
               results[i]);
    }

    clh2_free(count, results);
    free(indices);
    return EXIT_SUCCESS;
}
