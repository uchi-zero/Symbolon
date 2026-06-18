#include <stdio.h>
int main()
{
  int a,b;
  scanf("%X %X",&a,&b);
  if(a>b)printf(">");
  else if(a<b)printf("<");
  else printf("=");
}