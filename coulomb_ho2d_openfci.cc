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
#include <algorithm>
#include <new>
#include <quantumdot/QdotInteraction.hpp>
#include "coulomb_ho2d.h"
#include "coulomb_ho2d_utils.inl"

extern "C" {

/* calculate shell index */
static int shell_index(unsigned n, int ml) {
    return 2 * int(n) + abs(ml);
}

/* must have standard layout to ensure C compatibility */
struct clh2_ctx {

    /* sentinel value */
    static const int initial_shell_index_cap = -1;

    /* internal state used by OpenFCI */
    quantumdot::QdotInteraction q;

    /* maximum shell index allowed by `q` (must stay in sync) */
    int shell_index_cap;

    clh2_ctx() : shell_index_cap(initial_shell_index_cap) {}

    /* can throw exceptions */
    void setup(int shell_index_max) {
        if (shell_index_cap >= shell_index_max)
            return;
        if (shell_index_cap == initial_shell_index_cap) {
            /* must begin at a positive integer */
            shell_index_cap = 2;
        }
        do {
            shell_index_cap *= 2;
        } while (shell_index_cap < shell_index_max);
        /* increase the shell index; note that we can't actually reuse the
           internal state due to some weird bug in OpenFCI, so we have to
           construct a new one */
        q = quantumdot::QdotInteraction();
        q.setR(shell_index_cap);
        q.buildInteractionComBlocks();
    }

    /* must never throw exceptions */
    double element(const struct clh2_indices *ix) {
        const int N1 = shell_index(ix->n1, ix->ml1);
        const int N2 = shell_index(ix->n2, ix->ml2);
        const int N3 = shell_index(ix->n3, ix->ml3);
        const int N4 = shell_index(ix->n4, ix->ml4);
        const int N_max = std::max(std::max(N1, N2), std::max(N3, N4));
        try {
            setup(N_max);
            /* a sign correction is needed when using `q.singleElement` (it is
               not needed for `q.singleElementAnalytic` however) */
            const int sign = (ix->n1 + ix->n2 + ix->n3 + ix->n4) % 2 ? -1 : 1;
            return sign * q.singleElement(N1, ix->ml1, N2, ix->ml2,
                                          N3, ix->ml3, N4, ix->ml4);
        } catch (const std::exception& e) {
            warn("exception: %s\n", e.what());
        } catch (...) {}
        /* no idea if the internal state of `q` would remain valid after an
           error, so let's just start over; let's hope the assignment operator
           doesn't throw */
        q = quantumdot::QdotInteraction();
        shell_index_cap = initial_shell_index_cap;
        return NAN;
    }

};

/* constructor */
clh2_ctx *clh2_ctx_create(void) {
    try {
        /* don't use `nothrow` here since it doesn't guard against exceptions
           thrown by the constructor */
        return new clh2_ctx();
    } catch (...) {
        warn("can't create context\n");
        return NULL;
    }
}

/* destructor */
void clh2_ctx_destroy(clh2_ctx *ctx) {
    delete ctx;
}

/* calculate matrix element */
double clh2_element(clh2_ctx *ctx, const struct clh2_indices *ix) {
    return ctx->element(ix);
}

}
