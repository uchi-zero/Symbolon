// obstacle-01.c — symbolic-bound cleanup loop dominating solver time.
//
// Mirrors libtiff's TIFFHashSetClearInternal pattern: a function called
// after the program's "real" logic that walks a structure built from
// symbolic data, issuing one solver query per iteration. Its return
// value is discarded; the function exists only for resource cleanup,
// not coverage. But every surviving state pays its full cost, so on a
// program with K paths and N cleanup iterations the engine wastes
// K×N solver queries that contribute nothing to source-line coverage.
//
// Run with `klee --max-time=10s` and compare `total queries`,
// `completed paths`, `generated tests` against remedy-01.c.
//
// Build:
//   clang -emit-llvm -c -g -O0 -Xclang -disable-O0-optnone \
//         obstacle-01.c -o obstacle-01.bc
//   klee --max-time=10s --output-dir=out-obstacle obstacle-01.bc
#include "klee/klee.h"
#include <stdio.h>

#define POOL_SZ 8

// Cleanup hotspot: walks `n` symbolic entries, accumulating an XOR sum
// the caller discards. The loop's bound is symbolic so KLEE has to
// consider all iteration counts; the body's load is from a symbolic
// pool, so each step further compounds the path constraint. The result
// is discarded — this function is structurally not on the coverage
// path of interest, but it dominates the engine's budget.
static unsigned int cleanup_hotspot(unsigned int n, const unsigned int *pool) {
  unsigned int acc = 0;
  for (unsigned int i = 0; i < n; i++) {
    acc ^= pool[i];
  }
  return acc;
}

int main(void) {
  unsigned int pool[POOL_SZ];
  klee_make_symbolic(pool, sizeof(pool), "pool");

  // The "real" coverage-relevant logic — five distinguishable branches
  // the test is supposed to exercise.
  int input;
  klee_make_symbolic(&input, sizeof(input), "input");
  if (input == 0)
    printf("zero\n");
  else if (input == 1)
    printf("one\n");
  else if (input == 2)
    printf("two\n");
  else if (input == 3)
    printf("three\n");
  else
    printf("other\n");

  // Cleanup runs at the end of every surviving path. Its bound is a
  // symbolic count between 0 and POOL_SZ. Every state pays the
  // symbolic-loop cost. The acc result is discarded.
  unsigned int n;
  klee_make_symbolic(&n, sizeof(n), "n");
  if (n > POOL_SZ)
    return 0;
  (void)cleanup_hotspot(n, pool);
  return 0;
}
