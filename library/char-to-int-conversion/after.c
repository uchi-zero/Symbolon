#include<stdio.h>
int main(){
	int n,a=-1;
	scanf("%d",&n);
	for(int i=1;i<=4;i++){
		if(n%10==a){
			printf("Bad");
			return 0;
		}
		a=n%10;
		n/=10;
	}
	printf("Good");
	return 0;
}