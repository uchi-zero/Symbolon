#include "klee/klee.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// Pin n to the smallest length that lets the downstream check be
// satisfiable: the magic compare needs 4 bytes copied, so n = 4.
int main(void) {
  char src[40];
  klee_make_symbolic(src, sizeof(src), "src");
  size_t n;
  klee_make_symbolic(&n, sizeof(n), "n");
  klee_assume(n <= sizeof(src));
  int indicator = klee_range(0, 2, "indicator");
  if (indicator == 0) {
    klee_assume(n == 4);
    char dst[40] = {0};
    memcpy(dst, src, n);
    if (dst[0] == 'G' && dst[1] == 'I' && dst[2] == 'F' && dst[3] == '8') {
      printf("OK\n");
    }
  }
  return 0;
}
