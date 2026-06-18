// state-killer-finder: --enable=SymbolicVectorIndex
#include <klee/klee.h>
typedef int v4i __attribute__((vector_size(16)));
int main(void) {
  v4i v = {1, 2, 3, 4};
  unsigned i;
  int x;
  klee_make_symbolic(&i, sizeof i, "i");
  klee_make_symbolic(&x, sizeof x, "x");
  klee_assume(i < 4);
  v[i] = x;
  return v[0] + v[1] + v[2] + v[3];
}
