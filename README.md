# Coulomb HO2D [![Build status][ci]][ca]

[ca]: https://travis-ci.org/xrf/coulomb_ho2d
[ci]: https://travis-ci.org/xrf/coulomb_ho2d.svg?branch=master

Calculation of Coulomb interaction matrix elements in the basis of a
two-dimensional harmonic oscillator (HO2D).  See the [full documentation][6]
for details.

## Quick start

Download and unpack the [latest release][7], then run:

    make
    make install        # with sudo if needed

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

The library contains the `am` implementation, which uses an optimized version
of the analytic formula in [Anisimovas & Matulis (1998)][1].  This is the
simplest implementation with no external dependencies (and only requires a C99
compiler), but is very slow and inaccurate for higher shells.

## Building

The library can be built as either a static library or a shared library.  By
default (i.e. with `make` without any arguments), both are built.

To create symbolic links to your chosen implementation, namely

  - `libcoulombho2d.a` and
  - `libcoulombho2d.so`,

run:

    make alias

### Static library

A static library can be built using:

    make static

This will create the following library in the `dist` directory:

  - <code>libcoulombho2d.a</code>

### Shared libary

A shared library can be built using:

    make shared

This will create the following libraries in the `dist` directory:

  - `libcoulombho2d_am.so`
  - `libcoulombho2d.so`

## Installation

The library can be installed via:

<pre><code>make DESTDIR=<var>{DESTDIR}</var> install</code></pre>

Substitute <code><var>{DESTDIR}</var></code> with the destination directory.
If the macro `DESTDIR` is not provided, it defaults to `/usr/local`.

[1]:  http://dx.doi.org/10.1088/0953-8984/10/3/013
[11]: https://en.wikipedia.org/wiki/Laguerre_polynomials#Generalized_Laguerre_polynomials

[6]:  http://xrf.github.io/coulomb_ho2d
[7]:  https://github.com/xrf/coulomb_ho2d/releases
[8]:  https://github.com/xrf/coulomb_ho2d/blob/master/src/example.c
[9]:  https://github.com/xrf/coulomb_ho2d/raw/master/equation-matrix-element.png
[10]: https://github.com/xrf/coulomb_ho2d/raw/master/equation-basis-function.png
