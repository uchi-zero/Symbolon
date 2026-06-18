---
name: avoid-complex-modulo-conditions
description: Use when code uses complex arithmetic expressions with modulo operations in conditional statements, especially involving multiple variables
---

# avoid-complex-modulo-conditions

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses complex arithmetic expressions with modulo operations in conditional statements, especially involving multiple variables

## Transformation

Refactor to use floating-point arithmetic with rounding checks instead of integer arithmetic with modulo operations

## Why it helps

Complex modulo expressions create non-linear constraints that are difficult for SMT solvers to reason about, while floating-point comparisons with rounding create simpler constraints that KLEE can explore more efficiently

## Example

Before:

```c
#include <stdio.h>
typedef long long ll;

int main(void) {
  ll n, i, j;
  scanf("%lld", &n);
  for(i = 1; i <= 3500; ++i) for(j = 1; j <= 3500; ++j) if(4 * i * j - i * n - j * n > 0 && !((n * i * j) % (4 * i * j - i * n - j * n))) {
    printf("%lld %lld %lld", i, j, (n * i * j) / (4 * i * j - i * n - j * n));
    return 0;
  }
}
```

After:

```c
#include <stdio.h>
#include <math.h>

int main(void){
	double n,i,j,k,ans,ap;
	scanf("%lf",&n);
	ans = 4/n;
	for (i=1;i<5000;i++)
	{
		for (j=1;j<5000;j++)
		{
			ap = ans - 1/i - 1/j;
			if (ap >0)
			{
				ap = 1.0/ap;
				ap = roundl(ap * 10000000) / 10000000;
				if ((int)(ap) == ap) 
				{
					printf("%d %d %d",(int)(i),(int)(j),(int)(ap));
					return 0;
				}
			}
		}
	}
	return 0;
}
```

## Evidence

- Average coverage delta: 0.5
- Source problems: train_AtCoder_p03583
- Rule id: `946a26c9`
