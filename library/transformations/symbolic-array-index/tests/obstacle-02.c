#include "klee/klee.h"
#include <stdio.h>

// Figure 2 from Perry et al., ISSTA 2017. A DFA-style lexer with nested
// symbolic-indexed reads. The constraint formula for `state =
// dfa_next[state][cls]` cross-products the two arrays' index-to-offset
// axioms; over multiple iterations the formula grows multiplicatively.
//
// At LOOPS=80 with STP, KLEE times out at 120s without completing any
// path (0 completed, 27 queries answered, ~4.4s per query). With Z3 the
// same run answers 93 queries (~1.3s each) but still 0 completed paths.
// The obstacle is solver-bound, not path-explosion-bound.

#define LOOPS 6

static const unsigned char equiv_class[256] = {
    [48 ... 57] = 1,  // digits
    [65 ... 90] = 2,  // upper
    [97 ... 122] = 3, // lower
};

#define N_STATES 16
#define N_CLASSES 4
#define ACCEPT 7

static const unsigned char dfa_next[N_STATES][N_CLASSES] = {
    {0, 1, 2, 0},      {0, 1, 0, 0},
    {0, 0, 3, 0},      {0, 0, 3, 4},
    {0, 0, 0, 5},      {0, 0, 0, 6},
    {0, 0, 0, ACCEPT}, {ACCEPT, ACCEPT, ACCEPT, ACCEPT},
    {0, 0, 0, 0},      {0, 0, 0, 0},
    {0, 0, 0, 0},      {0, 0, 0, 0},
    {0, 0, 0, 0},      {0, 0, 0, 0},
    {0, 0, 0, 0},      {0, 0, 0, 0},
};

int main(void) {
  unsigned char input[LOOPS];
  klee_make_symbolic(input, sizeof(input), "input");

  unsigned char state = 0;
  for (int i = 0; i < LOOPS; i++) {
    unsigned char cls = equiv_class[input[i]];
    state = dfa_next[state][cls];
  }

  if (state == ACCEPT) {
    printf("OK\n");
  }
  return 0;
}
