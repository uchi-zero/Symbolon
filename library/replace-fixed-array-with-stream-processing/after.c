#include<stdio.h>
int main(void)
{
  int rainy=0,max=0,c=getchar();
  while(c=='R'||c=='S'){{if(c=='R')rainy++;else rainy=0;}max=max<rainy?rainy:max;c=getchar();}
  return printf("%d\n",max),0;
}