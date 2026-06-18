#include<stdio.h>
int main(){
	int N,Y,b,sum;
	scanf("%d %d",&N,&Y);
	for(int i = 0;i<Y;i++){
		scanf("%d",&b);
	}
	sum = 0;
	for(;;){
		N = N / 2;
		sum = sum + N;
		if(N==1)	break;
	}
	printf("%d\n",sum-Y);
}