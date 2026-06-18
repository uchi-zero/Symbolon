#include "klee/klee.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  size_t n;
  klee_make_symbolic(&n, sizeof(n), "n");
  klee_assume(n <= 40);
  int indicator = klee_range(0, 2, "indicator");
  if (indicator == 0) {
    klee_assume(n == 1);
    char buf[40];
    memset(buf, 'A', n);
    if (buf[0] == 'A') {
      printf("OK\n");
    }
  }
  return 0;
}
