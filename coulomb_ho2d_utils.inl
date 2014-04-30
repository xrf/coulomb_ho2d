#include <math.h>
#ifndef NDEBUG
# include <stdarg.h>
# include <stdio.h>
#endif
#ifndef NAN
#  define NAN (0./0.)
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
