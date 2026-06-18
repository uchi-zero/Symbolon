#include<stdio.h>
int main(void)
{
	int a,c;
	char b;
	scanf("%d %c %d",&a,&b,&c);
	if(b=='-') a-=c;
	if(b=='+') a+=c;
	printf("%d\n",a);
	return 0;
}