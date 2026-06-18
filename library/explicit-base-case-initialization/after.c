#include <stdio.h>
int dp[2020];
int MOD = 1000000007;
int main()
{
	int n;
	scanf("%d", &n);
	dp[1] = 0;
	dp[2] = 0;
	dp[3] = 1;
	for (int i = 4; i <= n; i++)
	{
		dp[i] = (dp[i - 1] + dp[i - 3]) % MOD;
	}
	printf("%d", dp[n]);
}