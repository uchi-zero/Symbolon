#include "klee/klee.h"
#include <stdio.h>

// Figure 3 from Cadar, FSE 2015 ("Targeted Program Transformations for
// Symbolic Execution"). A switch statement whose default branch
// contains a bug (division by zero) that is only reachable when none
// of the explicit cases match.
//
// How clang lowers the switch determines how quickly KLEE finds the
// default-branch bug:
//
//   - Linear chain of if-else (one comparison per case): with N cases,
//     the path constraint at the default is `x != 1 ∧ x != 2 ∧ ... ∧
//     x != N`. KLEE explores each case path before reaching default.
//     For N≥10 this often exceeds the budget.
//
//   - Balanced binary search (jump table or range tests): the path
//     constraint at the default is approximately `x < low ∨ x > high`.
//     KLEE reaches default after O(log N) decisions. Cadar reports
//     2.3s with BFS+binary-search vs >1 hour with linear chain.
//
// The lowering is a clang/LLVM CHOICE, not a source-level property of
// the program. To force one or the other:
//
//   - Default (jump-table-friendly): clang at -O1 or higher.
//   - Force linear chain: -O0 -fno-jump-tables.
//   - Force binary search: -fmllvm -switch-range-to-icmp=true at -O1+.
//
// This file is identical to remedy-01.c at the source level — the
// difference must be made at COMPILE time via clang flags. See the
// directory README for the recommended build invocation.

static int expensive(int v) {
  int bits = 0;
  for (int i = 0; i < 32; i++) {
    if (v & (1 << i))
      bits++;
  }
  return bits;
}

int foo(int x, int y) {
  switch (x) {
  case 1:
    return expensive(y + 1);
  case 2:
    return expensive(y + 2);
  case 3:
    return expensive(y + 3);
  case 4:
    return expensive(y + 4);
  case 5:
    return expensive(y + 5);
  case 6:
    return expensive(y + 6);
  case 7:
    return expensive(y + 7);
  case 8:
    return expensive(y + 8);
  default:
    return x / y; // bug: division by zero when y == 0
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
