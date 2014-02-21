Notes
=====

Here are some notes regarding the optimizations in this code.

Foreword
--------

All mentions of "speed" here refer to the inverse time taken, so a 33%
improvement implies that the time is reduced by 25%.

There are two kinds of test cases:

- `(kmax, kfmi, omega)`: This is done in the Hartree-Fock program as part of
  its matrix loading procedure.  Not as clean of an environment to test in,
  but it is more realistic.  `kmax` is the number of shells in the basis,
  `kfmi` is the number of occupied shells (shouldn't have any significant
  effect, although it may change the order in which matrix elements are
  computed), and `omega` is the frequency (minor effect, as it simply
  multiplies the matrix by a scalar).

- `(n_max, ml_max)`: This is done by the `profile(n_max, ml_max)` procedure in
  `test/coulomb_ho2d.cpp`.  This calculates matrix elements for all `n <
  n_max` and `|ml| < 5`.  When there is a `xN` present, it means the test was
  repeated `N` times.  Note that tests are often done consecutively in one
  process, so the cache is never cleared in between the adjacent runs.

All test cases are done with `-O3 -ffast-math`, using either Clang or GCC
(where such records exist).

Some statistics
---------------

Original method:

    (2, 3):    36.5 us / element
    (3, 3):   344.2 us / element
    (3, 6): 1.840   ms / element
    (4, 2): 1.101   ms / element

New method:

    (2, 3):    0.71 us / element
    (3, 3):    3.2  us / element
    (3, 6):    6.10 us / element
    (4, 2):    8.20 us / element

Here are some older benchmarks.  Original method:

    (4, 4): 354     s (for all  87808 elements)

"Semi-new" method (one of the previous versions):

    (4, 4):  45     s (for all  87808 elements)
    (5, 5): 975     s (for all 455625 elements)

New method:

    (4, 4):   1.159 s (for all  87808 elements)
    (5, 5):  20.44  s (for all 455625 elements)

The `(4, 4)` matrix is about 60-70% dense (i.e. nonzero elements) after
excluding the elements that don't conserve `ml`.  In general, the density
decreases as one increases the size of the basis.  (Note: this is measured
using an unphysical basis, since normally one does not truncate using `n` and
`|ml|` independently, but using `E = 2 * n + |ml| + 1` instead).

Optimization log
----------------

Note that this is not a comprehensive or completely accurate account of the
optimizations in this code.  The information here is roughly chronological.

### Copying matrix elements using hermitivity

Not really an optimization of the matrix element calculation, but more
relevant for the user.  Minimizing the number of calls to `coulomb_ho2d` is a
good thing: using hermitivity one can decrease that by about 50%.

#### Test case: `(kmax = 8, kfmi = 7, omega = 1)`

Indeed, this provided a 100% speed increase.

### Using the builtin `lgamma` for `LogFac`

Changed algorithm from logarithmic summation:

    double temp = 0;
    for (int i = 2; i < n + 1; i++)
        temp += log((double) i);
    return temp;

to the builtin `lgamma` function (requires C99 / C++11).

#### Test case: `(kmax = 6, kfmi = 5, omega = 1)`

This provides a moderate (25%) increase in speed.  Theoretically, it is
expected to provide higher gains in larger bases but no tests have been done
to verify this.

### Memoization of `LogFac`

`LogFac` is a major bottleneck in the calculation and there optimization of
this function is quite critical for fast calculation of matrix elements.
`LogFac` is called on predominantly a very small set of natural numbers, so it
helps to cache the results in an array instead of computing them every time.

One could also do this with a `switch` statement, but this suffers from
scalability issues:

- It's not clear how to extend this to larger bases since the number of
  branches will have to increase but `switch` statements are essentially
  static (implementing this dynamically is difficult and non-portable).

- The performance benefit likely diminishes as the number of branches grows,
  and eventually may become a pessimization due to the size of the code.
  `LogFac` is expected to be inlined at various places, so the growth in the
  size of the code can impact on the instruction caching.

- Does not mix with the dynamic

Therefore, for simplicity, the memoization is implemented using a simple array
where the results are cached.  It is expected that the array should be small
and used frequently enough that it can fit within a very fast cache.

