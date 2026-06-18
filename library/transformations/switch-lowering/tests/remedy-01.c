#include "klee/klee.h"
#include <stdio.h>

// Source-level binary-search rewrite of obstacle-01.c. When you can't
// (or don't want to) rely on clang to lower the switch as a balanced
// tree, write the tree by hand. KLEE then sees range comparisons that
// halve the unexplored space at each step.
//
// For the 8-case obstacle above, the rewrite below reaches the default
// (buggy) branch via approximately log2(8) = 3 comparisons, regardless
// of search heuristic.
//
// This is verbose but mechanical. Generate it from the switch's case
// list when applying at scale.

static int expensive(int v) {
  int bits = 0;
  for (int i = 0; i < 32; i++) {
    if (v & (1 << i))
      bits++;
  }
  return bits;
}

int foo(int x, int y) {
  if (x < 1 || x > 8) {
    return x / y; // default — the bug lives here
  }
  if (x <= 4) {
    if (x <= 2) {
      if (x == 1)
        return expensive(y + 1);
      return expensive(y + 2); // x == 2
    } else {
      if (x == 3)
        return expensive(y + 3);
      return expensive(y + 4); // x == 4
    }
  } else {
    if (x <= 6) {
      if (x == 5)
        return expensive(y + 5);
      return expensive(y + 6); // x == 6
    } else {
      if (x == 7)
        return expensive(y + 7);
      return expensive(y + 8); // x == 8
    }
  }
}

int main(void) {
  int x, y;
  klee_make_symbolic(&x, sizeof(x), "x");
  klee_make_symbolic(&y, sizeof(y), "y");
  int r = foo(x, y);
  printf("%d\n", r);
  return 0;
}
