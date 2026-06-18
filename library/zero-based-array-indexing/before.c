#include <stdio.h>

int main()
{
	int a[13] = {0,1,2,1,3,1,3,1,1,3,1,3,1};	
	int x,y;
//	while(){
	scanf("%d%d",&x,&y);
	printf(a[x] - a[y] ? "No" : "Yes");
	return 0;
}