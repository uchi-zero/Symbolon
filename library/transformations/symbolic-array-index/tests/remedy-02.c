#include "klee/klee.h"
#include <stdio.h>

// Index-based + value-based hybrid rewrite of the Figure 2 DFA.
// Both nested array reads are eliminated by inlining the lookup tables
// as branchless arithmetic. Each `(s == X) & (k == Y)` evaluates to 0
// or 1; multiplying by the target state and summing gives the same
// result as `dfa_next[s][k]`, but without any array operation and
// (crucially) without any forking — the entire transition is a single
// linear-integer-arithmetic expression that KLEE evaluates in one path.
//
// The verbosity is real: for the populated entries of the original 16x4
// table that's 13 multiply-add terms below. For larger DFAs, generate
// this code from the table values via a script.

#define LOOPS 6
#define ACCEPT 7

static inline unsigned eq_class(unsigned char c) {
  // Each row is 0/1; the * <class_id> selects the class.
  return ((c >= 48) & (c <= 57)) * 1u | ((c >= 65) & (c <= 90)) * 2u |
         ((c >= 97) & (c <= 122)) * 3u;
}

static inline unsigned dfa_step(unsigned s, unsigned k) {
  return ((s == 0) & (k == 1)) * 1u + ((s == 0) & (k == 2)) * 2u +
         ((s == 1) & (k == 1)) * 1u + ((s == 2) & (k == 2)) * 3u +
         ((s == 3) & (k == 2)) * 3u + ((s == 3) & (k == 3)) * 4u +
         ((s == 4) & (k == 3)) * 5u + ((s == 5) & (k == 3)) * 6u +
         ((s == 6) & (k == 3)) * (unsigned)ACCEPT +
         (s == ACCEPT) * (unsigned)ACCEPT;
}

int main(void) {
  unsigned char input[LOOPS];
  klee_make_symbolic(input, sizeof(input), "input");

  unsigned state = 0;
  for (int i = 0; i < LOOPS; i++) {
    unsigned cls = eq_class(input[i]);
    state = dfa_step(state, cls);
  }

  if (state == ACCEPT) {
    printf("OK\n");
  }
  return 0;
}
