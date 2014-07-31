# Coulomb HO2D [![Build status](https://travis-ci.org/xrf/coulomb_ho2d.svg?branch=master)](https://travis-ci.org/xrf/coulomb_ho2d)

Calculation of Coulomb interaction matrix elements in the basis of a
two-dimensional harmonic oscillator (HO2D).  See the [full documentation][6]
for details.

## Quick start

Download and unpack the [latest release][7], then run:

```sh
make
make install        # with sudo if needed
```

This will install the libaries to `/usr/local/lib` and the headers to
`/usr/local/include`.  You can find an [example of how to use the library][8]
in the source tree.

## Mathematical details

The code calculates the spatial part of the Coulomb interaction matrix
element:

![matrix element][9]

Here, `φ[n, ml]` denotes the eigenfunction of a two-dimensional harmonic
oscillator with principal quantum number `n` and angular momentum projection
`ml`.  In polar coordinates, these functions may be defined in terms of the
[associated Laguerre polynomials][11] `L`:

![basis function][10]

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

[1]:  http://dx.doi.org/10.1088/0953-8984/10/3/013
[2]:  http://arxiv.org/abs/0810.2644
[3]:  http://folk.uio.no/simenkva/openfci.shtml
[4]:  http://sourceforge.net/projects/lpp
[5]:  http://netlib.org/lapack
[11]: https://en.wikipedia.org/wiki/Laguerre_polynomials#Generalized_Laguerre_polynomials

[6]:  http://xrf.github.io/coulomb_ho2d
[7]:  https://github.com/xrf/coulomb_ho2d/releases
[8]:  https://github.com/xrf/coulomb_ho2d/blob/master/example.c
[9]:  https://github.com/xrf/coulomb_ho2d/raw/master/equation-matrix-element.png
[10]: https://github.com/xrf/coulomb_ho2d/raw/master/equation-basis-function.png
