#include<stdio.h>
int main(void)
{
	int w,h,x,y,r;
	scanf("%d %d %d %d %d",&w,&h,&x,&y,&r);
	if(0<=x-r&&0<=y-r&&x+r<=w&&y+r<=h){
					printf("Yes\n");
		}
	else{
			printf("No\n");
		}
	return 0;
}