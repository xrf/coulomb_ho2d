Coulomb HO2D
============

*Code to calculate the Coulomb repulsion matrix elements in the basis of a
two-dimensional harmonic oscillator.*

Getting started
---------------

The code itself is pretty self-contained.  There are only two important files:
`coulomb_ho2d.h` and `coulomb_ho2d.c`.  You'll need to compile
`coulomb_ho2d.c` with a C99 or C++11 compiler, although it will probably work
in an older compiler with some minor fixes (see docs for details).

Linking is simple: the code itself doesn't depend on anything else other than
the standard library (and the standard math library, of course).

The API is documented in the [generated documentation][1], or you can just
peek into the header file and read it from there.

  [1]: http://rufw.github.io/coulomb_ho2d
