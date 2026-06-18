#include<stdio.h>
int main()
{
	int n;
	scanf("%d",&n);
	int flag=0;
	for(int i=0;i<=25;i++)
	for(int j=0;j<=15;j++)
	if(4*i+7*j==n) {
		flag=1;
		break;
		
	}
	if(flag) printf("Yes");
	else  printf("No");
	
	
	
}