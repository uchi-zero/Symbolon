---
name: hoist-shared-subexpression-and-simplify-branches
description: Use when the same arithmetic subexpression (e.g., X/D, X%D) is recomputed across branches and intermediate state (like K) is mutated before being used in further conditions
---

# hoist-shared-subexpression-and-simplify-branches

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the same arithmetic subexpression (e.g., X/D, X%D) is recomputed across branches and intermediate state (like K) is mutated before being used in further conditions

## Transformation

Compute shared subexpressions once into named variables, avoid mutating inputs, and replace modulo-based parity checks (k%2) with bitwise AND on the original expression (k-y)&1 so conditions reference primary symbolic inputs directly

## Why it helps

KLEE's solver handles constraints faster when path conditions are expressed over fewer, simpler symbolic terms; eliminating redundant divisions/modulos and avoiding variable mutation reduces constraint complexity and lets the same generated inputs cover equivalent paths in other solutions

## Example

Before:

```c
#include <stdio.h>

int main(void)
{
	long long X,K,D,answer,amari;
	scanf("%lld",&X);
	scanf("%lld",&K);
	scanf("%lld",&D);
	if(X<0)X=-1*X;
	if(X/D>K)answer=X-D*K;
	else{
		amari=X%D;
		K-=X/D;
		if(K%2!=0)answer=D-amari;
		else answer=amari;
	}
	printf("%lld",answer);
	return 0;
}
```

After:

```c
#include <stdio.h>
typedef long long LL;

int main()
{	 
	LL x, k, d, y, z;
	
	scanf("%lld%lld%lld", &x, &k, &d);	
	if(x < 0) x = -x;
	y = x / d;
	if(y >= k) z = x - d * k;
	else{
		z = x % d;
		if((k - y) & 1) z = d - z;
	}
	printf("%lld\n", z);
		
	return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p02584
- Rule id: `f0433f8b`
