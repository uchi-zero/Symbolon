---
name: eliminate-explicit-branches
description: Use when code uses explicit if-else statements or conditional branches to handle different input cases
---

# eliminate-explicit-branches

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses explicit if-else statements or conditional branches to handle different input cases

## Transformation

Replace conditional branches with arithmetic or logical expressions that compute the result directly

## Why it helps

Eliminating branches reduces path explosion - KLEE explores fewer paths since there's only one execution path through the code instead of multiple branches

## Example

Before:

```c
#include<stdio.h>

int main()
{
	int n;
	scanf("%d", &n);
	if(n == 1) printf("0\n");
	else printf("1\n");
}
```

After:

```c
#include<stdio.h>
int main(){puts(getchar()-48?"0":"1");}
```

## Evidence

- Average coverage delta: 0.1429
- Source problems: train_AtCoder_p02552
- Rule id: `8f7bbcf1`
