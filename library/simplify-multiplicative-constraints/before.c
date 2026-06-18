#include<stdio.h>
int main()
{
	int m,n;
	scanf("%d%d",&m,&n);
	if(m*n==3 ||m*n==1 ||m*n==9) printf("Yes");
	else printf("No");
	return 0;
}