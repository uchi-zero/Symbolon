#include<stdio.h>
int main()
{
  int a, b, c, d, e;
  scanf("%d", &e);
  a=e/100;
  b=e-100*a;
  if(a>=1&&a<=12)
  {
  	if(b>=1&&b<=12)
  	printf("AMBIGUOUS");
  	else
  	printf("MMYY");
  }
  else
  {
  	if(b>=1&&b<=12)
  	printf("YYMM");
  	else
  	printf("NA");
  }
}