#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>
#include <dlfcn.h>
#include <time.h>
#include <coulomb_ho2d.h>

static const double abserr = 1e-6;

double (*coulomb_ref)(unsigned, int, unsigned, int,
                      unsigned, int, unsigned, int);

double coulomb_nop(unsigned, int, unsigned, int,
                   unsigned, int, unsigned, int) { return NAN; }

class dl {
    void *_handle;
    dl(const dl &);
    dl *operator=(const dl &);
public:

    dl(const char *file, int mode = RTLD_NOW | RTLD_LOCAL)
        : _handle(dlopen(file, static_cast<int>(mode))) {
        if (!_handle)
            throw std::runtime_error("dl::dl failed");
    }

    ~dl() { dlclose(_handle); }

    template<class T>
    void sym(const char *name, T &symbol) {
        void *s = dlsym(_handle, name);
        if (!s)
            throw std::runtime_error("dl::sym failed");
        // funky workaround to suppress warning about function pointer casting
        symbol = reinterpret_cast<T>(*reinterpret_cast<void (**)(void)>(&s));
    }

};

double tick() {
    timespec t;
    if (!clock_gettime(CLOCK_MONOTONIC, &t))
        return t.tv_sec + t.tv_nsec * 1e-9;
    return NAN;
}

template<class Func>
void loop(unsigned n_max, int ml_max, Func f) {
}

unsigned calc_total(unsigned n_max, int ml_max) {
    return static_cast<unsigned>(pow(n_max, 4.) * pow(2 * ml_max - 1, 3.));
}

double verify_case(unsigned n1, int ml1, unsigned n2, int ml2,
                   unsigned n3, int ml3, unsigned n4, int ml4,
                   unsigned* fail_counter = 0) {
    double z = coulomb_ho2d(n1, ml1, n2, ml2, n4, ml4, n3, ml3);
    double w = (*coulomb_ref)(n1, ml1, n2, ml2, n4, ml4, n3, ml3);
    if (!isfinite(z) || fabs(z - w) > abserr) {
        fprintf(stderr, "failed: <%d %d; %d %d | %d %d; %d %d> "
                "= %.7f != %.7f\n", n1, ml1, n2, ml2, n3, ml3, n4, ml4, z, w);
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
    const unsigned num_total = calc_total(n_max, ml_max);
    unsigned num_failed = 0;
    unsigned num_nonzero = 0;
    unsigned num_order3 = 0;
    unsigned num_order4 = 0;
    unsigned num_order5 = 0;
    printf("case (%d, %d):\n", n_max, ml_max);
    fflush(stdout);

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

    if (coulomb_ref != &coulomb_nop) {
        if (num_failed) {
            printf("*** FAILED ***:  %2.2f%% (%d)\n",
                   num_failed * 100. / num_total, num_failed);
            exit(1);
        } else {
            printf("passed (ok).\n");
        }
    } else {
        printf("note: no verification is done.\n");
    }
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
    const unsigned num_total = calc_total(n_max, ml_max);
    unsigned max_samples = samples;
    double time_avg = 0;
    double time_m2 = 0;
    printf("case (%d, %d) x%d:\n", n_max, ml_max, samples);
    fflush(stdout);
    for (; samples; --samples) {
        struct clh2_cache *cache = clh2_cache_create();
        assert(cache);
        double t0 = tick();

        for (unsigned n1 = 0; n1 < n_max; ++n1)
        for (unsigned n2 = 0; n2 < n_max; ++n2)
        for (unsigned n3 = 0; n3 < n_max; ++n3) {
        for (unsigned n4 = 0; n4 < n_max; ++n4)
        for (int ml1 = -ml_max + 1; ml1 < ml_max; ++ml1)
        for (int ml2 = -ml_max + 1; ml2 < ml_max; ++ml2)
        for (int ml3 = -ml_max + 1; ml3 < ml_max; ++ml3) {
            int ml4 = ml1 + ml2 - ml3;
                volatile double z =
#ifndef USE_LOCAL_CACHE
                    coulomb_ho2d(
#else
                    coulomb_ho2d_r(cache,
#endif
                        n1, ml1, n2, ml2, n4, ml4, n3, ml3);
                (void) z;
        }
        }

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

// regression test for a bug that only occurs when called in a certain order,
// apparently due to the reuse of the internal state, i.e.
// `QdotInteraction clh2_ctx::q`
void check_weird_bug() {
    clh2::ctx cache;
    cache.element(clh2_indices(0, 0, 1, 0, 0, 3, 0, -3));
    cache.element(clh2_indices(0, 0, 1, 0, 2, 1, 2, -1));
}

void check_specific_case() {
    static const double expected     = 0.303537;
    static const double expected_err = 0.000001;
    struct clh2_indices ix;
    clh2::ctx cache;
    ix.n1  =  0;
    ix.ml1 = -1;
    ix.n2  =  0;
    ix.ml2 =  1;
    ix.n3  =  0;
    ix.ml3 = -2;
    ix.n4  =  0;
    ix.ml4 =  2;
    double result = cache.element(ix);
    // check if it's correct (and not NAN)
    if (!(fabs(result - expected) < expected_err)) {
        fprintf(stderr, "got %.6f, which doesn't match expected: %.6f\n",
                result, expected);
        throw std::runtime_error("check_specific_case: failed");
    }
    printf("check_specific_case: passed\n");
}

int main() {
    dl *ref_so = 0;
    coulomb_ref = &coulomb_nop;
    try {
        // note that `RTLD_DEEPBIND` is really important here: if not for this
        // flag, the loader will instead find the symbol from `libcoulombho2d`
        // rather than the reference library we want here
        ref_so = new dl("libcoulombho2d_ref.so",
                              RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
        ref_so->sym("coulomb_ho2d", coulomb_ref);
    } catch (...) { }

    check_specific_case();
    check_weird_bug();
    verify_case(1, -4, 4, 0, 2, 4, 4, -8);
    verify(4, 2);
    profile(2, 3, 40);
    profile(3, 3, 10);
    profile(3, 6, 2);
    profile(4, 2, 2);

    delete ref_so;
    return 0;
}
