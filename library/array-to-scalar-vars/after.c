#include<stdio.h>
int main()
{
	register char a,b,c;
	a=getchar();b=getchar();c=getchar();
	if(a==b&&b==c)puts("No");
	else puts("Yes");
}