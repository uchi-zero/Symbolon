#include<stdio.h>
#include<math.h>
int main(void)
{
  long double A,B;
  if(scanf("%Lf %Lf",&A,&B)!=0)return printf("%lld",(long long)floor(A*B)),0;
}