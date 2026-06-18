// state-killer-finder: --enable=InlineAsm
#include <klee/klee.h>
int main(void) {
  int x;
  klee_make_symbolic(&x, sizeof x, "x");
  int y;
  asm volatile("movl %1, %0" : "=r"(y) : "r"(x));
  return y;
}
