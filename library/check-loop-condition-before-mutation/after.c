#include<stdio.h>
int main(void)
{
	int a,b,c,x,y;
	scanf("%d %d %d",&a,&b,&c);
	x=0;	y=0;	
	while(1){
		if(x>=c) break;
		x+=a;
		y++;
		if(y%7==0)  x+=b;
	}
	printf("%d\n",y);
	return 0;
}