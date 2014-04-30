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
#include <limits.h>
#include <stdlib.h>
#ifndef NO_STDINT_H
# define __STDC_LIMIT_MACROS
# include <stdint.h>
#endif
#include "coulomb_ho2d.h"
#include "coulomb_ho2d_utils.inl"
#ifdef _MSC_VER
#  define NOINLINE __declspec(noinline)
#  define PURE
#else
#  define NOINLINE __attribute__((noinline))
#  define PURE     __attribute__((pure))
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

#if !defined(__cplusplus) && !defined(_MSC_VER)
/* Declare the built-in `tgamma` in case it isn't declared already.  Avoid
   declaring this in C++ because those may contain an exception specifier. */
double tgamma(double);
#endif

/* Resizes an array of `double`s to have `count` elements.  If the array is
   `NULL`, then a new array is allocated using `malloc`.  Otherwise, the array
   is resized using `realloc`.  Returns zero on success.  Returns nonzero on
   failure, leaving the array unchanged. */
static int resize_arrayd(double **array, size_t count) {
    typedef double type;
    size_t new_size = count * sizeof(type);
    if (*array) {
        type *old_array = *array;
        *array = (type *) realloc(*array, new_size);
        if (!*array) {                  /* realloc failed */
            *array = old_array;
            return 1;
        }
    } else {                            /* array doesn't exist */
        *array = (type *) malloc(new_size);
        if (!*array)        /* malloc failed */
            return 1;
    }
    return 0;
}

/* A structure used to speed up the calculations. */
struct clh2_ctx {
    double *pow2;
    size_t  pow2_size;
    double *rgamma2;
    size_t  rgamma2_size;
    double *rfac;
    size_t  rfac_size;
};

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
static int load_caches(clh2_ctx *ctx,
                      size_t pow2_max,
                      size_t rgamma2_max,
                      size_t rfac_max) {
    /* pow2 */
    if (ctx->pow2_size <= pow2_max &&
        pow2_load(&ctx->pow2, &ctx->pow2_size, pow2_max))
        return 1;
    /* rgamma2 */
    if (ctx->rgamma2_size <= rgamma2_max &&
        rgamma2_load(&ctx->rgamma2, &ctx->rgamma2_size, rgamma2_max))
        return 1;
    /* rfac */
    if (ctx->rfac_size <= rfac_max &&
        rfac_load(&ctx->rfac, &ctx->rfac_size, rfac_max))
        return 1;
    return 0;
}

/* Allocates the context and initializes it to zero. */
clh2_ctx *clh2_ctx_create(void) {
    clh2_ctx *ctx = (clh2_ctx *) calloc(1, sizeof(*ctx));
    if (!ctx)
        warn("can't allocate memory to create context\n");
    return ctx;
}

/* Frees the context. */
void clh2_ctx_destroy(clh2_ctx *ctx) {
    free(ctx->pow2);
    free(ctx->rgamma2);
    free(ctx->rfac);
    free(ctx);
}

/* Macros to make the code more readable. */
#define rfac(x)     pure_at(ctx->rfac,    (x))
#define rgamma2(x)  pure_at(ctx->rgamma2, (x))
#define pow2(x)     pure_at(ctx->pow2,    (x))

/* Calculates the Coulomb matrix element. */
double clh2_element(clh2_ctx *ctx, const struct clh2_indices *ix) {
    /* Relabel indices in the same order as in the original paper:
       `<1 2||4 3>`.  Hence, the swapping of 3 and 4 here is intentional! */
    unsigned n1 = ix->n1;
    unsigned n2 = ix->n2;
    unsigned n3 = ix->n4;
    unsigned n4 = ix->n3;
    int m1 = ix->ml1;
    int m2 = ix->ml2;
    int m3 = ix->ml4;
    int m4 = ix->ml3;
    int M1_, M2_, M3_, M4_;
    uintf N, NM1, M, M1, M2, M3, M4, j1, j2, j3, j4, k1, k2, k3, k4;
    double result;
    if (m1 + m2 != m3 + m4)
        return 0;
    N = n1 + n2 + n3 + n4;
    M1_ = abs(m1);
    M2_ = abs(m2);
    M3_ = abs(m3);
    M4_ = abs(m4);
    M1 = (uintf) M1_;
    M2 = (uintf) M2_;
    M3 = (uintf) M3_;
    M4 = (uintf) M4_;
    k1 = (uintf) (M1_ + m1 + M4_ - m4) / 2;
    k2 = (uintf) (M2_ + m2 + M3_ - m3) / 2;
    k3 = (uintf) (M3_ + m3 + M2_ - m2) / 2;
    k4 = (uintf) (M4_ + m4 + M1_ - m1) / 2;
    M = M1 + M2 + M3 + M4;
    /* calculate the the maximum possible arguments for `pow2`, `rgamma2`, and
       `rfac`, and then precompute them if not cached already */
    NM1 = N + M + 1;
    if (load_caches(ctx, N + NM1, 1 + N + NM1, 2 * NM1)) {
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
        uintf g1 = j1 + j4 + k1;
        uintf g2 = j2 + j3 + k2;
        uintf g3 = j2 + j3 + k3;
        uintf g4 = j1 + j4 + k4;
        /* note: G1 is always odd */
        uintf G1 = ((j1 + j4) + (j2 + j3)) * 2 + M + 1;
        uintf l1, l2, l3, l4;
        for (l1 = 0; l1 <= g1; ++l1) {
            double sum1 = 0;
            sum = -sum;                 /* alternating sum */
            for (l2 = 0; l2 <= g2; ++l2) {
                double sum2 = 0;
                uintf l12 = l1 + l2;
                uintf L = l12 * 2;
                /* this is just a complicated way to restrict the sum */
                uintf la = l12 > g3 ? l12 - g3 : 0;
                uintf lb = g4 < l12 ? g4 : l12;
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

#ifdef __cplusplus
}
#endif
