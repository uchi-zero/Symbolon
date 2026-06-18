#include<stdio.h>
int d,t,s;
int main()
{
	scanf("%d %d %d", &d, &t, &s);
	d=d+s-1;
	if(t>=(d/s))printf("Yes");
	else printf("No");
}