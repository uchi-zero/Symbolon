#include<stdio.h>
int main()
{
	int n,i,x;
	scanf("%d",&n);
	x=1;
	while(1){
		x*=2;
		if(n<x){
			break;
		}
	}
	printf("%d\n",x/2);
	return 0;
}