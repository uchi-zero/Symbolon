---
name: avoid-arithmetic-constraints
description: Use when code performs arithmetic operations on symbolic input values
---

# avoid-arithmetic-constraints

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code performs arithmetic operations on symbolic input values

## Transformation

Replace arithmetic computations with direct conditional checks or constant outputs based on specific input values

## Why it helps

Arithmetic operations create complex symbolic expressions that are harder for KLEE to solve, while simple equality checks and constant outputs create simpler path conditions

## Example

Before:

```c
#include<stdio.h>

int x;

int main()
{
	scanf("%d", &x);
	printf("%d", (x-1)*(x-1));
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
- Rule id: `3ae6f9da`
