#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <clh2.h>
#include "am.h"
#include "protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

static const char *prog;

int main(int argc, char **argv) {
    clh2_ctx *ctx = clh2_ctx_create();
    clh2_main_init(&prog, &argc, &argv);

    if (!ctx) {
        fprintf(stderr, "%s: can't create context\n", prog);
        return EXIT_FAILURE;
    }

    for (; *argv; ++argv) {
        union clh2_cell *data, *p;
        size_t count;

        clh2_open_request(&data, &count, prog, *argv);
        for (p = data; p != data + count; ++p) {
            struct clh2_indices ix;
            ix.n1  = p->indices.n1;
            ix.ml1 = p->indices.ml1;
            ix.n2  = p->indices.n2;
            ix.ml2 = p->indices.ml2;
            ix.n3  = p->indices.n3;
            ix.ml3 = p->indices.ml3;
            ix.n4  = p->indices.n4;
            ix.ml4 = p->indices.ml4;
            p->value = clh2_element(ctx, &ix);
        }
        clh2_close_request(data, count);
    }

    clh2_ctx_destroy(ctx);
    return EXIT_SUCCESS;
}

#ifdef __cplusplus
}
#endif
