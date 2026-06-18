---
name: explicit-base-case-initialization
description: Use when dynamic programming arrays rely on implicit zero-initialization and compute base cases through general recurrence relations
---

# explicit-base-case-initialization

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When dynamic programming arrays rely on implicit zero-initialization and compute base cases through general recurrence relations

## Transformation

Explicitly initialize all base cases before the main computation loop, avoiding conditional checks or computations for small indices

## Why it helps

KLEE can directly use concrete base case values without exploring paths through recurrence relations for small indices, reducing path explosion and simplifying constraints

## Example

Before:

```c
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

- Average coverage delta: 0.6972
- Source problems: train_AtCoder_p02555
- Rule id: `e1a0d8a2`
