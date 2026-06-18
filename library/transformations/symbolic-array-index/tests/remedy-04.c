#include "klee/klee.h"
#include <stdlib.h>
#include <stdio.h>

// SHRINK-THE-ALLOCATION-DIMENSION remedy (combine 0.4.0 r20). When the array
// indexed by a symbolic value is a dynamically-sized *container* (hash table,
// bucket array, free-list) whose dimension is a tunable constant rather than a
// fixed lookup table, the symbolic-array-read query cost scales with that
// dimension. If the symbolic environment can only put a handful of live entries
// into the container (here, as in combine, the symbolic input is tiny), shrink
// the dimension to just above the live-entry ceiling: the index stays symbolic
// and the lookup logic is unchanged, but the array KLEE forwards to the solver
// drops from SIZE*slot to a few KB, so the symbolic-array query collapses.
//
// In a REAL program the shrink is gated to the KLEE build so the production /
// replay binary keeps the original size and behaviour is byte-identical:
//
//     #ifdef KLEE_BITCODE
//     #define TABLE_SIZE 64        /* analysis build: small symbolic array */
//     #else
//     #define TABLE_SIZE 32768     /* production / coverage-replay build   */
//     #endif
//
// (combine ships exactly this in src/df_common.h: DEFAULT_HASHTABLE_SIZE 64
// under KLEE_BITCODE, 32767 otherwise.) The verify-toy harness compiles a
// single build without -DKLEE_BITCODE, so this toy uses the shrunk size
// directly to demonstrate the solver-time win the shrink produces.
#define TABLE_SIZE 64

struct entry {
  int value;
  char pad[52];
};

int main(void) {
  unsigned long k;
  unsigned char seed[4];
  klee_make_symbolic(&k, sizeof(k), "k");
  klee_make_symbolic(seed, sizeof(seed), "seed");

  struct entry *table = calloc(TABLE_SIZE, sizeof(*table));
  if (table == NULL)
    return 1;

  for (int i = 0; i < 4; i++)
    table[(seed[i] * 2654435761u) % TABLE_SIZE].value = seed[i] + 1;

  unsigned long idx = (k * 2654435761u) % TABLE_SIZE;
  if (table[idx].value != 0) {
    printf("hit\n");
  }
  free(table);
  return 0;
}
