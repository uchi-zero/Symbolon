#include<stdio.h>
int main()
{
	int x, y, z;
	scanf("%d %d %d",&x,&y,&z);
	x = x - z;
	printf("%d\n",x/(y+z));
}