This provides a significant gain (100% to 200%) in speed.

### Handling cache misses

Note: this is referring to misses in the memoization array, not the CPU's.

If the result hasn't been cached yet, it needs to calculate the results.  This
part can be rather complicated as it involves memory allocation (it doesn't
know how large the cache would be ahead of time).  Fortunately this is a very
rare event, and should not significantly impact branch prediction.

Most of the time, the CPU should expect to read the value directly from the
memoization array.  This is a relatively simply process and should be inlined.

In contrast, the process of growing the cache is generally slow and
complicated, and it is a pessimization to inline this part of the code.
Therefore a `noinline` directive is used to prevent this from occurring.

In theory, a branch prediction hint may help (using `__builtin_expect`) but
this does not seem to be the case in practice.

#### Test case: `(n_max = 3, ml_max = 6)`

The use of `noinline` had little impact on GCC (probably because GCC decided
to not inline it anyway), but for clang using `noinline` provided a 20%
increase in speed (in the test case, at least).

- In Clang:
  - With `noinline` turned on, the time was 25.1 sec.
  - With `noinline` turned off, the time was 30.2 sec.
    - A correct branch hint increased time by 0.4 sec.
    - An incorrect branch hint increased time by 0.9 sec.
- In GCC:
  - With `noinline` either on or off, the time was 26.9 sec.  (Slightly slower
    than Clang in the best-case scenario.)
    - A correct branch hint made no difference here (probably deduced by GCC).
    - An incorrect branch hint increased time by 0.9 sec.

### Memoization of `lgamma2`

Turns out that although there are two `lgamma` calls, the input is always an
integer or half-integer, so the results can also be memoized.  This is done
using the same trick as before, but the key question is: can these two caches
be combined?  (Since `lgamma` and `LogFac2` are related, this means a cache
for `lgamma` for all half-integers is sufficient for `LogFac`.)

However, the downside is: forcing `LogFac` (note: this is now called `lfac` as
of writing) to use `lgamma`'s cache involves performing a small operation, so
this adds a tiny amount of overhead.  Furthermore, `LogFac` is used more
frequently than `lgamma`.

#### Test cases: (2, 3) x40, (3, 3) x10, (3, 4) x2

Turns out that there is a small difference of about 2% (after several
measurements).  That is, using two caches provides a very small speed
increase, at the expensive of managing another cache (and some redundant
data).

### Precomputing `lgamma2` & `pow2`

It turns out that for a given set of quantum numbers, `lgamma2` and `pow2`
have a finite domain that can be calculated ahead of time, so it is possible
to simply precompute all the necessary results of these two functions ahead of
time.  This allows the evaluation of these function to be much simpler: the
size comparison plus potential branching can be entirely eliminated.

This involved a fairly major rewrite however.  In part, this was done to
address the concerns about thread-safety: these functions are not safe when
called from different threads due to the use of global data.

#### Test cases: (2, 3) x40, (3, 3) x10, (3, 6) x2, (4, 2) x2

The benefit varies (was lowest for `(3, 6)` and highest for `(2, 3)`), but it
was significant nonetheless: about 25% to 60% increase in speed.

### Avoiding divisions

Division can be slower than multiplication, so as many divisions were
converted into multiplications as possible.  Furthermore, the inner loop
(after factoring out some terms) consists of solely division, so the `fac` and
`tgamma2` functions were converted into their reciprocals instead.

#### Test cases: (2, 3) x40, (3, 3) x10, (3, 6) x2, (4, 2) x2

A fair amount of variation in improvement: about 20% to 50% increase in speed.
The benefit was least for `(2, 3)` and highest for `(3, 6)`.

### Converting factorial ratios into a running binomial coefficient

There is a very simple way of calculating the binomial coefficient as a
running sum:

    n / 1 * (n - 1) / 2 * (n - 2) / 3 * (n - 3) / 4 * ... * (n - k + 1) / k

This means it's possible to avoid the factorial ratios by computing this
cumulative product.  However, after further testing this turned out to be more
of a pessimization.  Not unexpected, since the factorials were precomputed.
