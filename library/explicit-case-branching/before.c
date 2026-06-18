#include<stdio.h>
int main(){
	long long a,a1,a2;
	scanf("%lld %lld %lld",&a,&a1,&a2);
	long long t = a1+(a-1)*a2-(a1*(a-1)+a2);
	if(t>=0){
		printf("%lld\n",t+1);
	}
	else printf("0\n");
}