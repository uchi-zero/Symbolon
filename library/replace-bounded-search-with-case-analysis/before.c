#include<stdio.h>
int main()
{
	int d=1000000000;
	int a,b,c,a1,b1,c1,i,j;
	scanf("%d %d %d",&a,&b,&c);
	a1=c/1000;
	b1=c/500;
	for(i=0;i<a1+2;i++){
		for(j=b1+1;j>=0;j--){
			if(i*1000+j*500>=c){
				c1=i*a+j*b;
				if(c1<=d) d=c1;
			}
		}
	}
	printf("%d\n",d);
	return 0;
}