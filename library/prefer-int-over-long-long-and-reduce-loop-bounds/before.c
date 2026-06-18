#include <stdio.h>

long long a,b,c;

int main()
{
	scanf ("%lld %lld %lld",&a,&b,&c);

	for (int t=0;t<10000;t++){
		if (a % 2 || b % 2 || c % 2){
			printf ("%d\n",t);
			return 0;
		}
		long long p = (b + c) / 2;
		long long q = (c + a) / 2;
		long long r = (a + b) / 2;
		a = p; b = q; c = r;
	}
	puts("-1");

	return 0;
}