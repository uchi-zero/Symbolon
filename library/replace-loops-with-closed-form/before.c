#include<stdio.h>
int main(){
	int x,N,M,m[300],i;
	int num = 0;
	scanf("%d %d",&N,&M);
	for(i=0;i<M;i++){
		scanf("%d",&m[i]);
	}
	//num????£N/2
	x = N;
	for(;;){
		x = x/2;
		num = num + x;
		if(x == 1) break;
	}
	num = num - M;
	printf("%d\n",num);
	return 0;	
}