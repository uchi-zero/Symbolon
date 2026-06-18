#include<stdio.h>
int main(void)
{
	int a,b,x;
	scanf("%d %d %d",&a,&b,&x);
	while(1){
		if(a<=b){
			if(x%1000==0)	printf("%d\n",a*(x/1000));
			else	printf("%d\n",a*(x/1000+1));
			break;
		}
		if(a>=b*2){
			if(x%500==0)	printf("%d\n",b*(x/500));
			else	printf("%d\n",b*(x/500+1));
			break;
		}
		if(a<b*2){
			if(x%1000==0)	printf("%d\n",a*(x/1000));
			else{
				if(x%1000>500)	printf("%d\n",(a*(x/1000))+a);
				else	printf("%d\n",(a*(x/1000))+b);
			}
			break;
		}
	}
	return 0;
}