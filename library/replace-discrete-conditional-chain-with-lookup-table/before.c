#include<stdio.h>
int main(void)
{
	int a,b,c,d,t,i;
	for(i=1;i<=4;i++){
		scanf("%d %d",&a,&b);
		if(a==1){
			a=6000;
		}
		else if(a==2){
			a=4000;
		}
		else if(a==3){
			a=3000;
		}
		else if(a==4){
			a=2000;
		}
		c=a*b;
		printf("%d\n",c);
	}
	
		
	return 0;
}