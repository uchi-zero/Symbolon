#include<stdio.h>

int main(void){
	int n;
	for(;;){
		scanf("%d",&n);
		if(!n)break;
		int a=0,b;
		for(int i=4;i<=n;i++){
			b=5;
			while(b<=i){
				a+=(!(i%b) && i!=0)? 1 : 0;
				b*=5;
			}
		}
		printf("%d\n",a);
	}
}