---
name: prefer-implicit-nonzero-test-over-equality
description: Use when the code branches on an arithmetic equality like (expr % m == 0), forcing KLEE to solve an explicit equality constraint and structuring branches around the rarer (zero) case first
---

# prefer-implicit-nonzero-test-over-equality

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the code branches on an arithmetic equality like (expr % m == 0), forcing KLEE to solve an explicit equality constraint and structuring branches around the rarer (zero) case first

## Transformation

Rewrite the condition to test the value implicitly as a boolean (e.g., if(x%y) handles the nonzero/main case first, with the zero case in else), so the common path is taken on a simple nonzero constraint

## Why it helps

KLEE's solver and path scheduler handle implicit nonzero truthiness branches more uniformly than explicit ==0 equalities; placing the broader (nonzero) constraint on the then-branch yields test inputs that are representative typical values rather than the single degenerate divisor case, improving replay coverage on sibling solutions

## Example

Before:

```c
#include <stdio.h>

int main(void) {
	int n, m;
	scanf("%d %d", &n, &m);

	if (n % m == 0) {
		printf("-1\n");
	}
	else
		printf("%d\n", n);
}
```

After:

```c
#include<stdio.h>

int x,y;

int main(void){
	scanf("%d%d",&x,&y);
	if(x%y)
		printf("%d",x);
	else
		printf("-1");
	return putchar('\n'),0;
}
```

## Evidence

- Average coverage delta: 0.6667
- Source problems: train_AtCoder_p03437
- Rule id: `aa6c9d9c`
