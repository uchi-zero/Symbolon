#include "klee/klee.h"
#include <stdio.h>

// Figure 2 from Cadar, FSE 2015 ("Targeted Program Transformations for
// Symbolic Execution"). Strength reduction has replaced `c * i` (one
// multiplication per iteration) with an accumulator `k += c`. This is a
// classic native-perf optimisation: addition is cheaper than multiplication
// on most CPUs. But under symbolic execution it is a DISASTER.
//
// The reason: at iteration i, the symbolic value `k` is the chain
// `0 + c + c + c + ... + c` (i adds). Each new query about y[i] sees the
// full accumulated expression tree, and the constraint solver has to
// reason about a deeply-nested `Add(c, Add(c, Add(c, ...)))` expression
// — far harder than reasoning about a single `Mul(c, i)`.
//
// Cadar reports the strength-reduced version at 20 MINUTES vs 2.2 seconds
// for the equivalent multiplication version (see remedy-01.c) — a 545x
// slowdown, caused by an optimisation that *speeds up* native execution.
//
// The remedy is the inverse compiler transformation: induction variable
// substitution. Recover `c * i` and let the solver multiply by a constant
// rather than chain symbolic additions.

int main(void) {
  int c;
  klee_make_symbolic(&c, sizeof(c), "c");
  int y[500];
  int k = 0;
  int even = 0, odd = 0;
  for (int i = 0; i < 500; i++) {
    y[i] = k;
    k = k + c; // strength-reduced from `y[i] = c * i;`
    if (y[i] % 2 == 0) {
      even++;
    } else {
      odd++;
    }
  }
  printf("even=%d odd=%d\n", even, odd);
  return 0;
}
