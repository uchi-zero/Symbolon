#include<stdio.h>
int main()
{
	int x,y;
	scanf("%d%d",&x,&y);
	if(x%y==0)
		printf("-1\n");
	else
		printf("%d\n",x);
  return 0;
}