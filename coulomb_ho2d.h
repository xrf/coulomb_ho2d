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
/** @mainpage Coulomb HO2D

    This code calculates the matrix elements of the Coulomb repulsion operator
    in the basis of a two-dimensional harmonic oscillator (HO2D). @par

    For more information, see:

      - [Main interface](group__main.html)
      - [Legacy interface](group__compat.html)

    @copyright  MIT License.

    @defgroup main  Main interface

    Calculation of the Coulomb repulsion matrix elements in the
    two-dimensional harmonic oscillator basis.

    @{

 */
#ifdef _WIN32
# ifdef CLH2_BUILD
#  define CLH2_EXTERN __declspec(dllexport)
# else
#  define CLH2_EXTERN __declspec(dllimport)
# endif
#else
# define CLH2_EXTERN
#endif
#ifdef __cplusplus
extern "C" {
#endif
#ifndef CLH2_DOC
# include "coulomb_ho2d_compat.h"
#endif

/** A context structure used for matrix element calculations.

    The structure can be created with `#clh2_ctx_create`.  Once created, it
    must be later destroyed with `#clh2_ctx_destroy`.

 */
typedef struct clh2_ctx clh2_ctx;

/** Creates a context.

    @return
    If successful, a pointer to a valid context.  On failure, `NULL` is
    returned.

 */
CLH2_EXTERN clh2_ctx *clh2_ctx_create(void);

/** Destroys the context, releasing the memory used by it.

    @param[in] ctx
    Either a pointer to valid context or `NULL`.

 */
CLH2_EXTERN void clh2_ctx_destroy(clh2_ctx *ctx);

/** Indices of a matrix element.

    Here, `n` is the principal quantum number and `m` is to the angular
    momentum projection.  In the form of a matrix element, the indices appear
    in this order:

        <n1, ml1; n2, ml2 | V | n3, ml3; n4, ml4>

    where `V` is a two-particle operator.

 */
struct clh2_indices {

    /** The principal quantum number of the 1st particle. */
    unsigned n1;

    /** The principal quantum number of the 2nd particle. */
    unsigned n2;

    /** The principal quantum number of the 3rd particle. */
    unsigned n3;

    /** The principal quantum number of the 4th particle. */
    unsigned n4;

    /** The angular momentum projection of the 1st particle. */
    int ml1;

    /** The angular momentum projection of the 2nd particle. */
    int ml2;

    /** The angular momentum projection of the 3rd particle. */
    int ml3;

    /** The angular momentum projection of the 4th particle. */
    int ml4;

};

/** Calculates the Coulomb matrix element in a 2D harmonic oscillator basis.

    Returns the matrix element of a two-particle Coulomb repulsion operator.
    The matrix element is not antisymmetrized and does not consider spin.

    @param[in] ctx
    Pointer to a valid context object.  It shall remain valid after the
    invocation of this function regardless of whether an error occurs.
    Must not be `NULL`.

    @param[in] ix
    Pointer to a structure containing indices that label the matrix element.
    Must not be `NULL`.

    @return
    The value of the matrix element, or `NAN` if an error occurs.

    @warning
    The context must not be shared between threads.

 */
CLH2_EXTERN double clh2_element(clh2_ctx *ctx, const struct clh2_indices *ix);

/** @} */
#ifdef __cplusplus
}
#endif
#endif
