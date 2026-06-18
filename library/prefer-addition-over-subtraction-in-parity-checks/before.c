#include <stdio.h>

int main(void) {
  int a, b;
  scanf("%d%d", &a, &b);
  if((a - b) % 2) printf("IMPOSSIBLE");
  else printf("%d", (a + b) / 2);
  return 0;
}