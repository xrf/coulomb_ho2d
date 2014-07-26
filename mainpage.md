# Coulomb HO2D {#mainpage}

This code calculates the matrix elements of the Coulomb repulsion operator in
the basis of a two-dimensional harmonic oscillator (HO2D).

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

The library can be built as either a static library or a shared library.

### Static library

A static library can be built using `make lib-{IMPL}` where `{IMPL}` is either
`am` or `openfci`.  The library will be placed in `dist` directory and named
`libcoulombho2d_{IMPL}.a`.  Note that for the `openfci` implementation, the
user is required to link against LAPACK and the C++ standard library
(e.g. `-llapack -lstdc++`).

Alternatively, run the `make use-{IMPL}` command to build the same library but
instead named as `libcoulombho2d.a`.

### Shared libary

A shared library can be built using `make so-{IMPL}` where `{IMPL}` is either
`am` or `openfci`.  The library will be placed in `dist` directory and named
`libcoulombho2d_{IMPL}.so.{VERSION}` where `{VERSION}` is the version of the
library.  Additionally, a symbolic link named `libcoulombho2d_{IMPL}.so` is
created as an alias.

The `make use-{IMPL}` command will create a symbolic link to the given
implementation named `libcoulombho2d.so`.

## Example

The following code calculates a matrix element.  For efficiency, the context
object `ctx` should be reused when performing multiple matrix element
calculations (but only within a single thread).

@code{.c}
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
@endcode

## API

The library provides a C API.  For convenience, it also provides C++ interface
as header-only wrapper around the C API.  Both are contained in
`coulomb_ho2d.h`.  For more information, see:

  - [Main interface](group__main.html)
  - [Legacy interface](group__compat.html)

[1]: http://dx.doi.org/10.1088/0953-8984/10/3/013
[2]: http://arxiv.org/abs/0810.2644
[3]: http://folk.uio.no/simenkva/openfci.shtml
[4]: http://sourceforge.net/projects/lpp
[5]: http://netlib.org/lapack
