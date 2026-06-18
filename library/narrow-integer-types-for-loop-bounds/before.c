#include <stdio.h>
long long p = 998244353;
long long a, b, c, d;
long long x[3001][3001];
int main() {
	scanf("%lld%lld%lld%lld", &a, &b, &c, &d);
	x[a][b] = 1;
	for (long long i = a + 1; i <= c; i++) {
		x[i][b] = x[i - 1][b] * b % p;
	}
	for (long long i = b + 1; i <= d; i++) {
		x[a][i] = x[a][i - 1] * a % p;
	}
	for (long long i = a + 1; i <= c; i++) {
		for (long long j = b + 1; j <= d; j++) {
			x[i][j] = (x[i - 1][j] * j + x[i][j - 1] * i) % p;
			x[i][j] -= x[i - 1][j - 1] * (i - 1) * (j - 1) % p;
			x[i][j] += p;
			x[i][j] %= p;
		}
	}
	printf("%lld\n", x[c][d]);
}