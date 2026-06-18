#include "klee/klee.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// A parser-style fragment: copy `n` symbolic header bytes into dst, then
// check the magic value. Both the byte content and the length are
// symbolic, so memcpy forks once per possible value of n in [0, 40].
int main(void) {
  char src[40];
  klee_make_symbolic(src, sizeof(src), "src");
  size_t n;
  klee_make_symbolic(&n, sizeof(n), "n");
  klee_assume(n <= sizeof(src));
  char dst[40] = {0};
  memcpy(dst, src, n);
  if (dst[0] == 'G' && dst[1] == 'I' && dst[2] == 'F' && dst[3] == '8') {
    printf("OK\n");
  }
  return 0;
}
