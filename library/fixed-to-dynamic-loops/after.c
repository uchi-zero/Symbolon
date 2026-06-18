#include <stdio.h>

int main(void){
  int n, c;

  scanf("%d", &n);

  c = 0;
  while (n > 0) {
    if (n % 10 == 2) {
      c++;
    }
    n /= 10;
  }

  printf("%d\n", c);
}