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

The library can be built as either a static library or a shared library.  By
default (i.e. with `make` without any arguments), both are built.

In this section, substitute <code><var>{IMPL}</var></code> with your chosen
implementation: either `am` or `openfci`.

To create symbolic links to your chosen implementation, namely

  - `libcoulombho2d.a` and
  - `libcoulombho2d.so`,

run:

<pre><code>make IMPL=<var>{IMPL}</var> alias</code></pre>

If the macro <code>IMPL</code> is not given, it defaults to `openfci`.

### Static library

A static library can be built using:

<pre><code>make static-<var>{IMPL}</var></code></pre>

This will create the following library in the `dist` directory:

  - <code>libcoulombho2d_<var>{IMPL}</var>.a</code>

Note that for the `openfci` implementation, the *user* of the library must
additionally link against a LAPACK implementation as well as the C++ standard
library (e.g. `-llapack -lstdc++`).

### Shared libary

A shared library can be built using:

<pre><code>make shared-<var>{IMPL}</var></code></pre>

This will create the following libraries in the `dist` directory:

  - <code>libcoulombho2d_<var>{IMPL}</var>.so.<var>VERSION</var></code>
  - <code>libcoulombho2d_<var>{IMPL}</var>.so</code> (a symbolic link to the
    above library)

## Installation

The library can be installed via:

<pre><code>make DESTDIR=<var>{DESTDIR}</var> install</code></pre>

Substitute <code><var>{DESTDIR}</var></code> with the destination directory.
If the macro `DESTDIR` is not provided, it defaults to `/usr/local`.

## Example

The following code calculates a matrix element.  For efficiency, the context
object `ctx` should be reused when performing multiple matrix element
calculations (but only within a single thread).

~~~c
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
~~~

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
