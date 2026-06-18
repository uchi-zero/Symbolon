#include "klee/klee.h"
#include <stdio.h>

// Figure 4 from Cadar, FSE 2015 ("Targeted Program Transformations for
// Symbolic Execution"). A loop with a data-dependent branch inside its
// body. Each iteration's branch on `a[i] > 0` forks the symbolic state,
// so a 10-iteration loop produces 2^10 = 1024 paths even though the
// final state only depends on whether ALL elements were positive.
//
// Cadar reports: -O0 build → 1024 paths in 23s; -O2 build → 2 paths in
// 0.04s. The -O2 difference comes from LLVM lowering the branch as a
// `select` instruction rather than a `br` instruction. KLEE sees `select`
// as an SMT `ite`, which is a single symbolic expression — no fork —
// while `br` forces it to explore both successors.
//
// The remedy expresses the same intent using a C ternary so KLEE/LLVM
// emit `select` even at -O0. See tests/remedy-01.c.

int main(void) {
  int a[10];
  klee_make_symbolic(a, sizeof(a), "a");

  int count = 0;
  for (int i = 0; i < 10; i++) {
    if (a[i] > 0) {
      count++;
    }
  }

  if (count == 10) {
    printf("Success\n");
  }
  return count;
}
