#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <clh2.h>
#include "protocol.h"
#include "util.h"
#include "math.inl"

static const size_t cell_size = sizeof(union clh2_cell);

#ifdef __cplusplus
extern "C" {
#endif

int clh2_request(const double **values, const char *provider,
                 size_t count, const struct clh2_indicesp *args) {
    const char *argv[3] = {"clh2-am", NULL, NULL};
    union clh2_cell *data;
    struct rf_sigset set;
    char *tmpfile;
    int e, status;
    size_t size, new_size;
    rf_off fsize;
    rf_fd fd;
    void *ptr;

    if (!values || (count && !args))
        return EINVAL;

    if (!count) {
        *values = NULL;
        return 0;
    }

    if (provider)
        argv[0] = provider;

    /* calculate: size <- (count + 1) * cell_size */
    if (rf_adds(&size, count, 1))
        return ENOMEM;
    if (rf_muls(&size, size, cell_size))
        return ENOMEM;

    if (rf_size_to_off(&fsize, size))
        return EFBIG;

    /* block all signals for now; we rely on the child process to tell us when
       a signal has occurred (since it is part of the same process group, it
       receives the same signals from the terminal)

       this is needed so we get a chance clean up the temporary file after a
       signal (e.g. due to SIGINT from the user); otherwise we may end up with
       a lot of abandoned temporary files

       this isn't an ideal solution, preferably we should have "gaps" in the
       `waitpid` call so that we can also receive signals in a timely manner.
       however this would greatly complicate the implementation */
    (void) rf_sigfillset(&set);
    (void) rf_sigmask(&set, 0, set);

    /* create and open a temporary file */
    e = rf_tmpfile(&tmpfile, &fd, "clh2_req.");
    argv[1] = tmpfile;
    if (e) {
        (void) rf_sigmask(NULL, 0, set);
        return e;
    }

    /* resize file and memory map */
    e = rf_mmapt(&ptr, fd, size, 06);
    if (e) {
        (void) rf_close(fd);
        (void) unlink(tmpfile);
        free(tmpfile);
        (void) rf_sigmask(NULL, 0, set);
        return e;
    }

    /* set the magic number */
    data = (union clh2_cell *) ptr;
    data->indices = clh2_magic_in;

    /* copy inputs (choose the faster way) */
    if (cell_size == sizeof(*args)) {
        (void) memcpy(data + 1, args, size - cell_size);
    } else {
        const struct clh2_indicesp *src = args;
        union clh2_cell *dest = data + 1;
        for (; src != args + count; ++dest, ++src)
            dest->indices = *src;
    }

    /* flush data and close file */
    (void) rf_munmap(ptr, size);
    e = rf_close(fd);
    if (e) {
        (void) unlink(tmpfile);
        free(tmpfile);
        (void) rf_sigmask(NULL, 0, set);
        return e;
    }

    /* run child process */
    e = rf_spawn_wait(&status, argv);
    if (!e && status) {
        if (status == 127)              /* due to spawn failure */
            e = ENOPROTOOPT;
        else if (status > 0)            /* due to child exit status */
            e = EPROTO;
        else                            /* due to signal */
            e = ENOLINK;
    }
    if (e) {
        (void) unlink(tmpfile);
        free(tmpfile);
        (void) rf_sigmask(NULL, 0, set);
        return e;
    }

    /* memory map again (we can delete the file now) */
    e = rf_mmapl(&ptr, &new_size, tmpfile, 04, 0);
    (void) unlink(tmpfile);
    free(tmpfile);
    (void) rf_sigmask(NULL, 0, set);
    if (e)
        return e;

    /* make sure the output is of the expected size */
    if (new_size != size) {
        (void) rf_munmap(ptr, new_size);
        return EPROTO;
    }

    /* check if the representations are compatible */
    if (cell_size == sizeof(**values)) {
        const double *const data = (double *) ptr;

        /* check the magic number */
        if (*data != clh2_magic_out) {
            (void) rf_munmap(ptr, new_size);
            return EPROTO;
        }

        /* use it as is */
        *values = data + 1;

    } else {
        const union clh2_cell *const data = (union clh2_cell *) ptr;
        const union clh2_cell *src = data + 1;
        double *dest;

        /* check the magic number */
        if (data->value != clh2_magic_out) {
            (void) rf_munmap(ptr, new_size);
            return EPROTO;
        }

        /* (safe to multiply since `double` is smaller than the union) */
        dest = (double *) malloc(count * sizeof(**values));
        if (!dest) {
            (void) rf_munmap(ptr, new_size);
            return ENOMEM;
        }

        /* copy the values */
        *values = dest;
        for (; dest != *values + count; ++dest, ++src)
            *dest = src->value;
        (void) rf_munmap(ptr, new_size);
    }
    return 0;
}

void clh2_free(size_t count, const double *values) {
    /* release the memory based on how it was allocated previously */
    if (cell_size == sizeof(*values)) {
        const size_t size = (count + 1) * sizeof(*values);
        /* this handles `values == NULL` just fine */
        (void) rf_munmap((void *) values, size);
    } else {
        free((double *) values);
    }
}

#ifdef __cplusplus
}
#endif
