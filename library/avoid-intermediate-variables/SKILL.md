---
name: avoid-intermediate-variables
description: Use when intermediate variables are used to store simple arithmetic expressions before using them in conditional statements
---

# avoid-intermediate-variables

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When intermediate variables are used to store simple arithmetic expressions before using them in conditional statements

## Transformation

Inline the arithmetic expressions directly into the conditional statements instead of storing them in intermediate variables

## Why it helps

KLEE can more efficiently reason about constraints when expressions are used directly in conditionals, as it avoids creating additional symbolic variables and simplifies the constraint solving process

## Example

Before:

```c
#include<stdio.h>
int main(void)
{
    int a,b,c;
	scanf("%d %d",&a,&b);
	c=a+b;
	if(c%3==0||a%3==0||b%3==0) printf("Possible\n");
	else printf("Impossible\n");
	return 0;
}
```

After:

```c
#include <stdio.h>
int main()
{
    int A,B;
    scanf("%d%d",&A,&B);
    if((A%3==0)||(B%3==0)||((A+B)%3==0))
        printf("Possible");
    else
        printf("Impossible");
}
```

## Evidence

- Average coverage delta: 0.3241
- Source problems: train_AtCoder_p03657
- Rule id: `2fc4fc8e`
