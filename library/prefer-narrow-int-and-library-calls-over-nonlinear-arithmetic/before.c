#include <stdio.h>

int main()
{
	long long a, b, c; scanf ("%lld %lld %lld", &a, &b, &c);

	if (c - a - b <= 0) puts("No");
	else puts(4 * a * b < (c - a - b) * (c - a - b) ? "Yes" : "No");
	return 0;
}