---
name: linearize-multiplicative-constraint
description: Use when a branch condition uses multiplication of symbolic inputs (e.g., a <= b*c), creating a nonlinear constraint
---

# linearize-multiplicative-constraint

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a branch condition uses multiplication of symbolic inputs (e.g., a <= b*c), creating a nonlinear constraint

## Transformation

Rewrite the comparison using division with ceiling adjustment (e.g., (a+c-1)/c <= b) so the constraint becomes linear in the symbolic variables

## Why it helps

KLEE relies on SMT solvers (like STP/Z3) that handle linear integer arithmetic far more efficiently than nonlinear multiplication; linearizing the constraint enables faster path solving and broader coverage during symbolic exploration

## Example

Before:

```c
#include<stdio.h>
int main()
{
	int a,b,c;
	scanf("%d %d %d",&a,&b,&c);
	if(a<=b*c) printf("Yes\n");
	else printf("No\n");
	return 0;
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
- Rule id: `7de2d123`
