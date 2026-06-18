#include "klee/klee.h"
#include <stdio.h>

// Induction variable substitution: the inverse of strength reduction.
// The accumulator k = k + c is replaced by `y[i] = c * i`. Each query is
// now about the single symbolic expression `c * i` — multiplication of a
// symbolic value by a *concrete* loop index, which is linear arithmetic
// and trivial for the solver. No accumulated expression tree.
//
// Cadar's reported 545x speedup is at the source-level. With modern KLEE
// + STP, the gap is smaller but the direction is preserved: see the
// numbers in the README at the end of this directory.

int main(void) {
  int c;
  klee_make_symbolic(&c, sizeof(c), "c");
  int y[500];
  int even = 0, odd = 0;
  for (int i = 0; i < 500; i++) {
    y[i] = c * i;
    if (y[i] % 2 == 0) {
      even++;
    } else {
      odd++;
    }
  }
  printf("even=%d odd=%d\n", even, odd);
  return 0;
}
