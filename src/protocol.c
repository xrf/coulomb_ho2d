#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "protocol.h"
#ifdef __cplusplus
extern "C" {
#endif

void clh2_main_init(const char **prog, int *argc, char ***argv) {

    /* get program name */
    *prog = **argv;
    if (!*prog) {
        (void) fprintf(stderr, "no input files\n");
        exit(EXIT_FAILURE);
    }

    /* make sure there is at least one input file */
    if (!argv[1]) {
        (void) fprintf(stderr, "%s: no input files\n", *prog);
        exit(EXIT_FAILURE);
    }

    /* shift the arguments */
    --*argc;
    ++*argv;
}

void clh2_open_request(union clh2_cell **data, size_t *count,
                       const char *prog, const char *path) {
    static const size_t cell_size = sizeof(union clh2_cell);
    union clh2_cell *p;
    size_t size;
    void *ptr;

    int e = rf_mmapl(&ptr, &size, path, 06, 1);
    if (e) {
        (void) fprintf(stderr, "%s: %s: %s\n", prog, strerror(e), path);
        exit(EXIT_FAILURE);
    }

    if (!size) {
        (void) rf_munmap(ptr, size);
        (void) fprintf(stderr, "%s: empty input file: %s\n", prog, path);
        exit(EXIT_FAILURE);
    }

    if (size % cell_size) {
        (void) rf_munmap(ptr, size);
        (void) fprintf(stderr, "%s: size must be a multiple of %lu: %s\n",
                       prog, (unsigned long) cell_size, path);
        exit(EXIT_FAILURE);
    }

    p = (union clh2_cell *) ptr;
    if (!CLH2_CHECK_MAGIC_IN(p->indices)) {
        (void) rf_munmap(ptr, size);
        (void) fprintf(stderr, "%s: bad magic number in %s\n",
                       prog, path);
        exit(EXIT_FAILURE);
    }

    /* set the output parameters */
    *data  = p + 1;
    *count = size / cell_size - 1;
}

void clh2_close_request(union clh2_cell *data, size_t count) {
    /* recover the original pointer and size */
    union clh2_cell *const p = data - 1;
    const size_t size = (count + 1) * sizeof(*p);

    /* set the magic number */
    p->value = clh2_magic_out;

    /* unmap the memory */
    (void) rf_munmap(p, size);
}

#ifdef __cplusplus
}
#endif
