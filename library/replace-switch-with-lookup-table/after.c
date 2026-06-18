#include <stdio.h>

int main(void) {
  int i, j, k, a, b, p[5] = {0, 6000, 4000, 3000, 2000};
  for(i = 0; i < 4; ++i) {
    scanf("%d%d", &a, &b);
    printf("%d\n", p[a] * b);
  }
  return 0;
}