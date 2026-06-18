// state-killer-finder: --enable=all
// state-killer-finder-no-debug
#include <stdio.h>
int main(void) {
  int a = 0;
  __asm__("" : "+r"(a)); // defeat constant-fold so a is non-constant
  printf("%d", a);
  return 0;
}
