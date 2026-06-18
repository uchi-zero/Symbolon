#include <stdio.h>

int main(void)
{
	long long a, b, c;

	scanf("%lld %lld.%lld" ,&a, &b, &c);

	printf("%lld" ,a * (b * 100 + c) / 100);
	return 0;
}