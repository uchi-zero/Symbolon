#include<stdio.h>
char c;
int main()
{
	c=getchar();
  if(c=='A')c='T';
  else if(c=='G')c='C';
  else if(c=='T')c='A';
  else if(c=='C')c='G';
  putchar(c);
}/**/