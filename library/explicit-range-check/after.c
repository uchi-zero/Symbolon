#include<stdio.h>
int main(void)
{
  int c=getchar();
  if(c>='A'&&c<='Z')return puts("A"),0;
  else return puts("a"),0;
}