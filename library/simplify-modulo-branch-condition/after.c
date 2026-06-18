#include<stdio.h>

int x,y;

int main(void){
	scanf("%d%d",&x,&y);
	if(x%y)
		printf("%d",x);
	else
		printf("-1");
	return putchar('\n'),0;
}