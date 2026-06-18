#include "klee/klee.h"
#include <stdio.h>

// Buffer shrinking. The symbolic buffer is reduced from 1024 to 16
// bytes. The loop body is unchanged; we just iterate over a smaller
// symbolic region. Path count drops from 2^1024 (infeasible) to 2^16
// (still large, but tractable for short runs and enough to validate
// the parser's loop behaviour).
//
// SEMANTICS-ALTERING: code that depends on the full 1024-byte input
// is no longer exercised. The trade-off is identical to fuzz testing
// with a smaller seed corpus — you cover the algorithmic structure
// but miss bugs that require specific large inputs (e.g. overflow
// at byte 1000).
//
// Use a SHRUNK SIZE that's:
//   - Large enough to exercise the algorithm's relevant cases
//     (e.g. for a header check, ≥ the header size)
//   - Small enough that 2^SIZE is within KLEE's path budget
//   - Often 8-32 bytes for byte-by-byte parsers

#define BUFSIZE 16

int main(void) {
  char buf[BUFSIZE];
  klee_make_symbolic(buf, sizeof(buf), "buf");

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
