#include <stdio.h>

int main()
{
	long long a,i;
	const long long MOD = 1000000007;
	scanf("%lld",&a);
	for(i=a-1;i>0;i--)
	{
		a = a*i%MOD;
	}
	printf("%lld\n",a);
}