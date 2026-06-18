#include<stdio.h>
int main()
{
	int m,f,b;
	scanf("%d %d %d",&m,&f,&b);
	if(m>=b){
		printf("0\n");
	}
	else if(f>=b){
		printf("%d\n",b-m);
	}
	else if(m+f>=b){
		printf("%d\n",b-m);
	}
	else printf("NA\n");
	return 0;
}