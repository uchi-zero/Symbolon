// state-killer-finder: --enable=ExternCallSymArg
#include <stdio.h>
int compute(int x);
int main(void) {
  int a = compute(3);
  printf("%d", a);
  return 0;
}
