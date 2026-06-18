#include<stdio.h>


int main(){
	int n,a,b;
	scanf("%d%d%d",&n,&a,&b);
	if(a>b||(n==1&&b!=a)){
		printf("0");
	}else{
		printf("%lld\n",(long long )(b-a)*(n-2)+1);
	}
}