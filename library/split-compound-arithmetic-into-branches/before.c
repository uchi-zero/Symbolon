#include<stdio.h>
int main(){
	int a;
	while(~scanf("%d",&a)){
		printf("3C%02d\n",(a-1)%39+1);
	}
}