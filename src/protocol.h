#ifndef G_O6CAWD6M72WXP7ZIISKNJQROCUXVZ
#define G_O6CAWD6M72WXP7ZIISKNJQROCUXVZ
#include <clh2.h>

union clh2_cell {
    struct clh2_indicesp indices;
    double value;
};

static const double clh2_magic_out = 2.5883786e-12;

static const struct clh2_indicesp clh2_magic_in =
    {83, -57, 55, 38, 26, -81, 45, 59};

#define CLH2_CHECK_MAGIC_IN(indices)                                        \
    (indices.n1 == clh2_magic_in.n1 && indices.ml1 == clh2_magic_in.ml1 &&  \
     indices.n2 == clh2_magic_in.n2 && indices.ml2 == clh2_magic_in.ml2 &&  \
     indices.n3 == clh2_magic_in.n3 && indices.ml3 == clh2_magic_in.ml3 &&  \
     indices.n4 == clh2_magic_in.n4 && indices.ml4 == clh2_magic_in.ml4)

#ifdef __cplusplus
extern "C" {
#endif

void clh2_main_init(const char **prog, int *argc, char ***argv);

void clh2_open_request(union clh2_cell **data, size_t *count,
                       const char *prog, const char *path);

void clh2_close_request(union clh2_cell *data, size_t count);

#ifdef __cplusplus
}
#endif
#endif
