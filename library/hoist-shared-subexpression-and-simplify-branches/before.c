#include <stdio.h>

int main(void)
{
	long long X,K,D,answer,amari;
	scanf("%lld",&X);
	scanf("%lld",&K);
	scanf("%lld",&D);
	if(X<0)X=-1*X;
	if(X/D>K)answer=X-D*K;
	else{
		amari=X%D;
		K-=X/D;
		if(K%2!=0)answer=D-amari;
		else answer=amari;
	}
	printf("%lld",answer);
	return 0;
}