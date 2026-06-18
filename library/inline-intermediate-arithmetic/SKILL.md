---
name: inline-intermediate-arithmetic
description: Use when intermediate arithmetic results are stored in extra named variables that are only used once in a subsequent expression
---

# inline-intermediate-arithmetic

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When intermediate arithmetic results are stored in extra named variables that are only used once in a subsequent expression

## Transformation

Eliminate single-use temporary variables by inlining their defining expressions directly into the point of use, or reuse an existing input variable instead of declaring new ones

## Why it helps

Fewer symbolic variables and assignments reduce the size of KLEE's symbolic state and constraint expressions, letting the solver evaluate the final expression in one step and produce more general test inputs that exercise the core computation rather than auxiliary bindings

## Example

Before:

```c
#include<stdio.h>
int main(void)
{
	int x,y,z,w,a,b,c;
	scanf("%d %d %d",&x,&y,&z);
	w=y+z;
	a=x-z;
	printf("%d\n",a/w);
	return 0;
}
```

After:

```c
#include<stdio.h>
int main()
{
	int x, y, z;
	scanf("%d %d %d",&x,&y,&z);
	x = x - z;
	printf("%d\n",x/(y+z));
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03548
- Rule id: `c69633ef`
