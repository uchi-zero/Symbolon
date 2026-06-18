#include<stdio.h>
int main(){
	long long x,y=100;
	scanf("%lld",&x);
	for(int i=1;;++i){
		y=y*1.01;
		if(y>=x){
			printf("%d\n",i);
			break;
		}
	}
}