#include <stdio.h>

int prime(int n)
{
	int i;
	for(i=n/2; i>=2; i--){
		if(n%i==0){
			return 1;
		}
	}
	return 0;
}

int main(void)
{
	int n,i,j,x,y,z=0;
	scanf("%d",&n);
	for(i=0; i<n; i++){
		for(j=2; z<n; j=j+2){
			x=j*5+1;	y=prime(x);
			if(y==0){
				z=z+1;
				printf("%d",x);
				if(z<n){
					printf(" ");
				}
			}
		}
	}
	printf("\n");
	return 0;
}