#include <stdio.h>
typedef long long ll;

int main(void) {
  ll n, i, j;
  scanf("%lld", &n);
  for(i = 1; i <= 3500; ++i) for(j = 1; j <= 3500; ++j) if(4 * i * j - i * n - j * n > 0 && !((n * i * j) % (4 * i * j - i * n - j * n))) {
    printf("%lld %lld %lld", i, j, (n * i * j) / (4 * i * j - i * n - j * n));
    return 0;
  }
}