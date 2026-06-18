#include<stdio.h>
int a,b,c,d,e,k;
int main()
{
	scanf("%d%d%d%d%d%d",&a,&b,&c,&d,&e,&k);
	if(e-a>k)puts(":(");
	else puts("Yay!");
}