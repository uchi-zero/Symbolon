#include <stdio.h>

int main(void) {
  int l, a, b, c, d;
  scanf("%d%d%d%d%d", &l, &a, &b, &c, &d);
  a = (a + c - 1) / c, b = (b + d - 1) / d;
  int m = a > b ? a : b;
  printf("%d\n", l - m);
  return 0;
}