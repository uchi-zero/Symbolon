#include <stdio.h>
int main(void){
	int a,b;
	scanf("%d %d",&a,&b);
	printf("%d",a-1 + !(b<a));
}