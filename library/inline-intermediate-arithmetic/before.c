#include<stdio.h>
int main(void)
{
	int x,y,z,w,a,b,c;
	scanf("%d %d %d",&x,&y,&z);
	w=y+z;
	a=x-z;
	printf("%d\n",a/w);
	return 0;
}