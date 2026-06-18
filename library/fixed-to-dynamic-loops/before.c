#include<stdio.h>

int main()
{
  int n;
  scanf("%d",&n);
  int s=0;
  for (int i=0;i<4;i++)
  {
    if (n%10==2) s++;
    n=n/10;
  }
  printf("%d\n",s);
  return 0;
}