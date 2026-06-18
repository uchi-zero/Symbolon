#include<stdio.h>

int main(){
	int a,b,c;
	scanf("%d %d",&a,&b);
	c=a+b;
	if(c>=24) c-=24;
	printf("%d",c);
	return 0;
}