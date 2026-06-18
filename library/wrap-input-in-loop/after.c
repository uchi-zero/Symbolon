#include<stdio.h>
int main(){
	int m,n;
	while(~scanf("%d%d",&m,&n)){
		if(m>n)
		printf("%d",m-1);
		else
		printf("%d\n",m);
	}
}