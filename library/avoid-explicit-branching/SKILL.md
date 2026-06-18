---
name: avoid-explicit-branching
description: Use when code uses explicit if-else statements to handle different input cases
---

# avoid-explicit-branching

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses explicit if-else statements to handle different input cases

## Transformation

Replace conditional branches with arithmetic/logical expressions that compute the result directly

## Why it helps

Eliminating explicit branches reduces the number of path constraints KLEE must solve, allowing it to explore the behavior space more efficiently through simpler symbolic expressions

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
- Rule id: `36264af3`
