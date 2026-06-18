// state-killer-finder: --enable=SymbolicAllocSize
#include <klee/klee.h>
#include <stdlib.h>
int main(void) {
  size_t n;
  klee_make_symbolic(&n, sizeof n, "n");
  klee_assume(n > 0 && n < 64);
  int *p = calloc(n, sizeof(int));
  if (!p)
    return 1;
  int first = p[0];
  free(p);
  return first;
}
