#ifndef G_TTYJPLI3QWFPLCWM7JZ3EMCZJODXB
#define G_TTYJPLI3QWFPLCWM7JZ3EMCZJODXB
#include <limits.h>
#include <stddef.h>

#if __STDC_VERSION__ >= 199901L
# define RESTRICT restrict
#else
# define RESTRICT
#endif

/* POSIX-specific */
#include <signal.h>
#include <sys/types.h>
typedef off_t rf_off;
typedef int rf_fd;
typedef pid_t rf_pid;
struct rf_sigset { sigset_t value; };

#ifndef RF_OFF_MAX
# ifndef OFF_MAX
/** Maximum value of the signed integer type `rf_off`. */
#  define RF_OFF_MAX ((off_t) 1 << (CHAR_BIT * sizeof(rf_off) - 1))
/* WARNING: the above expression gives only a guess */
# else
#  define RF_OFF_MAX OFF_MAX
# endif
#endif

#ifndef RF_OFF_MIN
# ifndef OFF_MIN
/** Minimum value of the signed integer type `rf_off`. */
#  define RF_OFF_MIN (-RF_OFF_MAX - ((off_t) 1 & (off_t) -1 && \
                                     ((off_t) 3 & (off_t) -1) != (off_t) 1))
# else
#  define RF_OFF_MIN OFF_MIN
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** Convert from `rf_off` to `size_t`. */
int rf_off_to_size(size_t *z, rf_off x);

/** Convert from `size_t` to `rf_off`. */
int rf_size_to_off(rf_off *z, size_t x);

/** Ensure the file is closed.

    @param[in] fd             File descriptor.
    @return                   Either `0`, `EBADF`, `EINTR`, or `EIO`.

*/
int rf_close(int fd);

/** Ensure the file is closed.

    @param[in] fd             File descriptor.
    @return                   Either `EINTR` or `0`.

    Note: `EBADF` and `EIO` are silently ignored.  Use `fsync` beforehand if
          you care about the integrity of the data.

*/
int rf_sclose(int fd);

/** Create a temporary file in a system-dependent temporary directory.

    @param[out] path
    Path to the file, which must be later freed.  If `NULL`, the file is
    unlinked from the file system (or deleted as soon as the file descriptor is
    closed).

    @param[out] fd
    A file descriptor.  If `NULL`, the file is immediately closed.

    @param[in]  name
    Prefix of the basename.  Can be `NULL`.

    @return
    Zero on success; a valid `errno` on failure.

    Note that if both `path` and `fd` are `NULL`, no file is created.

*/
int rf_tmpfile(char **path, int *fd, const char *name);

/** Truncate a file to a given size.

    @param[in]  fd            File descriptor.
    @param[in]  size          The size to which to truncate.
    @return                   `0` on success; `errno` on failure.

*/
int rf_ftruncate(rf_fd fd, rf_off size);

/** Map a file into memory.

    @param[out] addr          Address to the beginning of the file.
    @param[in]  fd            File descriptor.
    @param[in]  offset        Where the mapping begins.
    @param[in]  size          The size of the mapping in bytes.
    @param[in]  prot          Memory protection (octal).
    @param[in]  shared        Whether changes are shared with the file (bool).
    @return                   `0` on success; `errno` on failure.

*/
int rf_mmap(void **addr, rf_fd fd, rf_off offset,
            size_t size, int prot, int shared);

/** Map a file into memory.

    @param[out] addr          Address to the beginning of the file.
    @param[out] size          The size of the file in bytes.
    @param[in]  fd            File descriptor.
    @param[in]  prot          Memory protection (octal).
    @param[in]  shared        Whether changes are shared with the file (bool).
    @return                   `0` on success; `errno` on failure.

*/
int rf_mmapf(void **addr, size_t *size, rf_fd fd, int prot, int shared);

/** Map a file into memory.

    @param[out] addr          Address to the beginning of the file.
    @param[out] size          The size of the file in bytes.
    @param[in]  filename      Filename.
    @param[in]  prot          Memory protection (octal).
    @param[in]  shared        Whether changes are shared with the file (bool).
    @return                   `0` on success; `errno` on failure.

*/
int rf_mmapl(void **addr, size_t *size, const char *filename,
             int prot, int shared);

