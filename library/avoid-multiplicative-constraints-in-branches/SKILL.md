---
name: avoid-multiplicative-constraints-in-branches
description: Use when a branch condition uses multiplication of two symbolic inputs (e.g., s*t < d) creating a nonlinear constraint
---

# avoid-multiplicative-constraints-in-branches

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a branch condition uses multiplication of two symbolic inputs (e.g., s*t < d) creating a nonlinear constraint

## Transformation

Rewrite the condition to use division/ceiling arithmetic on a single symbolic operand (e.g., t >= (d+s-1)/s) so the branch predicate becomes linear in each variable

## Why it helps

KLEE's STP/Z3 backend handles linear integer arithmetic far more efficiently than nonlinear multiplication of symbolics; replacing s*t with division yields branch constraints the solver can satisfy quickly, enabling deeper path exploration and richer test inputs

## Example

Before:

```c
#include <stdio.h>
int d, s, t;
int main() {
	scanf("%d%d%d", &d, &s, &t);
	if (s * t < d)printf("No\n");
	else printf("Yes\n");
}
```

After:

```c
#include<stdio.h>
int d,t,s;
int main()
{
	scanf("%d %d %d", &d, &t, &s);
	d=d+s-1;
	if(t>=(d/s))printf("Yes");
	else printf("No");
}
```

## Evidence

- Average coverage delta: 0.4
- Source problems: train_AtCoder_p02570
- Rule id: `dbe5d2ef`
