// state-killer-finder: --enable=CallArityMismatch
#include <klee/klee.h>
int target(int a, int b, int c) { return a + b + c; }
typedef int (*two_arg_fp)(int, int);
int main(void) {
  int a, b;
  klee_make_symbolic(&a, sizeof a, "a");
  klee_make_symbolic(&b, sizeof b, "b");
  return ((two_arg_fp)&target)(a, b);
}
