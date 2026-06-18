---
name: extract-expressions-to-named-variables
description: Use when arithmetic expressions (e.g., a/b, a%b, type-casted divisions) are computed inline inside output or other consuming statements
---

# extract-expressions-to-named-variables

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When arithmetic expressions (e.g., a/b, a%b, type-casted divisions) are computed inline inside output or other consuming statements

## Transformation

Hoist each compound expression into its own named intermediate variable assigned before use, so the printf or consumer just references the variables

## Why it helps

Separate assignments give KLEE distinct instructions and SSA-like values for each sub-expression, exposing each operation (e.g., division-by-zero on a/b and a%b) as an independent constraint point rather than nested within a single expression, leading to broader path exploration and simpler per-step constraints

## Example

Before:

```c
#include <stdio.h>

int main(){
int a,b;
double c=0.0;
scanf("%d %d",&a,&b);
c=(double)a/(double)b;
printf("%d %d %f\n",a/b,a%b,c);
return 0;
}
```

After:

```c
#include<stdio.h>
int main(){
	int a, b, d, r;
	double f;
	scanf("%d%d", &a, &b);
	d = a/b;
	r = a%b;
	f = (double)a/b;
	printf("%d %d %.5f\n", d, r, f);
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AIZU_p02399
- Rule id: `27c7e1ec`
