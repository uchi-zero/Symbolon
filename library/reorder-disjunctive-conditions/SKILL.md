---
name: reorder-disjunctive-conditions
description: Use when multiple conditions are OR'd together and complex expressions appear before simpler ones
---

# reorder-disjunctive-conditions

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When multiple conditions are OR'd together and complex expressions appear before simpler ones

## Transformation

Reorder OR'd conditions to place simpler expressions (like single variable modulo operations) before more complex ones (like sum modulo operations)

## Why it helps

KLEE can more efficiently explore paths when simpler constraints are evaluated first, as they create cleaner path conditions and allow the constraint solver to prune infeasible paths earlier

## Example

Before:

```c
#include<stdio.h>
int main()
{
	int a,b;
	scanf("%d%d",&a,&b);
	if((a+b)%3==0||a%3==0||b%3==0) printf("Possible\n");
	else printf("Impossible\n"); 
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

- Average coverage delta: 0.1852
- Source problems: train_AtCoder_p03657
- Rule id: `af3a07f7`