/** Resize a file and maps it into memory.

    @param[out] addr          Address to the beginning of the file.
    @param[in]  fd            File descriptor.
    @param[in]  size          The size of the file in bytes.
    @param[in]  prot          Memory protection (octal).
    @return                   `0` on success; `errno` on failure.

    If the file is shorter than the required size, the file is extended with
    null bytes.  If the file is longer than the required size, the file is
    truncated.  Changes are always shared with the file.

*/
int rf_mmapt(void **addr, rf_fd fd, size_t size, int prot);

/** Resize a file and map it into memory.

    @param[out] addr          Address to the beginning of the file.
    @param[in]  filename      Filename.
    @param[in]  size          The size of the file in bytes.
    @param[in]  prot          Memory protection (octal).
    @return                   `0` on success; `errno` on failure.

    If the file is shorter than the required size, the file is extended with
    null bytes.  If the file is longer than the required size, the file is
    truncated.  Changes are always shared with the file.

*/
int rf_mmapc(void **addr, const char *filename, size_t size, int prot);

/** Unloads the file from memory.

    @param[in] addr           Address to the memory-mapped file, which can be
                              `NULL` (no-op).
    @param[in] size           The size of the memory-mapped file in bytes.
    @return                   `0` on success; `errno` on failure.

    This function fails only if the arguments are invalid.

*/
int rf_munmap(void *addr, size_t size);

/** Create a copy of an argument vector.

    @param[out] argv          The copied argument vector.
    @param[in] cargv          The original argument vector.
    @return                   `0` on success; `errno` on failure.

    The copied argument vectors must be later freed using:

        free(argv[0]);
        free(argv);

*/
int rf_copy_argv(char ***argv, const char *const *cargv);

/** Spawn a process and wait until completes.

    @param[out] status        Exit status, which is negative if the process
                              was killed by a signal.  Can be `NULL`.
    @param[in] cargv          An argument vector.
    @return                   `0` on success; `errno` on failure.

*/
int rf_spawn_wait(int *status, const char *const *argv);

/** Initialize an empty signal set.

    @param[out] set           A possibly uninitialized signal set.
    @return                   `0` on success; `errno` on failure.

*/
int rf_sigemptyset(struct rf_sigset *set);

/** Initialize a filled signal set.

    @param[out] set           A possibly uninitialized signal set.
    @return                   `0` on success; `errno` on failure.

*/
int rf_sigfillset(struct rf_sigset *set);

/** Add a signal to a signal set.

    @param[in,out] set        A signal set.
    @param[in] signo          A signal to be added.
    @return                   `0` on success; `errno` on failure.

*/
int rf_sigaddset(struct rf_sigset *set, int signo);

/** Remove a signal from a signal set.

    @param[in,out] set        A signal set.
    @param[in] signo          A signal to be removed.
    @return                   `0` on success; `errno` on failure.

*/
int rf_sigdelset(struct rf_sigset *set, int signo);

/** Check whether a signal is a member of a signal set.

    @param[out] result        `1` if it is a member; `0` otherwise.
    @param[in] set            A signal set.
    @param[in] signo          A signal to check.
    @return                   `0` on success; `errno` on failure.

*/
int rf_sigismember(int *result, const struct rf_sigset *set, int signo);

/** Get the current signal mask.

    @param[out] set           The current signal mask.
    @return                   `0` on success; `errno` on failure.

*/
int rf_getsigmask(struct rf_sigset *set);

/** Modify the current signal mask.

    @param[out] oldset        The previous signal mask.
    @param[in] how            How to combine the given signal set with the
                              existing signal mask: `1` to block all given
                              signals, `-1` to unblock all given signals,
                              and `0` to set the signal mask directly.
    @param[in] set            A signal set.
    @return                   `0` on success; `errno` on failure.

*/
int rf_sigmask(struct rf_sigset *oldset, int how, struct rf_sigset set);

#ifdef __cplusplus
}
#endif
#endif
