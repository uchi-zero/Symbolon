#include <stdio.h>
#include <math.h>

int main(int argc, char const *argv[])
{
  int n,m;

  scanf("%d",&n);
  m=ceil(n/1.08);
  if(n==(int)(m*1.08)) printf("%d",m);
  else printf(":(");
  return 0;
}