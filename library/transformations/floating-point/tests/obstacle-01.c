#include "klee/klee.h"
#include <stdio.h>

// Pattern 5 from Cadar, FSE 2015 ("Targeted Program Transformations for
// Symbolic Execution"). KLEE does not have a floating-point solver by
// default, and even FP-capable solvers struggle: bit-blasted reasoning
// over IEEE-754 is exponential in operand width, and FP path constraints
// frequently exceed solver budgets.
//
// In practice, KLEE either rejects symbolic FP operations outright or
// concretises them (turning a symbolic input into a single concrete
// value, losing alternative paths). Either way the obstacle is the
// same: code that uses symbolic doubles cannot be effectively explored.
//
// Cadar's transformation is semantics-altering: replace floats with
// integers (or fixed-point rationals), accepting that some FP-specific
// behaviours (overflow, NaN propagation, rounding edge cases) are no
// longer testable. In exchange, the analyser can make progress.

int main(void) {
  double x;
  klee_make_symbolic(&x, sizeof(x), "x");

  // A simple threshold predicate. KLEE without an FP solver will refuse
  // to fork on this branch.
  if (x * x > 100.0) {
    printf("large\n");
  } else {
    printf("small\n");
  }
  return 0;
}
