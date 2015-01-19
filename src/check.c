#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <clh2.h>

static const char *provider_ref = "clh2-ref";
static const double abserr = 1e-6;
static int no_ref;

static size_t calc_total(unsigned char n_max, signed char ml_max) {
    return (size_t) (pow(n_max, 4.) * pow(2 * ml_max - 1, 3.));
}

static void ensure(int errnum) {
    if (!errnum)
        return;
    fprintf(stderr, "check: error: %s\n", strerror(errnum));
    exit(EXIT_FAILURE);
}

/* regression test for a bug that only occurs when called in a certain order,
   apparently due to the reuse of the internal state (`QdotInteraction`) */
static void check_weird_bug(const char *provider) {
    const double *out;
    const struct clh2_indicesp ixs[] = {
        {0, 0, 1, 0, 0, 3, 0, -3},
        {0, 0, 1, 0, 2, 1, 2, -1}
    };
    static const size_t count = sizeof(ixs) / sizeof(struct clh2_indicesp);
    ensure(clh2_request(&out, provider, count, ixs));
    clh2_free(count, out);
}

static void verify(const struct clh2_indicesp *ix, double z, double w) {
    if (!(fabs(z - w) < abserr)) {
        fprintf(stderr,
                "failed: <%d %d; %d %d | %d %d; %d %d> = %.7f != %.7f\n",
                ix->n1, ix->ml1, ix->n2, ix->ml2,
                ix->n3, ix->ml3, ix->n4, ix->ml4, z, w);
        exit(EXIT_FAILURE);
    }
}

static void verify_element(const char *provider,
                           unsigned char n1, signed char ml1,
                           unsigned char n2, signed char ml2,
                           unsigned char n3, signed char ml3,
                           unsigned char n4, signed char ml4) {
    const double *z, *w = NULL;
    struct clh2_indicesp ix;
    ix.n1  = n1;
    ix.ml1 = ml1;
    ix.n2  = n2;
    ix.ml2 = ml2;
    ix.n3  = n3;
    ix.ml3 = ml3;
    ix.n4  = n4;
    ix.ml4 = ml4;
    ensure(clh2_request(&z, provider, 1, &ix));
    if (clh2_request(&w, provider_ref, 1, &ix))
        no_ref = 1;
    else
        verify(&ix, *z, *w);
    clh2_free(1, z);
    clh2_free(1, w);
}

static void verify_group(const char *provider,
                         unsigned char n_max, signed char ml_max) {
    const size_t count = calc_total(n_max, ml_max);
    struct clh2_indicesp *ixs;
    const double *zs, *ws = NULL;
    size_t num_nonzero = 0, num_order3 = 0, num_order4 = 0, num_order5 = 0,
           i = 0;
    unsigned char n1, n2, n3, n4;
    signed char ml1, ml2, ml3, ml4;

    printf("group (%u, %d):\n", n_max, ml_max);
    fflush(stdout);

    ixs = (struct clh2_indicesp *) malloc(sizeof(*ixs) * count);
    if (!ixs)
        ensure(ENOMEM);

    for (n1 = 0; n1 < n_max; ++n1)
    for (n2 = 0; n2 < n_max; ++n2)
    for (n3 = 0; n3 < n_max; ++n3)
    for (n4 = 0; n4 < n_max; ++n4)
    for (ml1 = (signed char) (-ml_max + 1); ml1 < ml_max; ++ml1)
    for (ml2 = (signed char) (-ml_max + 1); ml2 < ml_max; ++ml2)
    for (ml3 = (signed char) (-ml_max + 1); ml3 < ml_max; ++ml3) {
        ml4 = (signed char) (ml1 + ml2 - ml3);
        ixs[i].n1  = n1;
        ixs[i].ml1 = ml1;
        ixs[i].n2  = n2;
        ixs[i].ml2 = ml2;
        ixs[i].n3  = n3;
        ixs[i].ml3 = ml3;
        ixs[i].n4  = n4;
        ixs[i].ml4 = ml4;
        ++i;
    }

    ensure(clh2_request(&zs, provider, count, ixs));
    if (clh2_request(&ws, provider_ref, count, ixs))
        no_ref = 1;

    for (i = 0; i != count; ++i) {
        const double z = zs[i];
        if (ws)
            verify(&ixs[i], z, ws[i]);
        if (z > abserr)
            ++num_nonzero;
        if (1e-3 <= z && z < 1e-2)
            ++num_order3;
        if (1e-4 <= z && z < 1e-3)
            ++num_order4;
        if (1e-5 <= z && z < 1e-4)
            ++num_order5;
    }

    clh2_free(count, zs);
    clh2_free(count, ws);

    printf(
        "----------------------------------------\n"
        "total:   ~%.8g\n"
        "nonzero: %2.2f%% (excluding nonconserving elements)\n"
        "  1e-3 to 1e-2:  %2.2f%% \n"
        "  1e-4 to 1e-3:  %2.2f%% \n"
        "  1e-5 to 1e-4:  %2.2f%% \n"
        "----------------------------------------\n",
        (double) count,
        (double) num_nonzero * 100. / (double) count,
        (double) num_order3 * 100. / (double) count,
        (double) num_order4 * 100. / (double) count,
        (double) num_order5 * 100. / (double) count
    );
}

static void check_all(const char *provider) {
    check_weird_bug(provider);
    verify_element(provider, 1, -4, 4, 0, 2, 4, 4, -8);
    verify_group(provider, 4, 2);
    if (no_ref)
        printf("WARNING: no verification is done.\n");
    else
        printf("verification passed.\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        check_all(NULL);
    } else {
        char **arg;
        for (arg = argv + 1; *arg; ++arg)
            check_all(*arg);
    }
    return EXIT_SUCCESS;
}
