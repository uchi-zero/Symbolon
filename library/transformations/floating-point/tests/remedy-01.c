#include "klee/klee.h"
#include <stdio.h>

// Float-to-int substitution. The symbolic input is now a 32-bit signed
// integer, and the predicate is rewritten in integer arithmetic. KLEE
// reasons about this fully — it's pure linear arithmetic that the SMT
// solver handles in microseconds.
//
// SEMANTICS-ALTERING. This is NOT a sound replacement for the original:
//   - Range is now -2^31 ≤ x < 2^31, not the full IEEE-754 double range.
//   - No subnormal / NaN / infinity behaviours are tested.
//   - Overflow semantics differ: integer multiplication wraps; FP
//     multiplication saturates to infinity.
//
// What you DO get: coverage of the algorithmic control flow that
// depends on input magnitudes. For most application-level code (game
// logic, threshold checks, geometric queries with bounded ranges),
// this captures the behaviours that matter. Bugs that require specific
// FP corner cases (e.g. NaN-propagation flaws in a financial library)
// will not be found by the substituted version — re-test those with a
// fuzzer or with an FP-capable solver if available.

int main(void) {
  int x;
  klee_make_symbolic(&x, sizeof(x), "x");
  // Limit the symbolic range so x*x doesn't overflow int (still leaves
  // billions of paths to explore inside the bound).
  klee_assume(x > -46340);
  klee_assume(x < 46340);

  if (x * x > 100) {
    printf("large\n");
  } else {
    printf("small\n");
  }
  return 0;
}
