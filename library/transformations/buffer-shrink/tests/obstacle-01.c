#include "klee/klee.h"
#include <stdio.h>

// Pattern 6 from Cadar, FSE 2015 ("Targeted Program Transformations for
// Symbolic Execution"). A large symbolic buffer drives a parser-style
// loop where each iteration's branch depends on a different byte of
// the buffer. Path explosion is exponential in buffer size.
//
// This is distinct from symbolic-bound-mem (concretize the length
// argument to memcpy/memset): here the *capacity* of the symbolic
// buffer is what's expensive, not a length operand.

#define BUFSIZE 1024

int main(void) {
  char buf[BUFSIZE];
  klee_make_symbolic(buf, sizeof(buf), "buf");

  // Count the bytes that match a magic prefix-marker. With BUFSIZE=1024
  // and a per-byte branch, the loop forks 1024 times — KLEE cannot
  // finish within reasonable budgets.
  int matches = 0;
  for (int i = 0; i < BUFSIZE; i++) {
    if (buf[i] == 0x42) {
      matches++;
    }
  }

  if (matches > 10) {
    printf("many\n");
  }
  return 0;
}
