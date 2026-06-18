#include "klee/klee.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  size_t n;
  klee_make_symbolic(&n, sizeof(n), "n");
  klee_assume(n <= 40);
  char buf[40];
  memset(buf, 'A', n);
  if (buf[0] == 'A') {
    printf("OK\n");
  }
  return 0;
}
