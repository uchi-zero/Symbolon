---
name: simplify-multiplicative-constraints
description: Use when multiple equality checks involve multiplication of symbolic variables
---

# simplify-multiplicative-constraints

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When multiple equality checks involve multiplication of symbolic variables

## Transformation

Replace multiplicative constraints with simpler linear constraints on individual variables

## Why it helps

KLEE's constraint solver handles linear constraints more efficiently than non-linear multiplicative ones, enabling faster path exploration and better coverage

## Example

Before:

```c
#include<stdio.h>
int main()
{
	int m,n;
	scanf("%d%d",&m,&n);
	if(m*n==3 ||m*n==1 ||m*n==9) printf("Yes");
	else printf("No");
	return 0;
}
```

After:

```c
#include<stdio.h>
int main(void){
  int a,b;
  scanf("%d%d",&a,&b);
  if(a==2||b==2)
    printf("No");
  else
    printf("Yes");
  return 0;
}
```

## Evidence

- Average coverage delta: 0.1111
- Source problems: train_AtCoder_p03260
- Rule id: `bc61a7fa`
