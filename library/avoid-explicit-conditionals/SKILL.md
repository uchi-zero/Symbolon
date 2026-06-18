---
name: avoid-explicit-conditionals
description: Use when code uses explicit if-else branches based on input comparisons
---

# avoid-explicit-conditionals

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses explicit if-else branches based on input comparisons

## Transformation

Replace conditional branches with arithmetic/logical expressions that compute the result directly

## Why it helps

Eliminates path explosion by converting multiple execution paths into a single path with symbolic expressions, allowing KLEE to explore the behavior more efficiently without branching

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
- Rule id: `a7c57eed`
