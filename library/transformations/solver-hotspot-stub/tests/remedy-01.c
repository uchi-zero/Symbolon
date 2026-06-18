// remedy-01.c — solver-hotspot-stub remedy for obstacle-01.c.
//
// The cleanup function's body is replaced with `return 0;` under
// `#ifdef KLEE_BITCODE`. KLEE-built bitcode sees a no-op cleanup; the
// instrumented coverage build (compiled without `-DKLEE_BITCODE`) sees
// the original cleanup so its observable native behavior is preserved.
//
// Two-build guarantee:
//   - bitcode build (-DKLEE_BITCODE):  cleanup_hotspot is a no-op
//   - cov build (no KLEE_BITCODE):     cleanup_hotspot runs normally
// On ktest replay, the cov binary executes the concrete `n` value KLEE
// chose with the original cleanup, so the cleanup function's
// instrumentation IS exercised — just from one concrete input per
// ktest, not from a symbolic search-tree explosion.
//
// Build + measure:
//   clang -emit-llvm -c -g -O0 -Xclang -disable-O0-optnone \
//         -DKLEE_BITCODE remedy-01.c -o remedy-01.bc
//   klee --max-time=10s --output-dir=out-remedy remedy-01.bc
#include "klee/klee.h"
#include <stdio.h>

#define POOL_SZ 8

static unsigned int cleanup_hotspot(unsigned int n, const unsigned int *pool) {
#ifdef KLEE_BITCODE
  // Stub: cleanup is irrelevant to source-line coverage. Replacing with
  // a no-op saves the symbolic-bound loop expansion per state. Coverage
  // measurement on the cov binary (no `-DKLEE_BITCODE`) still runs the
  // real cleanup with each ktest's concrete n.
  (void)n;
  (void)pool;
  return 0;
#else
  unsigned int acc = 0;
  for (unsigned int i = 0; i < n; i++) {
    acc ^= pool[i];
  }
  return acc;
#endif
}

int main(void) {
  unsigned int pool[POOL_SZ];
  klee_make_symbolic(pool, sizeof(pool), "pool");

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

  unsigned int n;
  klee_make_symbolic(&n, sizeof(n), "n");
  if (n > POOL_SZ)
    return 0;
  (void)cleanup_hotspot(n, pool);
  return 0;
}
