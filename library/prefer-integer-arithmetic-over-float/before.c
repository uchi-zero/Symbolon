#include<stdio.h>
int main(){
	long long x,y=100,i=0;
	scanf("%lld",&x);
	while(1) {
		y*=1.01;
		(long long)y;
		i++;
		if(y>=x) break;
	}
	printf("%lld",i);
	return 0;
}