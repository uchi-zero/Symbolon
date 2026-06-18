---
name: narrow-integer-types-for-loop-bounds
description: Use when loop bounds and array indices are declared as long long (64-bit) symbolic inputs, forcing KLEE to reason about wide bitvector constraints for every loop iteration and index computation
---

# narrow-integer-types-for-loop-bounds

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When loop bounds and array indices are declared as long long (64-bit) symbolic inputs, forcing KLEE to reason about wide bitvector constraints for every loop iteration and index computation

## Transformation

Declare input variables that serve as loop bounds and array indices as int (32-bit) instead of long long, keeping long long only for values where overflow matters (e.g., modular arithmetic accumulators)

## Why it helps

Smaller bitwidth symbolic variables produce simpler SMT constraints, allowing KLEE's solver to enumerate feasible loop bounds faster and explore more paths within the time budget, improving coverage of the nested-loop body

## Example

Before:

```c
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
```

After:

```c
#include <stdio.h>
#define MOD 998244353
long long int DP[3010][3010];
int main()
{
	int a,b,c,d;
	scanf("%d%d%d%d",&a,&b,&c,&d);
	
	long long int C = 1;
	for(int j=c;j>=a;j--)
	{
		DP[d][j] = C;
		C*=d;
		C%=MOD;
	}
	
	for(int i=d-1;i>=b;i--)
	{
		C = 0;
		for(int j=c;j>=a;j--)
		{
			DP[i][j] = (C + j*DP[i+1][j])%MOD;
			
			//C *= i;
			C += DP[i+1][j];
			C *= i;
			C %= MOD;
			//printf("%d %d %lld!!\n",i,j,DP[i][j]);
		}
	}
	
	printf("%lld",DP[b][a]);
}
```

## Evidence

- Average coverage delta: 0.1754
- Source problems: train_AtCoder_p02634
- Rule id: `05367afd`
