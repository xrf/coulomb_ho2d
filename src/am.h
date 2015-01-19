#ifndef G_DPMPPZBSRKP7WYWCFVOCVIQJLYDMY
#define G_DPMPPZBSRKP7WYWCFVOCVIQJLYDMY
#ifdef __cplusplus
#include <stdexcept>
extern "C" {
#endif

/** A context structure used for matrix element calculations.

    The structure can be created with `#clh2_ctx_create`.  Once created, it
    must be later destroyed with `#clh2_ctx_destroy`.

*/
typedef struct clh2_ctx clh2_ctx;

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

    /** The angular momentum projection of the 1st particle. */
    int ml1;

    /** The principal quantum number of the 2nd particle. */
    unsigned n2;

    /** The angular momentum projection of the 2nd particle. */
    int ml2;

    /** The principal quantum number of the 3rd particle. */
    unsigned n3;

    /** The angular momentum projection of the 3rd particle. */
    int ml3;

    /** The principal quantum number of the 4th particle. */
    unsigned n4;

    /** The angular momentum projection of the 4th particle. */
    int ml4;

};

/** Creates a context.

    @return
    If successful, a pointer to a valid context.  On failure, `NULL` is
    returned.

*/
clh2_ctx *clh2_ctx_create(void);

/** Destroys the context, releasing the memory used by it.

    @param[in] ctx
    Either a pointer to valid context or `NULL`.

*/
void clh2_ctx_destroy(clh2_ctx *ctx);

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
double clh2_element(clh2_ctx *ctx, const struct clh2_indices *ix);

#ifdef __cplusplus
}
#endif
#endif
