#include "klee/klee.h"
#include <stdlib.h>
#include <stdio.h>

// Dynamically-sized hash table indexed by a symbolic key (the combine 0.4.0
// `find_table_entry` shape). Unlike obstacle-01/02/03 (a *constant* lookup
// table), here the array is heap-allocated with a tunable SIZE constant, its
// slots are filled from symbolic input (as combine fills its table from the
// symbolic reference-file records), and the lookup index is `hash(sym) % SIZE`.
// On the symbolic-index read KLEE must forward the WHOLE table to the solver, so
// the query cost scales with SIZE. At a production size like 32768 this is a
// multi-megabyte symbolic-array read that dominates wall time (KLEE prints
// "Symbolic memory read will send N bytes to the constraint solver").
#define TABLE_SIZE 4096

struct entry {
  int value;
  char pad[52]; /* wide slots, like combine's 56-byte val_entry */
};

int main(void) {
  unsigned long k;
  unsigned char seed[4];
  klee_make_symbolic(&k, sizeof(k), "k");
  klee_make_symbolic(seed, sizeof(seed), "seed");

  struct entry *table = calloc(TABLE_SIZE, sizeof(*table));
  if (table == NULL)
    return 1;

  /* Populate a few slots from symbolic input so the contents are not a
   * constant KLEE can fold away (mirrors combine inserting symbolic records). */
  for (int i = 0; i < 4; i++)
    table[(seed[i] * 2654435761u) % TABLE_SIZE].value = seed[i] + 1;

  /* symbolic index into the full table: KLEE forwards all TABLE_SIZE slots */
  unsigned long idx = (k * 2654435761u) % TABLE_SIZE;
  if (table[idx].value != 0) {
    printf("hit\n");
  }
  free(table);
  return 0;
}
