#include<stdio.h>
int main()
{
	int i,b=0,c=0;
	char s[5];
	scanf("%s",s);
	for(i=0;i<3;i++)
	{
		if(s[i]=='R')c++;
		else 
		c=0;
		b=c>b?c:b;
	}
	printf("%d\n",b);
}