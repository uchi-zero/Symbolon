#include<stdio.h>
int main(void)
{
	long long int n,i,w=1,c=0;
	scanf("%lld",&n);
	for(i=1;i<=n;i++){
		w=w*i;
		w=w%1000000007;
	}
	printf("%lld\n",w);
	return 0;
}