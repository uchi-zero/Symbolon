---
name: prefer-narrow-int-and-library-calls-over-nonlinear-arithmetic
description: Use when the code branches on a condition built from nonlinear integer arithmetic (e.g., products like 4*a*b and squared expressions over 64-bit symbolic variables)
---

# prefer-narrow-int-and-library-calls-over-nonlinear-arithmetic

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the code branches on a condition built from nonlinear integer arithmetic (e.g., products like 4*a*b and squared expressions over 64-bit symbolic variables)

## Transformation

Replace the multiplicative/quadratic comparison with an equivalent formulation using narrower integer types and a library call (e.g., sqrtl) so the branch condition becomes a simple linear inequality on symbolic inputs

## Why it helps

KLEE's STP/Z3 backends struggle with nonlinear constraints over wide bitvectors; reducing variable width and pushing nonlinearity into an external/concrete function call yields linear path conditions that the solver can dispatch quickly, enabling broader path exploration

## Example

Before:

```c
#include <stdio.h>

int main()
{
	long long a, b, c; scanf ("%lld %lld %lld", &a, &b, &c);

	if (c - a - b <= 0) puts("No");
	else puts(4 * a * b < (c - a - b) * (c - a - b) ? "Yes" : "No");
	return 0;
}
```

After:

```c
#include <stdio.h>
#include <math.h>
int main(){
  int a,b,c;
  scanf("%d%d%d",&a,&b,&c);
  puts(a+2*sqrtl((long long)a*b)+b<c ? "Yes" : "No");
  }
```

## Evidence

- Average coverage delta: 0.5778
- Source problems: train_AtCoder_p02743
- Rule id: `3bb63182`
