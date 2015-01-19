#ifndef G_N7J6X5ANB5BXFOPRYI4ZUJBJOHRRC
#define G_N7J6X5ANB5BXFOPRYI4ZUJBJOHRRC
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
/** @defgroup main  Main interface

    Calculation of the Coulomb repulsion matrix elements in the
    two-dimensional harmonic oscillator basis.

    Header file: `<clh2.h>`

    @{

*/
#include <stddef.h>
#ifndef CLH2_EXTERN
# if defined _WIN32 || defined __CYGWIN__
#  ifdef CLH2_BUILD
#   define CLH2_EXTERN __declspec(dllexport)
#  else
#   define CLH2_EXTERN __declspec(dllimport)
#  endif
# elif __GNUC__ >= 4
#  define CLH2_EXTERN __attribute__ ((visibility ("default")))
# else
#  define CLH2_EXTERN
# endif
#endif
#ifdef __cplusplus
extern "C" {
#endif

/** Indices of a matrix element.

    Here, `n` is the principal quantum number and `m` is to the angular
    momentum projection.  In the form of a matrix element, the indices appear
    in this order:

        <n1, ml1; n2, ml2 | V | n3, ml3; n4, ml4>

    where `V` is a two-particle operator.

*/
struct clh2_indicesp {

    /** The principal quantum number of the 1st particle. */
    unsigned char n1;

    /** The angular momentum projection of the 1st particle. */
    signed char ml1;

    /** The principal quantum number of the 2nd particle. */
    unsigned char n2;

    /** The angular momentum projection of the 2nd particle. */
    signed char ml2;

    /** The principal quantum number of the 3rd particle. */
    unsigned char n3;

    /** The angular momentum projection of the 3rd particle. */
    signed char ml3;

    /** The principal quantum number of the 4th particle. */
    unsigned char n4;

    /** The angular momentum projection of the 4th particle. */
    signed char ml4;

};

/** Request a tabulation of matrix elements from a given provider.

    @param[out] values
    An array containing the result of the tabulation in the same order as
    requested.  Must not be `NULL`.  The array must later be freed using
    `#clh2_free`.

    @param[in] provider
    A string that designates the tabulation provider (an executable).  This is
    a filename to an executable, which can be either be a name or a relative
    or absolute path.  If `NULL`, defaults to `"clh2-am"`.

    @param[in] count
    Number of matrix elements to tabulate.

    @param[in] args
    An array containing the indices for which matrix elements are to be
    tabulated.  Must not be `NULL` unless `count` is zero.

    @return
    `0` on success, or `errno` on failure.  The argument `values` is not
    modified unless the function succeeds.

    Some of the typical errors include:

      - `ENOPROTOOPT`: could not spawn provider process (for example, provider
        executable does not exist or does not have the necessary permissions).

      - `EPROTO`: provider process failed to execute properly or did not
        return the results in the correct format.

      - `ENOLINK`: provider process was terminated prematurely.

 */
CLH2_EXTERN int clh2_request(const double **values, const char *provider,
                             size_t count, const struct clh2_indicesp *args);

/** Request a tabulation of matrix elements from a given provider.

    @param[in] count
    Number of matrix elements that was tabulated.

    @param[in] values
    The array of matrix elements originally returned from `#clh2_request`.
    This can also be `NULL`, in which case the function is a no-op and `count`
    is ignored.

 */
CLH2_EXTERN void clh2_free(size_t count, const double *values);

#ifdef __cplusplus
}
#endif
/** @} */
#endif
