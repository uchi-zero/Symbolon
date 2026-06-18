#include <stdio.h>
int V[21], W[21];
int main(){
	int N;
	scanf("%d",&N);
	int sum=0;
	for(int i=0;i<N;i++) scanf("%d",V+i);
	for(int i=0;i<N;i++) scanf("%d",W+i);
	for(int i=0;i<N;i++) sum+=V[i]>W[i]?V[i]-W[i]:0;
	printf("%d",sum);
	
}