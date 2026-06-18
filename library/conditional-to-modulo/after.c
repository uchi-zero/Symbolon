#include<stdio.h>
int main(){
	int a,b,m;
	while(scanf("%d %d",&a,&b)!=EOF){
		m =(a+b)%24;
		printf("%d\n",m);
	}
}