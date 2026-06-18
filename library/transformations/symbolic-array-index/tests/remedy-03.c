#include "klee/klee.h"
#include <stdio.h>

// Cadar's transformation of Figure 1: REVERSE the manual lookup-table
// optimisation. The table existed only as a precomputation of k*k*k;
// recovering the closed form removes the array operation entirely, so
// the SMT formula stays in linear-integer-arithmetic + a single multiply.
//
// The constraint solver now sees `k*k*k > 100000` directly, instead of
// `cubes[k] > 100000 ∧ (k=0 → cubes_k = cubes_0) ∧ ... ∧ (k=999 →
// cubes_k = cubes_999) ∧ cubes_0 = 0 ∧ ... ∧ cubes_999 = 997002999`.
//
// This transformation requires *recognising* the closed form the table
// computes. It applies cleanly to mathematical functions (powers,
// trigonometry, hashes with known structure), and is the inverse of a
// common compiler/developer optimisation. When the table has no
// closed form (e.g. a parser's character-class table), use Perry et
// al.'s index-based / value-based rewrite instead — see remedy-01.c.

static inline int cube(unsigned k) { return (int)(k * k * k); }

int foo(unsigned k) {
  // Precondition: 0 < k < 1000.
  if (cube(k) > 100000 || cube(k - 1) > 100000)
    return 0;
  return 1;
}

int main(void) {
  unsigned k;
  klee_make_symbolic(&k, sizeof(k), "k");
  klee_assume(k > 0);
  klee_assume(k < 1000);
  int r = foo(k);
  printf("%d\n", r);
  return 0;
}
