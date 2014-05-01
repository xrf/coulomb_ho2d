#ifndef CLH2_EXTERN
# error "Do not include this header directly.  Use <coulomb_ho2d.h> instead."
#else

/** @defgroup compat  Legacy interface

    For backward compatibility only.

    Header file: `<coulomb_ho2d.h>`

    @{ */

struct clh2_cache;

/** Calculates the Coulomb matrix element (not thread-safe).

    @deprecated Use `#clh2_element` instead, which is thread-safe.
 */
CLH2_EXTERN double coulomb_ho2d(unsigned n1, int ml1, unsigned n2, int ml2,
                                unsigned n4, int ml4, unsigned n3, int ml3);

/** Calculates the Coulomb matrix element (thread-safe).

    @deprecated Use `#clh2_element` instead.
 */
CLH2_EXTERN double coulomb_ho2d_r(struct clh2_cache *,
                                  unsigned n1, int ml1, unsigned n2, int ml2,
                                  unsigned n4, int ml4, unsigned n3, int ml3);

/** Creates a cache object.

    @deprecated Use `#clh2_ctx_create` in conjunction with `#clh2_element`
    instead.
 */
CLH2_EXTERN struct clh2_cache *clh2_cache_create(void);

/** Destroys the cache object.

    @deprecated Use `#clh2_ctx_destroy` in conjunction with `#clh2_element`
    instead.
 */
CLH2_EXTERN void clh2_cache_destroy(struct clh2_cache *);

/** @} */
#endif
