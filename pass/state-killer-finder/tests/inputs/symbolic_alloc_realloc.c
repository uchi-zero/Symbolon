// state-killer-finder: --enable=SymbolicAllocSize
#include <klee/klee.h>
#include <stdlib.h>
int main(void) {
  size_t n;
  klee_make_symbolic(&n, sizeof n, "n");
  klee_assume(n > 0 && n < 1024);
  char *p = malloc(8);
  if (!p)
    return 1;
  p = realloc(p, n);
  if (!p)
    return 1;
  int first = p[0];
  free(p);
  return first;
}
