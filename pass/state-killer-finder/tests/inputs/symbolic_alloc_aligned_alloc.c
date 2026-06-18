// state-killer-finder: --enable=SymbolicAllocSize
#include <klee/klee.h>
#include <stdlib.h>
int main(void) {
  size_t n;
  klee_make_symbolic(&n, sizeof n, "n");
  klee_assume(n > 0 && n < 1024);
  void *p = aligned_alloc(64, n);
  if (!p)
    return 1;
  free(p);
  return 0;
}
