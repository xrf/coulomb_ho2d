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
#ifndef NDEBUG
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#endif
#include <limits.h>
#include <math.h>
#ifndef NO_STDINT_H
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#endif
#include <stdlib.h>
#include "coulomb_ho2d.h"
#ifdef _MSC_VER
#  define NOINLINE __declspec(noinline)
#  define PURE
#else
#  define NOINLINE __attribute__((noinline))
#  define PURE     __attribute__((pure))
#endif
#ifndef NAN
#  define NAN sqrt(-1.)
#endif
#ifdef __cplusplus
extern "C" {
#endif

/* Find the fastest unsigned integer type at least as large as `unsigned`. */
typedef
#if   UINT_FAST16_MAX >= UINT_MAX
      uint_fast16_t
#elif UINT_FAST32_MAX >= UINT_MAX
      uint_fast32_t
#elif UINT_FAST64_MAX >= UINT_MAX
      uint_fast64_t
#else
      unsigned
#endif
      uintf;

#ifndef __cplusplus
/* Declare the built-in `tgamma` in case it isn't declared already.  Avoid
   declaring this in C++ because those may contain an exception specifier. */
double tgamma(double);
#endif

/* Prints a prefixed warning to the standard error. */
static void warn(const char *format, ...) {
#ifdef NDEBUG
    (void) format;
#else
    va_list args;
    va_start(args, format);
    fprintf(stderr, "coulomb_ho2d: ");
    vfprintf(stderr, format, args);
    fflush(stderr);
    va_end(args);
#endif
}

/* Resizes an array of `double`s to have at least `count` elements.  If the
   array is null, then a new array is allocated.  Returns zero on success.
   Returns nonzero on failure, and leaves the array unchanged. */
static int resize_arrayd(double **array, size_t count) {
    typedef double type;
    size_t new_size = count * sizeof(type);
    if (count) {
        type *old_array = *array;
        *array = (type *) realloc(*array, new_size);
        if (!*array) {                  /* realloc failed */
            *array = old_array;
            return 1;
        }
    } else {                            /* cache doesn't exist */
        *array = (type *) malloc(new_size);
        if (!*array)                    /* malloc failed */
            return 1;
    }
    return 0;
}

/* A structure used to speed up the calculations. */
struct clh2_cache {
    double *pow2;
    size_t  pow2_size;
    double *rgamma2;
    size_t  rgamma2_size;
    double *rfac;
    size_t  rfac_size;
};

/* Global cache. */
static struct clh2_cache global_cache;

/* Whether cleanup on exit has been registered. */
static int cleanup_registered;

/* Frees the cache. */
static void free_cache(struct clh2_cache *cache) {
    /* pow2 */
    free(cache->pow2);
    cache->pow2_size = 0;
    /* tgamma2 */
    free(cache->rgamma2);
    cache->rgamma2_size = 0;
    /* rfac */
    free(cache->rfac);
    cache->rfac_size = 0;
}

/* Cleans up the global cache. */
static void global_cache_cleanup(void) {
    free_cache(&global_cache);
}

/* Ensure the global cache is cleaned up before exit.  Always returns `1`. */
static int register_cleanup(void) {
    if (!cleanup_registered) {
        atexit(&global_cache_cleanup);
        cleanup_registered = 1;
    }
    return 1;
}

/* Returns the `n`-th element in the array `m` (declared as a pure function
   for optimization purposes). */
static PURE double pure_at(const double *array, uintf n) { return array[n]; }

/* Calculates `1 / 2^(n / 2)`. */
static PURE double pow2(uintf n)    { return pow(sqrt(.5), n); }

/* Calculates `1 / Γ[n / 2]`. */
static PURE double rgamma2(uintf n) { return 1 / tgamma(.5 * n); }

/* Calculates `1 / n!`. */
static PURE double rfac(uintf n)    { return 1 / tgamma(n + 1); }

/* Calculates `(-1) ^ n`. */
static PURE int minuspow(uintf n)   { return n % 2 ? -1 : 1; }

/* Builds the cache for the given function. */
#define CACHE_LOADER(func)                                                  \
    static NOINLINE                                                         \
    int func ## _load(double **cache, uintf *size, uintf new_max) {         \
        size_t minimum = 2;                                                 \
        size_t new_size = (new_max <= minimum ? minimum : new_max) * 2;     \
        if (resize_arrayd(cache, new_size))                                 \
            return 1;                                                       \
        for (; *size != new_size; ++*size)                                  \
            (*cache)[*size] = func(*size);                                  \
        return 0;                                                           \
    }
CACHE_LOADER(pow2)
CACHE_LOADER(rgamma2)
CACHE_LOADER(rfac)

/* Builds the cache for the functions up to the given maximums. */
static int load_cache(struct clh2_cache *cache,
                      size_t pow2_max,
                      size_t rgamma2_max,
                      size_t rfac_max) {
    /* pow2 */
    if (cache->pow2_size <= pow2_max &&
        register_cleanup() &&           /* only needs to be called here */
        pow2_load(&cache->pow2, &cache->pow2_size, pow2_max))
        return 1;
    /* rgamma2 */
    if (cache->rgamma2_size <= rgamma2_max &&
        rgamma2_load(&cache->rgamma2, &cache->rgamma2_size, rgamma2_max))
        return 1;
    /* rfac */
    if (cache->rfac_size <= rfac_max &&
        rfac_load(&cache->rfac, &cache->rfac_size, rfac_max))
        return 1;
    return 0;
}

