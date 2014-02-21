#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <coulomb_ho2d.h>
#ifndef NOREF
#include "coulomb_ref.h"
#else
double coulomb_ref(...) { return NAN; }
#endif

static const double abserr = 1e-7;

double tick() {
    timespec t;
    if (clock_gettime(CLOCK_MONOTONIC, &t))
        return 1 / 0.;
    return t.tv_sec + t.tv_nsec * 1e-9;
}

template<class Func>
void loop(unsigned n_max, int ml_max, Func f) {
    for (unsigned n1 = 0; n1 < n_max; ++n1)
    for (unsigned n2 = 0; n2 < n_max; ++n2)
    for (unsigned n3 = 0; n3 < n_max; ++n3) {
    for (unsigned n4 = 0; n4 < n_max; ++n4)
    for (int ml1 = -ml_max + 1; ml1 < ml_max; ++ml1)
    for (int ml2 = -ml_max + 1; ml2 < ml_max; ++ml2)
    for (int ml3 = -ml_max + 1; ml3 < ml_max; ++ml3) {
        int ml4 = ml1 + ml2 - ml3;
        f(n1, ml1, n2, ml2, n3, ml3, n4, ml4);
    }
    }
}

template<class Func>
void tloop(unsigned n_max, int ml_max, Func f) {
    const unsigned num_total = pow(n_max, 4) * pow(2 * ml_max - 1, 3);
    unsigned num_done = 0;
    double init_time = tick();
    double prev = init_time;
    double prev_progress = 0;
    for (unsigned n1 = 0; n1 < n_max; ++n1)
    for (unsigned n2 = 0; n2 < n_max; ++n2)
    for (unsigned n3 = 0; n3 < n_max; ++n3) {
    for (unsigned n4 = 0; n4 < n_max; ++n4)
    for (int ml1 = -ml_max + 1; ml1 < ml_max; ++ml1)
    for (int ml2 = -ml_max + 1; ml2 < ml_max; ++ml2)
    for (int ml3 = -ml_max + 1; ml3 < ml_max; ++ml3) {
        int ml4 = ml1 + ml2 - ml3;
        f(n1, ml1, n2, ml2, n3, ml3, n4, ml4);
        ++num_done;
    }
    double now = tick();
    double progress = num_done * 100. / num_total;
    if (now - prev > 1 && progress - prev_progress > .1) {
        printf("... %2.1f%% (%.1f s elapsed)\n", progress, now - init_time);
        fflush(stdout);
        prev_progress = progress;
        prev = now;
    }
    }
    double final_time = tick();
    printf("time:    %.3f s\n", final_time - init_time);
    fflush(stdout);
}

double verify_case(unsigned n1, int ml1, unsigned n2, int ml2,
                   unsigned n3, int ml3, unsigned n4, int ml4,
                   unsigned* fail_counter = 0) {
    double z = coulomb_ho2d(n1, ml1, n2, ml2, n4, ml4, n3, ml3);
    double w = coulomb_ref(n1, ml1, n2, ml2, n4, ml4, n3, ml3);
    if (!isfinite(z) || fabs(z - w) > abserr) {
        fprintf(stderr, "failed: <%d %d; %d %d | %d %d; %d %d> "
                "= %f != %f\n", n1, ml1, n2, ml2, n3, ml3, n4, ml4, z, w);
        fflush(stderr);
        if (fail_counter) {
            ++*fail_counter;
            if (*fail_counter > 30) {
                fprintf(stderr, "Too many failures ... bailing.\n");
                exit(1);
            }
        }
    }
    return z;
}

static void verify(unsigned n_max, int ml_max) {
    const unsigned num_total = pow(n_max, 4) * pow(2 * ml_max - 1, 3);
    unsigned num_failed = 0;
    unsigned num_nonzero = 0;
    unsigned num_order3 = 0;
    unsigned num_order4 = 0;
    unsigned num_order5 = 0;
    printf("case (%d, %d):\n", n_max, ml_max);
    fflush(stdout);
    tloop(n_max, ml_max, [&](unsigned n1, int ml1, unsigned n2, int ml2,
                             unsigned n4, int ml4, unsigned n3, int ml3) {
        double z = verify_case(n1, ml1, n2, ml2, n3, ml3, n4, ml4,
                               &num_failed);
        if (z > abserr)
            ++num_nonzero;
        if (1e-3 <= z && z < 1e-2)
            ++num_order3;
        if (1e-4 <= z && z < 1e-3)
            ++num_order4;
        if (1e-5 <= z && z < 1e-4)
            ++num_order5;
          });
#ifndef NOREF
    if (num_failed) {
        printf("*** FAILED ***:  %2.2f%% (%d)\n",
               num_failed * 100. / num_total, num_failed);
        exit(1);
    } else {
        printf("passed (ok).\n");
    }
#else
    printf("note: no verification is done.\n");
#endif
    printf(
        "----------------------------------------\n"
        "total:   %d\n"
        "nonzero: %2.2f%% (excluding nonconserving elements)\n"
        "  1e-3 to 1e-2:  %2.2f%% \n"
        "  1e-4 to 1e-3:  %2.2f%% \n"
        "  1e-5 to 1e-4:  %2.2f%% \n"
        "----------------------------------------\n",
        num_total,
        num_nonzero * 100. / num_total,
        num_order3 * 100. / num_total,
        num_order4 * 100. / num_total,
        num_order5 * 100. / num_total
    );
    fflush(stdout);
}

static void profile(unsigned n_max, int ml_max, unsigned samples = 10) {
    const unsigned num_total = pow(n_max, 4) * pow(2 * ml_max - 1, 3);
    unsigned max_samples = samples;
    double time_avg = 0;
    double time_m2 = 0;
    printf("case (%d, %d) x%d:\n", n_max, ml_max, samples);
    fflush(stdout);
    for (; samples; --samples) {
        struct clh2_cache *cache = clh2_cache_create();
        assert(cache);
        double t0 = tick();
        loop(n_max, ml_max, [&](unsigned n1, int ml1, unsigned n2, int ml2,
                                unsigned n4, int ml4, unsigned n3, int ml3) {
            volatile auto z =
#ifndef USE_LOCAL_CACHE
                coulomb_ho2d(
#else
                coulomb_ho2d_r(cache,
#endif
                    n1, ml1, n2, ml2, n4, ml4, n3, ml3);
            (void) z;
        });
        double t1 = tick();
        double time = t1 - t0;
        // update mean / m2 (which is used for stdev)
        double d = time - time_avg;
        time_avg += d / (max_samples - samples + 1);
        time_m2 += d * (time - time_avg); // not the same as `d * d`
        printf(".");
        fflush(stdout);
        clh2_cache_destroy(cache);
    }
    double time_stdev2 = sqrt(time_m2 / (max_samples - 1)) * 2;
    printf("\n");
    printf("----------------------------------------\n");
    printf("time per element: %8.2f us\n", time_avg * 1e6 / num_total);
    printf("  stdev * 2:      %8.2f us\n", time_stdev2 * 1e6 / num_total);
    printf("total: %d\n", num_total);
    printf("----------------------------------------\n");
    fflush(stdout);
}

int main() {
    verify_case(1, -4, 4, 0, 2, 4, 4, -8);
    verify(4, 2);
    profile(2, 3, 40);
    profile(3, 3, 10);
    profile(3, 6, 2);
    profile(4, 2, 2);
    return 0;
}
