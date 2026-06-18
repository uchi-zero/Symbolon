#include "klee/klee.h"
#include <stdio.h>

// Branch-to-select rewrite (Cadar, FSE 2015, Figure 4). The branchy
// `if (a[i] > 0) count++;` is replaced with a ternary that always
// assigns to `count` — either the incremented value or the unchanged
// value. LLVM lowers this to a `select` instruction, which KLEE treats
// as a single SMT `ite` expression; no fork per iteration.
//
// Result: instead of 2^10 = 1024 paths from the loop, KLEE explores
// just two — one where the count==10 condition holds, one where it
// doesn't — and finishes in tens of milliseconds.

int main(void) {
  int a[10];
  klee_make_symbolic(a, sizeof(a), "a");

  int count = 0;
  for (int i = 0; i < 10; i++) {
    count = (a[i] > 0) ? count + 1 : count;
  }

  if (count == 10) {
    printf("Success\n");
  }
  return count;
}
