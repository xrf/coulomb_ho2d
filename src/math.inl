#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#ifndef UNUSED
# ifdef __GNUC__
#  define UNUSED __attribute__ ((unused))
# else
#  define UNUSED
# endif
#endif

UNUSED static int rf_add(int *z, int x, int y) {
    if ((y > 0 && x > INT_MAX - y) ||
        (y < 0 && x < INT_MIN - y)) return ERANGE;
    *z = x + y;
    return 0;
}

UNUSED static int rf_adds(size_t *z, size_t x, size_t y) {
    if (x > SIZE_MAX - y) return ERANGE;
    *z = x + y;
    return 0;
}

UNUSED static int rf_sub(int *z, int x, int y) {
    if ((y > 0 && x < INT_MIN + y) ||
        (y < 0 && x > INT_MAX + y)) return ERANGE;
    *z = x - y;
    return 0;
}

UNUSED static int rf_subs(size_t *z, size_t x, size_t y) {
    if (x < y) return ERANGE;
    *z = x - y;
    return 0;
}

UNUSED static int rf_neg(int *z, int x) {
    if (-INT_MAX != INT_MIN && x == INT_MIN) return ERANGE;
    *z = -x;
    return 0;
}

UNUSED static int rf_mul(int *z, int x, int y) {
    if (x > 0) {
        if (y > 0) {
            if (x > INT_MAX / y) return ERANGE;
        } else {
            if (y < INT_MIN / x) return ERANGE;
        }
    } else {
        if (y > 0) {
            if (x < INT_MIN / y) return ERANGE;
        } else {
            if (x && y < INT_MAX / x) return ERANGE;
        }
    }
    *z = x * y;
    return 0;
}

UNUSED static int rf_muls(size_t *z, size_t x, size_t y) {
    if (y && x > SIZE_MAX / y) return ERANGE;
    *z = x * y;
    return 0;
}

UNUSED static int rf_div(int *z, int x, int y) {
    if (!y || (-INT_MAX != INT_MIN && x == INT_MIN && y == -1)) return ERANGE;
    *z = x / y;
    return 0;
}

UNUSED static int rf_mod(int *z, int x, int y) {
    /* in C11, modulo is undefined if div is not representable (6.5.5) */
    if (!y || (-INT_MAX != INT_MIN && x == INT_MIN && y == -1)) return ERANGE;
    *z = x % y;
    return 0;
}
