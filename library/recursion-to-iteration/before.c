#include <stdio.h>

long long fac(int n) {
	return n == 0 ? 1 : fac(n - 1) * n % 1000000007;
}

int main() {
	int n;

	scanf("%d", &n);
	printf("%lld\n", fac(n));
	return 0;
}