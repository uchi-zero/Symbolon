#include <stdio.h>
#include <math.h>
typedef long long ll;

int main(void) {
  ll i, j, k, n, m = 1e9, a, b;
  printf("0 0 ");
  scanf("%lld", &n);
  a = (n + m - 1) / m;
  b = (n + a - 1) / a;
  printf("%lld %lld 1 %lld", a, a * b - n, b);
  return 0;
}