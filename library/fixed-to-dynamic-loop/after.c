#include <stdio.h>

int main()
{
  int n;
  scanf("%d", &n);
  int ans =0;
  while(n)
  {
    int tmp = n%10;
    if (tmp ==2) ans++;
    n/=10;
  }
  printf("%d\n", ans);


}