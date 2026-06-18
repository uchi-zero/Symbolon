#include<stdio.h>
int main(void)
{
	int a,b,c,d,e,f,x,y,z;
	scanf("%d %d %d %d %d %d",&a,&b,&c,&d,&e,&f);
	y=a+b+c+d;
	x=a;
	if(b<x){
			x=b;
	}
	if(c<x){
			x=c;
	}
	if(d<x){
			x=d;
	}
	z=y-x;
	if(e<f) printf("%d\n",f+z);
	else 	printf("%d\n",e+z);
	return 0;
}