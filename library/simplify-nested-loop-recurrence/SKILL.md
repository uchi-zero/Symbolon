---
name: simplify-nested-loop-recurrence
description: Use when a dynamic programming solution uses a nested loop to compute recurrence relations that sum over multiple previous states
---

# simplify-nested-loop-recurrence

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a dynamic programming solution uses a nested loop to compute recurrence relations that sum over multiple previous states

## Transformation

Replace the nested loop with a simplified recurrence relation that directly references specific previous states (e.g., dp[i] = dp[i-1] + dp[i-3] instead of summing dp[i-j] for all valid j)

## Why it helps

Eliminating the inner loop reduces the number of symbolic constraints and path conditions KLEE must track, making constraint solving more tractable and allowing deeper exploration of the state space

## Example

Before:

```c
#include<stdio.h>
#define ll long long
const ll mod=1e9+7;
ll dp[2005]={0},n;
int main()
{
	scanf("%lld",&n);
	dp[0]=1;
	dp[1]=0;
	dp[2]=0;
	for(ll i=3;i<=n;i++)
	{
		for(ll j=3;i-j>=0;j++)
		{
			dp[i]+=dp[i-j];
			dp[i]%=mod;
		}
//	printf("%lld\n",dp[i]);
		
	}
	printf("%lld\n",dp[n]);
 }
```

After:

```c
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
```

## Evidence

- Average coverage delta: 0.875
- Source problems: train_AtCoder_p02555
- Rule id: `3dd89696`
