// state-killer-finder: --enable=SymbolicVectorIndex
#include <klee/klee.h>
typedef int v4i __attribute__((vector_size(16)));
int main(void) {
  v4i v = {10, 20, 30, 40};
  unsigned i;
  klee_make_symbolic(&i, sizeof i, "i");
  klee_assume(i < 4);
  return v[i];
}
