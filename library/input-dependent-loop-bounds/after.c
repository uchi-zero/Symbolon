#include <stdio.h>

int main(void)
{
	int N,x,y,z,answer[10001],i;
	scanf("%d",&N);
	for(i=1;i<=N;i++){
		answer[i]=0;
	}
	for(x=1;x*x<N;x++){
		for(y=1;x*x+y*y<N;y++){
			for(z=1;x*x+y*y+z*z<N;z++){
				if(x*x+y*y+z*z+x*y+y*z+z*x<=N)answer[x*x+y*y+z*z+x*y+y*z+z*x]++;
			}
		}
	}
	for(i=1;i<=N;i++)printf("%d\n",answer[i]);
	return 0;
}