/* Macros to make the code more readable. */
#define rfac(x)     pure_at(cache->rfac,    (x))
#define rgamma2(x)  pure_at(cache->rgamma2, (x))
#define pow2(x)     pure_at(cache->pow2,    (x))

/* Calculates the Coulomb matrix element. */
static double calculate(struct clh2_cache *cache,
                        unsigned n1, int m1, unsigned n2, int m2,
                        unsigned n3, int m3, unsigned n4, int m4) {
    double result;
    uintf N, NM1, M, M1, M2, M3, M4, j1, j2, j3, j4;
    if (m1 + m2 != m3 + m4)
        return 0;
    N = n1 + n2 + n3 + n4;
    M1 = abs(m1);
    M2 = abs(m2);
    M3 = abs(m3);
    M4 = abs(m4);
    M = M1 + M2 + M3 + M4;
    /* calculate the the maximum possible arguments for `pow2`, `rgamma2`, and
       `rfac`, and then precompute them if not cached already */
    NM1 = N + M + 1;
    if (load_cache(cache, N + NM1, 1 + N + NM1, 2 * NM1)) {
        warn("can't allocate memory\n");
        return NAN;
    }
    /* calculate using the Anisimovas & Matulis formula */
    result = 0;
    for (j1 = 0; j1 <= n1; ++j1)
    for (j4 = 0; j4 <= n4; ++j4)
    for (j2 = 0; j2 <= n2; ++j2)
    for (j3 = 0; j3 <= n3; ++j3) {
        double sum = 0;
        uintf G1, g1, g2, g3, g4, l1, l2, l3, l4, l12, L;
        g1 = j1 + j4 + (M1 + m1 + M4 - m4) / 2;
        g2 = j2 + j3 + (M2 + m2 + M3 - m3) / 2;
        g3 = j2 + j3 + (M3 + m3 + M2 - m2) / 2;
        g4 = j1 + j4 + (M4 + m4 + M1 - m1) / 2;
        /* note: G1 is always odd */
        G1 = ((j1 + j4) + (j2 + j3)) * 2 + M + 1;
        for (l1 = 0; l1 <= g1; ++l1) {
            double sum1 = 0;
            sum = -sum;                 /* alternating sum */
            for (l2 = 0; l2 <= g2; ++l2) {
                uintf la, lb;
                double sum2 = 0;
                l12 = l1 + l2;
                L = l12 * 2;
                /* this is just a complicated way to restrict the sum */
                la = l12 > g3 ? l12 - g3 : 0;
                lb = g4 < l12 ? g4 : l12;
                if (la % 2)
                    sum2 = -sum2;       /* prepare the sign */
                for (l4 = la; l4 <= lb; ++l4) {
                    l3 = l12 - l4;
                    sum2 += rfac(l3) * rfac(l4)
                          * rfac(g3 - l3) * rfac(g4 - l4)
                          * rfac(l2) * rfac(g2 - l2)
                          * rfac(l1) * rfac(g1 - l1);
                    sum2 = -sum2;       /* alternating sum */
                }
                if (lb % 2 == 0)
                    sum2 = -sum2;       /* restore the sign */
                sum1 += sum2 / rgamma2(2 + L) / rgamma2(G1 - L);
            }
            sum += sum1;
         }
        if (g1 % 2)
            sum = -sum;                 /* restore the sign */
        result += minuspow((j1 + j4) + (j2 + j3)) * sum
                * rfac(j1) * rfac(j2) * rfac(j3) * rfac(j4)
                * rfac(n1 - j1) * rfac(n2 - j2)
                * rfac(n3 - j3) * rfac(n4 - j4)
                * rfac(j1 + M1) * rfac(j2 + M2)
                * rfac(j3 + M3) * rfac(j4 + M4)
                * pow2(G1) / (rfac(g1) * rfac(g2) * rfac(g3) * rfac(g4));
    }
    return result * minuspow(M2 + M3)
         / (rfac(n1 + M1) * rfac(n2 + M2) * rfac(n3 + M3) * rfac(n4 + M4))
         * sqrt((rfac(n1 + M1) * rfac(n2 + M2) * rfac(n3 + M3) * rfac(n4 + M4))
                / (rfac(n1) * rfac(n2) * rfac(n3) * rfac(n4)));
}

/* Calculates the matrix element (thread-unsafe). */
double coulomb_ho2d(unsigned n1, int m1, unsigned n2, int m2,
                    unsigned n3, int m3, unsigned n4, int m4) {
    return calculate(&global_cache, n1, m1, n2, m2, n3, m3, n4, m4);
}

/* Calculates the matrix element (thread-safe). */
double coulomb_ho2d_r(struct clh2_cache *cache,
                      unsigned n1, int m1, unsigned n2, int m2,
                      unsigned n3, int m3, unsigned n4, int m4) {
    if (!cache) {
        warn("cache can't be NULL\n");
        return NAN;
    }
    return calculate(cache, n1, m1, n2, m2, n3, m3, n4, m4);
}

/* Allocates the cache and initializes it to zero. */
struct clh2_cache *clh2_cache_create(void) {
    struct clh2_cache *cache;
    cache = (struct clh2_cache *) calloc(1, sizeof(struct clh2_cache));
    if (!cache)
        warn("can't allocate memory to create cache\n");
    return cache;
}

/* Frees the cache. */
void clh2_cache_destroy(struct clh2_cache *cache) {
    free_cache(cache);
    free(cache);
}

#ifdef __cplusplus
}
#endif
