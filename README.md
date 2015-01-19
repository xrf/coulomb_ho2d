# Coulomb HO2D [![Build status][ci]][ca]

**Quick links:** [documentation][dc], [releases][rl].

Calculation of Coulomb interaction matrix elements in the basis of a
two-dimensional harmonic oscillator (HO2D).

## Installation

Download and unpack the [latest release][rl], then run:

    make PREFIX=/usr/local install

Replace `/usr/local` with wherever you want it to be installed.

## Mathematical details

The code calculates the spatial part of the Coulomb interaction matrix
element:

![matrix element][me]

Here, `φ[n, ml]` denotes the eigenfunction of a two-dimensional harmonic
oscillator with principal quantum number `n` and angular momentum projection
`ml`.  In polar coordinates, these functions may be defined in terms of the
[associated Laguerre polynomials][lp] `L`:

![basis function][bf]

## Implementations

The library is designed to be extensible: calculation of the the matrix
elements is fulfilled via a *tabulation provider*, which is an executable that
understands a certain file format and protocol (for details, see `protocol.h`
and `clh2-am.c`).

The library has a default provider called `clh2-am` that is installed with the
package.  This provider use the analytic formula in
[Anisimovas & Matulis (1998)][am], which yields a relatively simple
implementation with no external dependencies.  However, it can be quite slow
and may be inaccurate for higher shells.  You can substitute another provider
easily by setting the `provider` argument.

[ca]: https://travis-ci.org/xrf/coulomb_ho2d
[ci]: https://travis-ci.org/xrf/coulomb_ho2d.svg?branch=master
[dc]: https://xrf.github.io/coulomb_ho2d
[rl]: https://github.com/xrf/coulomb_ho2d/releases
[eg]: https://github.com/xrf/coulomb_ho2d/blob/master/src/example.c
[me]: https://github.com/xrf/coulomb_ho2d/raw/master/equation-matrix-element.png
[bf]: https://github.com/xrf/coulomb_ho2d/raw/master/equation-basis-function.png

[am]: http://dx.doi.org/10.1088/0953-8984/10/3/013
[lp]: https://en.wikipedia.org/wiki/Laguerre_polynomials#Generalized_Laguerre_polynomials
