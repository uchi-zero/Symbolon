#include<stdio.h>
int main()
{
	int a,b;
	char c[12];
	scanf("%d",&a);
	scanf("%s",c);
	scanf("%d",&b);
	for(int i=0;i<a;i++)
	{
		if(c[i]!=c[b-1])
		c[i]='*';
	}
	printf("%s",c);
}