#include <stdio.h>

int main(void) {
  int i, j, n, m, k;
  scanf("%d%d%d", &n, &m, &k);
  for(i = 0; i <= n; ++i) for(j = 0; j <= m; ++j) if(i * j + (n - i) * (m - j) == k) {
    printf("Yes");
    return 0;
  }
  printf("No");
  return 0;
}