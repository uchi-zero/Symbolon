// state-killer-finder: --enable=SymbolicAllocSize
#include <klee/klee.h>
int sum(unsigned n) {
  int buf[n];
  int s = 0;
  for (unsigned i = 0; i < n; i++) {
    buf[i] = (int)i;
    s += buf[i];
  }
  return s;
}
int main(void) {
  unsigned n;
  klee_make_symbolic(&n, sizeof n, "n");
  klee_assume(n > 0 && n < 64);
  return sum(n);
}
