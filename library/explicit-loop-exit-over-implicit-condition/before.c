#include <stdio.h>
#include <stdlib.h>

int main(void) {
  long long a, ans;
  while( 1 ) {
    ans = 0;
    scanf("%lld", &a);
    if(!a) break;
    while(a) {
      ans += a / 5;
      a /= 5;
    }
    printf("%lld\n", ans);
  }
  return 0;
}