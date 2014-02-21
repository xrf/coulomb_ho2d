#ifndef DZLVQLVXBFUSJUFLMCUMLUAFPWWJACOMBIVKLDXA
#define DZLVQLVXBFUSJUFLMCUMLUAFPWWJACOMBIVKLDXA
/* The MIT License (MIT)

   Copyright (c) 2014 Fei Yuan.

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.

 */
/** @file

    Calculation of the Coulomb repulsion matrix elements in the basis of a
    two-dimensional harmonic oscillator.

    @mainpage Coulomb HO2D

    This code calculates the Coulomb repulsion matrix elements in the basis of
    a two-dimensional harmonic oscillator.

    ### Usage

    To use the library, simply compile `coulomb_ho2d.c` and statically link it
    with your program.  You can build a shared library too, but this may
    reduce performance.  It is highly recommended that the code be compiled at
    maximum optimization level.  Use unsafe floating-point optimizations
    (e.g. `-ffast-math`) at your own discretion.

    The API documentation can be found in `coulomb_ho2d.h`.

    The code has been tested with GCC and Clang and has minimal dependencies.
    It is written in comforming C99 / C++11 and uses the standard library.  No
    external libraries are required, although you may need to explicitly link
    to the standard math library.

    It can also be compiled in C89 / C++03 provided that suitable replacements
    for `tgamma` and `stdint.h` are supplied.  If `<stdint.h>` is not
    available, define `NO_STDINT_H` instead (this may impact performance).

    The precision of the results is less than `1e-7`.

    @author     Analytic formula from paper by Anisimovas & Matulis (1998).
    @author     Originally by Morten Hjorth-Jensen and Patrick Merlot (2013).
    @author     Optimized by Fei Yuan (2014).

    @copyright  MIT License.

    @see        E. Anisimovas & A. Matulis (1998).
                <i>J. Phys. Condens. Matter</i> <b>10</b> 601.
                http://dx.doi.org/10.1088/0953-8984/10/3/013

 */
#ifdef __cplusplus
extern "C" {
#endif
struct clh2_cache;

/** Calculates the Coulomb matrix element in a 2D harmonic oscillator basis.

    Returns the matrix element:

        <n1, ml1; n2, ml2 | V | n3, ml3; n4, ml4>

    where `V` is the two-particle Coulomb repulsion operator.  The matrix
    element is not antisymmetrized and does not consider spin.

    An analytic expression is used to calculate the matrix element (see the
    referenced paper by Anisimovas & Matulis).

    @param n1, n2, n3, n4
               The principal quantum numbers in the 2D harmonic oscillator
               basis and can be any nonnegative number.

    @param ml1, ml2, ml3, ml4
               The angular momentum projection quantum number and can be any
               integer.

    @return    The value of the matrix element, or `NAN` if an error occurs.

    @note      For "historical reasons", the last two index pairs are swapped
               in the parameter list.

    @warning   This function is not thread-safe as it uses a statically
               allocated cache to speed up the calculations.  For
               thread-safety, use `#coulomb_ho2d_r` instead.

 */
double coulomb_ho2d(unsigned n1, int ml1, unsigned n2, int ml2,
                    unsigned n4, int ml4, unsigned n3, int ml3);

/** Thread-safe version of `#coulomb_ho2d`.

    This function requires a thread-local cache to perform the calculations.

    @param cache
               Pointer to a cache created by `#clh2_cache_create`.
               The cache shall remain valid after the invocation.

    @param n1, n2, n3, n4, ml1, ml2, ml3, ml4
               The quantum numbers as described in the documentation of
               `#coulomb_ho2d`.

    @return    The value of the matrix element, or `NAN` if `cache` is `NULL`
               or an error occurs during the calculation.

    @warning   Invoking this function on multiple threads with a shared cache
               will result in undefined behavior.

    @see `#coulomb_ho2d`
 */
double coulomb_ho2d_r(struct clh2_cache *cache,
                      unsigned n1, int ml1, unsigned n2, int ml2,
                      unsigned n4, int ml4, unsigned n3, int ml3);

/** Creates a cache that can be used by `coulomb_ho2d_r`.

    @return    If successful, a pointer to a cache that can be used with
               `#coulomb_ho2d_r` or destroyed with `#clh2_cache_destroy`.
               On failure, `NULL` is returned.
 */
struct clh2_cache *clh2_cache_create();

/** Destroys the cache, releasing the memory used by it.

    @param cache
               Either a cache previously created by `#clh2_cache_create`, or
               `NULL`.
 */
void clh2_cache_destroy(struct clh2_cache *cache);

#ifdef __cplusplus
}
#endif
#endif
