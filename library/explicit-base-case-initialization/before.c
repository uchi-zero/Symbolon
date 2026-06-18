#include <stdio.h>

typedef long long ll;

const ll MAXS = 2000;
const ll MOD = (ll)1e9 + 7;

ll dp[MAXS];

int main(void)
{
	ll s, i, j;

	scanf("%lld", &s);

	dp[0] = 1;

	for (i = 1; i <= s; i++)
		for (j = 3; j <= s; j++)
			if (i >= j)
				dp[i] = (dp[i] + dp[i - j]) % MOD;

	printf("%lld", dp[s]);
	return 0;
}