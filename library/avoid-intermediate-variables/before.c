#include<stdio.h>
int main(void)
{
    int a,b,c;
	scanf("%d %d",&a,&b);
	c=a+b;
	if(c%3==0||a%3==0||b%3==0) printf("Possible\n");
	else printf("Impossible\n");
	return 0;
}