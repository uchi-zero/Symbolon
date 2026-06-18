#include<stdio.h>
int main(void)
{
	int a,b;
	scanf("%d %d",&a,&b);
	a+=b;
	if(a>23)a-=24;
	printf("%d\n",a);
	return 0;
}