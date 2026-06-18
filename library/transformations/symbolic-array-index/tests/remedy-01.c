#include "klee/klee.h"
#include <stdio.h>

// Index-based transformation of Figure 1 (Perry et al., ISSTA 2017).
// The 256-element b64 table has positive values exactly at indices
// 43, 47-57, 65-90, 97-122. The predicate `b64[k] >= 0` is rewritten
// as a single boolean expression over k that uses *bitwise* operators
// (& and |) instead of short-circuit ones (&& and ||). This is crucial:
// short-circuit operators cause KLEE to fork at each conjunct/disjunct,
// trading one expensive array query for many cheap branches. Bitwise
// operators on 0/1 ints produce a single symbolic boolean, so KLEE
// issues exactly one branch query — and that query is in linear-integer
// arithmetic, not array theory.
int main(void) {
  unsigned char k;
  klee_make_symbolic(&k, sizeof(k), "k");
  int valid = (k == 43) | ((k >= 47) & (k <= 57)) | ((k >= 65) & (k <= 90)) |
              ((k >= 97) & (k <= 122));
  if (valid) {
    printf("base64\n");
  }
  return 0;
}
