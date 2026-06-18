#include<stdio.h>
int main(void)
{
	int N,h[10],i,n,f,a,k;
	char c;
	scanf("%d",&N);
	while(N!=0){
		for(i=0;i!=10;i++){
			h[i]=0;
		}
		f=0;
		for(i=1;i!=102;i++){
			n=i%N;
			scanf("%1c",&c);
			switch(c){
				case 'M':
				h[n]+=1;
				break;
				case 'S':
				f+=h[n]+1;
				h[n]=0;
				break;
				case 'L':
				h[n]+=f+1;
				f=0;
				break;
			}
		}
		for(i=0;i!=N;i++){
			for(a=i;a!=N;a++){
				if(h[i]>h[a]){
					k=h[i];
					h[i]=h[a];
					h[a]=k;
				}
			}
		}
		for(i=0;i!=N;i++){
			printf("%d ",h[i]);
		}
		printf("%d\n",f);
		scanf("%d",&N);
	}
	return 0;
}