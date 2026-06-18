---
name: inline-arithmetic-avoid-mutating-operands
description: Use when input variables are mutated in place via compound assignments before being used in a later expression, creating intermediate symbolic state
---

# inline-arithmetic-avoid-mutating-operands

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When input variables are mutated in place via compound assignments before being used in a later expression, creating intermediate symbolic state

## Transformation

Keep input variables as close to their original symbolic form as possible and inline arithmetic directly into the expression that uses it (e.g., replace 'b+=c; a/b' with 'a/(y+z)')

## Why it helps

Fewer mutations on symbolic variables mean simpler symbolic expressions and constraint terms for KLEE, allowing the solver to reason about original inputs directly and explore paths (like division-by-zero on y+z) without tracking aliasing through reassignments

## Example

Before:

```c
#include <stdio.h>
int main(){
int a,b,c;
  scanf("%d%d%d",&a,&b,&c);
  a-=c;
  b+=c;
  printf("%d\n",a/b);
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
- Rule id: `bcfc45ca`
