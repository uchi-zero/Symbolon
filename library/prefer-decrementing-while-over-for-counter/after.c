#include<stdio.h>

int main(void){

	int i=7,a,b;
	while(i!=0){
		scanf("%d %d",&a,&b);
		printf("%d\n",a-b);
		i--;
	}
	return 0;
}