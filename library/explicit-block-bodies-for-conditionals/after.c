#include <stdio.h>
int main(void)
{
	int a,b,c,x,i,j,k,z=0;
	scanf("%d%d%d%d",&a,&b,&c,&x);
	for(i=0; i<=a; i++){
		for(j=0; j<=b; j++){
			for(k=0; k<=c; k++){
				if(i*500+j*100+k*50==x){
					z=z+1;
				}
			}
		}
	}
	printf("%d\n",z);
	return 0;
}