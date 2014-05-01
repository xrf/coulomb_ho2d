# Coulomb HO2D

This code calculates the matrix elements of the Coulomb repulsion operator in
the basis of a two-dimensional harmonic oscillator (HO2D).  See the
[full documentation][6] for details.

## Implementations

The library contains two implementations of the same interface:

  - The `am` version, which uses an optimized version of the analytic formula
    in [Anisimovas & Matulis (1998)][1].  This is the simplest implementation
    with no external dependencies (and only requires a C99 compiler), but is
    very slow and inaccurate for higher shells.

  - The `openfci` version, which uses Gaussian quadrature as described in
    [Kvaal (2008)][1].  This implementation is faster and more accurate, but
    requires a C++ compiler as well as several dependencies:

      - [OpenFCI][3]
      - [LAPACK Plus Plus][4]
      - [LAPACK][5]

    The makefile of this library automatically downloads and builds the first
    two dependencies so the user only needs to be handle the LAPACK
    dependency.  If the library is linked using a C compiler, be sure to
    include the C++ standard library as well.

## Building

The library is expected to be built as a static library, although it should be
possible to build it as a shared library as well.

To build the `am` version, run `make lib-am` in the root directory.  This will
build a static library in the `dist` directory named `libcoulombho2d_am.a`.
Alternatively, run the `make use-am` command to build the same library but
instead named as `libcoulombho2d.a`.

Similarly, for the `openfci` version, one can run `make lib-openfci` or `make
use-openfci`.

## Example

The following code calculates a matrix element.  For efficiency, the context
object `ctx` should be reused when performing multiple matrix element
calculations (but only within a single thread).

````c
#include <stdio.h>
#include <coulomb_ho2d.h>
int main(void) {
    struct clh2_indices ix;

    /* create context */
    clh2_ctx *ctx = clh2_ctx_create();
    if (!ctx) return 1;

    /* initialize indices */
    ix.n1  =  0;
    ix.ml1 = -1;
    ix.n2  =  0;
    ix.ml2 =  1;
    ix.n3  =  0;
    ix.ml3 = -2;
    ix.n4  =  0;
    ix.ml4 =  2;

    /* calculate and print matrix element (expected result: 0.303537) */
    printf("%f\n", clh2_element(ctx, &ix));

    /* destroy context */
    clh2_ctx_destroy(ctx);
    return 0;
}
````

[1]: http://dx.doi.org/10.1088/0953-8984/10/3/013
[2]: http://arxiv.org/abs/0810.2644
[3]: http://folk.uio.no/simenkva/openfci.shtml
[4]: http://sourceforge.net/projects/lpp
[5]: http://netlib.org/lapack
[6]: http://xrf.github.io/coulomb_ho2d
