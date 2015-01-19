#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <spawn.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "util.h"
#include "math.inl"
#ifdef __cplusplus
extern "C" {
#endif

extern char **environ;

int rf_close(int fd) {
    /* When `EINTR` is received, pretty much most OSes except HP-UX will still
       close the file.  If you are using HP-UX however, be sure to define
       `RETRY_CLOSE_ON_EINTR`. */
    errno = 0;
    while (close(fd) && errno == EINTR)
#ifndef RETRY_CLOSE_ON_EINTR
        break;
#endif
    return errno;
}

int rf_off_to_size(size_t *z, rf_off x) {
    if ((rf_off) (size_t) x != x)
        return ERANGE;
    *z = (size_t) x;
    return 0;
}

int rf_size_to_off(rf_off *z, size_t x) {
    if ((rf_off) (size_t) RF_OFF_MAX == RF_OFF_MAX && x > (size_t) RF_OFF_MAX)
        return ERANGE;
    *z = (rf_off) x;
    return 0;
}

int rf_sclose(int fd) {
    return rf_close(fd) == EINTR ? EINTR : 0;
}

int rf_tmpfile(char **path, int *fd, const char *name) {
    const char *tmpdir = getenv("TMPDIR");
    const char *const xs = "XXXXXX";
    size_t len_tmpdir;
    const size_t len_name = name ? strlen(name) : 0;
    const size_t len_xs = strlen(xs);
    size_t bufsz = 0;
    char *p, *buf;
    mode_t mode;
    int e, f;

    if (!path && !fd)
        return 0;

    if (!tmpdir)
        tmpdir = "/tmp";
    len_tmpdir = strlen(tmpdir);

    /* calculate size of buffer */
    if ((e = rf_adds(&bufsz, bufsz, len_tmpdir)))
        return e;
    if ((e = rf_adds(&bufsz, bufsz, 1)))
        return e;
    if ((e = rf_adds(&bufsz, bufsz, len_name)))
        return e;
    if ((e = rf_adds(&bufsz, bufsz, len_xs)))
        return e;
    if ((e = rf_adds(&bufsz, bufsz, 1)))
        return e;

    buf = (char *) malloc(bufsz);
    if (!buf)
        return ENOMEM;

    /* append strings */
    p = buf;
    (void) memcpy(p, tmpdir, len_tmpdir);
    p += len_tmpdir;
    *p = '/';
    p += 1;
    if (name) {
        (void) memcpy(p, name, len_name);
        p += len_name;
    }
    (void) memcpy(p, xs, len_xs);
    p += len_xs;
    *p = '\0';

    /* use mask to ensure 0600 even on non-compliant systems */
    mode = umask(S_IXUSR | S_IRWXG | S_IRWXO);
    f = mkstemp(buf);
    e = errno;
    (void) umask(mode);
    if (f == -1) {
        free(buf);
        return e;
    }

    /* close file if `fd` is `NULL` */
    if (!fd) {
        e = rf_close(f);
        if (e) {
            (void) unlink(buf);
            free(buf);
            return e;
        }
    }

    /* delete file if `path` is `NULL` */
    if (!path) {
        const int r = unlink(buf);
        e = errno;
        free(buf);
        if (r) {
            (void) rf_close(f);
            return e;
        }
    } else {
        *path = buf;
    }
    if (fd)
        *fd = f;
    return 0;
}

int rf_ftruncate(rf_fd fd, rf_off size) {
    return ftruncate(fd, size) && errno;
}

int rf_mmap(void **addr, rf_fd fd, rf_off offset,
            size_t size, int prot, int shared) {
    void *a = NULL;
    int p = PROT_NONE;

    if (!addr || prot >= (1 << 3) || prot < 0)
        return EINVAL;

    /* special handling since mmap doesn't work with zero sizes */
    if (!size) {
        *addr = a;
        return 0;
    }

    /* convert the protection flag */
    if (prot) {
        p = 0;
        if (prot & (1 << 0))
            p |= PROT_EXEC;
        if (prot & (1 << 1))
            p |= PROT_WRITE;
        if (prot & (1 << 2))
            p |= PROT_READ;
    }

    /* memory map */
    a = mmap(NULL, size, p, shared ? MAP_SHARED : MAP_PRIVATE, fd, offset);
    if (a == MAP_FAILED)
        return errno;

    *addr = a;
    return 0;
}

int rf_mmapf(void **addr, size_t *size, rf_fd fd, int prot, int shared) {
    struct stat st;
    size_t s;
    int e;

    if (!addr || !size)
        return EINVAL;

    /* get size of file */
    if (fstat(fd, &st))
        return errno;

    /* make sure it's not too big */
    if (rf_off_to_size(&s, st.st_size))
        return ENOMEM;

    /* map file */
    e = rf_mmap(addr, fd, 0, s, prot, shared);
    if (e)
        return e;

    *size = (size_t) st.st_size;
    return 0;
}

int rf_mmapl(void **addr, size_t *size, const char *filename,
             int prot, int shared) {
    size_t s;
    void *a;
    int e, fd;

    if (!addr || !size || !filename)
        return EINVAL;

    /* open file */
    fd = open(filename, O_RDWR);
    if (fd == -1)
        return errno;

    /* map file */
    e = rf_mmapf(&a, &s, fd, prot, shared);
    if (e) {
        (void) rf_close(fd);
        return e;
    }

    /* don't need the file descriptor anymore */
    e = rf_sclose(fd);
    if (e) {
        (void) rf_munmap(a, s);
        return e;
    }

    *addr = a;
    *size = s;
    return 0;
}

int rf_mmapt(void **addr, rf_fd fd, size_t size, int prot) {
    rf_off s;
    int e;

    if (!addr)
        return EINVAL;

    /* make sure it's not too big */
    if (rf_size_to_off(&s, size))
        return EFBIG;

    /* set the size of the file */
    e = rf_ftruncate(fd, s);
    if (e)
        return e;

    /* map file */
    return rf_mmap(addr, fd, 0, size, prot, 1);
}

int rf_mmapc(void **addr, const char *filename, size_t size, int prot) {
    rf_fd fd;
    void *a = NULL;
    int e;

    if (!addr || !filename)
        return EINVAL;

    /* open file */
    fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
        return errno;

    /* map file */
    e = rf_mmapt(&a, fd, size, prot);
    if (e) {
        (void) rf_close(fd);
        return e;
    }

    /* don't need the file descriptor anymore */
    e = rf_sclose(fd);
    if (e) {
        (void) rf_munmap(a, size);
        return e;
    }

    *addr = a;
    return 0;
}

int rf_munmap(void *data, size_t size) {
    /* special handling since mmap doesn't work with zero sizes */
    if (!data || !size)
        return 0;

    return munmap(data, size) && errno;
}

int rf_copy_argv(char ***margv, const char *const *cargv) {
    const char *const *cargvp;
    char *args = NULL, *argsp;
    char **argv, **argvp;
    size_t args_size = 0, argv_size = sizeof(*argv);

    if (!margv || !cargv)
        return EINVAL;

    /* calculate lengths */
    for (cargvp = cargv; *cargvp; ++cargvp) {
        int e = rf_adds(&argv_size, argv_size, sizeof(*argv));
        if (e)
            return e;
        e = rf_adds(&args_size, args_size, strlen(*cargvp) + 1);
        if (e)
            return e;
    }

    /* allocate */
    argv = (char **) malloc(argv_size);
    if (!argv)
        return ENOMEM;
    if (args_size) {
        args = (char *) malloc(args_size);
        if (!args) {
            free(argv);
            return ENOMEM;
        }
    }

    /* copy strings */
    argsp = args;
    argvp = argv;
    for (cargvp = cargv; *cargvp; ++cargvp) {
        const size_t size = strlen(*cargvp) + 1;
        (void) memcpy(argsp, *cargvp, size);
        *argvp = argsp;
        argsp += size;
        ++argvp;
    }
    *argvp = NULL;

    *margv = argv;
    return 0;
}

static int rf_spawnattr_default(posix_spawnattr_t *attr) {
    sigset_t set;

    if (posix_spawnattr_init(attr))
        return errno;

    /* reset the signal mask */
    if (posix_spawnattr_setflags(attr, POSIX_SPAWN_SETSIGMASK) ||
        sigemptyset(&set) ||
        posix_spawnattr_setsigmask(attr, &set)) {
        (void) posix_spawnattr_destroy(attr);
        return errno;
    }

    return 0;
}

int rf_spawn_wait(int *status, const char *const *cargv) {
    posix_spawnattr_t attr;
    rf_pid pid;
    int e, s;
    char **argv;

    if (!cargv || !cargv[0])
        return EINVAL;

    /* copy arguments because `posix_spawn` expects it to be mutable */
    e = rf_copy_argv(&argv, cargv);
    if (e)
        return e;

    /* set default spawn attributes */
    e = rf_spawnattr_default(&attr);
    if (e) {
        free(argv[0]);
        free(argv);
        return e;
    }

    /* spawn process */
    e = posix_spawnp(&pid, argv[0], NULL, &attr, argv, environ);
    (void) posix_spawnattr_destroy(&attr);
    free(argv[0]);
    free(argv);
    if (e)
        return e;

    /* wait for completion */
    do {
        const rf_pid rpid = waitpid(pid, &s, 0);
        e = errno;
        if (rpid == -1)
            break;

        /* check for termination */
        if (WIFEXITED(s)) {
            if (status)
                *status = WEXITSTATUS(s);
            return 0;
        } else if (WIFSIGNALED(s)) {
            if (status)
                *status = -WTERMSIG(s);
            return 0;
        }

        /* if child was stopped, wait again; otherwise fail */
        e = ENOTSUP;
    } while (WIFSTOPPED(s));

    /* clean up */
    (void) kill(pid, SIGHUP);
    return e;
}

int rf_sigemptyset(struct rf_sigset *set) {
    struct rf_sigset s;
    int e = sigemptyset(&s.value);
    if (e)
        return errno;
    *set = s;
    return 0;
}

int rf_sigfillset(struct rf_sigset *set) {
    struct rf_sigset s;
    int e = sigfillset(&s.value);
    if (e)
        return errno;
    *set = s;
    return 0;
}

int rf_sigaddset(struct rf_sigset *set, int signo) {
    struct rf_sigset s;
    int e = sigaddset(&s.value, signo);
    if (e)
        return errno;
    *set = s;
    return 0;
}

int rf_sigdelset(struct rf_sigset *set, int signo) {
    struct rf_sigset s;
    int e = sigdelset(&s.value, signo);
    if (e)
        return errno;
    *set = s;
    return 0;
}

int rf_sigismember(int *result, const struct rf_sigset *set, int signo) {
    int e = sigismember(&set->value, signo);
    if (e < 0)
        return errno;
    *result = e;
    return 0;
}

int rf_getsigmask(struct rf_sigset *set) {
    struct rf_sigset s;
    int e;
    if (!set)
        return EINVAL;
#ifdef RF_NOTHREAD
    e = sigprocmask(0, NULL, &s.value);
#else
    e = pthread_sigmask(0, NULL, &s.value);
#endif
    if (e)
        return errno;
    *set = s;
    return 0;
}

int rf_sigmask(struct rf_sigset *oldset, int how, struct rf_sigset set) {
    struct rf_sigset s;
    int h, e;
    switch (how) {
    case 1:
        h = SIG_BLOCK;
        break;
    case -1:
        h = SIG_UNBLOCK;
        break;
    case 0:
        h = SIG_SETMASK;
        break;
    default:
        return EINVAL;
    }
#ifdef RF_NOTHREAD
    e = sigprocmask(h, &set.value, oldset ? &s.value : NULL);
#else
    e = pthread_sigmask(h, &set.value, oldset ? &s.value : NULL);
#endif
    if (e)
        return errno;
    if (oldset)
        *oldset = s;
    return 0;
}

#ifdef __cplusplus
}
#endif
