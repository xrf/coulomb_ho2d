/*

simple example program that demonstrates the library:

  - when compiling be sure to specify `-lclh2`

  - if you want to try out this program *before* installing the `clh2`
    library, you can do so by running `make example`; otherwise, you will need
    to manually add the `-L` linker flag and modify both `LD_LIBRARY_PATH` and
    `PATH` accordingly

*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* include the clh2 library header */
#include <clh2.h>

int main(void) {
    static const double expected  = 0.303537;
    static const double tolerance = 0.000001;
    static const size_t count     = 1;
    static const char  *provider  = NULL; /* use default provider */

    const double *result;
    double discrepancy;
    int errnum;

    /* initialize indices */
    struct clh2_indicesp indices;
    indices.n1  =  0;
    indices.ml1 = -1;
    indices.n2  =  0;
    indices.ml2 =  1;
    indices.n3  =  0;
    indices.ml3 = -2;
    indices.n4  =  0;
    indices.ml4 =  2;

    /* calculate a single matrix element */
    errnum = clh2_request(&result, provider, count, &indices);
    if (errnum) {
        fprintf(stderr, "example: error: %s\n", strerror(errnum));
        return EXIT_FAILURE;
    }

    /* print the matrix element */
    printf("matrix element = %.6f\n", *result);

    /* check if the result is correct (and not NAN) */
    discrepancy = fabs(*result - expected);
    clh2_free(count, result);
    if (!(discrepancy < tolerance)) {
        fprintf(stderr, "example: doesn't match expected value: %.6f\n",
                expected);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
