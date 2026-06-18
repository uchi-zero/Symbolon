#include <stdio.h>


int main(void) {
  int m, f, b;
  scanf("%d%d%d",&m, &f, &b);
  if(m + f >= b)printf("%d\n", b - m > 0 ? b - m : 0);
  else printf("NA\n");
  return 0;
}